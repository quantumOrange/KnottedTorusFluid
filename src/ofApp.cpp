#include "ofApp.h"

/*
    A fluid simulation running on a torus tied in a knot.
*/

float bumpTimer = 0;
float shouldBumb = true;
bool  hasChangedParticles = false;
bool  hasChangedParticlesAgain = false;

void ofApp::setup(){
    
    //We create and run fluid simulation and we have surface in the shape of a knotted torus.
    //We'll use the fluid as a texture for the knot.
    fluid = new FluidSym();
    knotSurface = new KnotSurface();
    
    //We create Inksources, which are just 2d-particles that float around in the fluid, leaving a trail of color.
    auto numberOfInksources = 30;
    inkSources.assign(numberOfInksources,InkSource());
    
    ofDisableArbTex();
    ofSetFrameRate(60);
    ofBackground(0,0,0);
    ofSetSmoothLighting(true);
    ofEnableAlphaBlending();
    knotSurface->setup();
    
    //we want the fluid to have similar proportions to the knotted torus - which is longer along the length of the knot than the cicumferance.
    //But we have to stick to power of 2 so we can use GL_REPEAT.
    fluid->setup(true,2048,512);
    
    //We are going to drop some ink into the fluid so that we can see some fluid dynamics.
    //These inksource particles will leak ink into the fluid as they drift around.
    //They start off red-yellow and slowly shift hue over time.
    double mass = 1.0;
    double flow = 45.0;
    double drag = 0.002;
    
    ofImage *image = new ofImage();
    image->load("images/particle.png");
    for_each(inkSources.begin(), inkSources.end(),[=](InkSource& p) {p.setup(fluid->width,fluid->height,1.0,40.0,0.005);} );
    for_each(inkSources.begin(), inkSources.end(),[=](InkSource& p) {p.image = image;} );
    
    //set up the camera
    cam.setScale(1,-1,1);
    camDistance = 1024;
    camTheta = camTheta0-1.0;
    camPhi = camPhi0;
    camDeltaAngle = 0.02;
    camDeltaSpace = 0.0;
    camCentre = ofVec3f(100, 25,0);
    
    //set up the lights
    auto color1 = ofColor();
    color1.setHsb(0, 30, 255);
    color1.setHueAngle(45);
    
    auto color2 = ofColor();
    color2.setHsb(0, 150, 100);
    color2.setHueAngle(45);
    
    auto color3 = ofColor();
    color3.setHsb(0, 200, 255);
    color3.setHueAngle(225);
    
    auto color4 = ofColor();
    color4.setHsb(0, 255, 150);
    color4.setHueAngle(225);
    
    
    light.setDiffuseColor( color1 );
    light.setSpecularColor( color2);
    
    light2.setDiffuseColor( color3);
    light2.setSpecularColor(color4);
    
    light3.setDiffuseColor( color1 );
    light3.setSpecularColor( color2);
    
    
    float at = 0.0000005;
    light.setAttenuation(0.f, 0.f, at);
    light2.setAttenuation(1.0, 0.f,0);
    light3.setAttenuation(0.f, 0.f,  0.0000001);
  
    
    lightMainPosition = ofVec3f(400,-160,1150);
    lightDisplacement = ofVec3f(-1500,0,-1600);
    
    //set up video recording
    /*
        vidInput = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
        vidGrabber.setGrabber(vidInput);
        vidGrabber.initGrabber(1280, 720);
        vidRecorder.setFfmpegLocation(ofFilePath::getAbsolutePath("ASSETS/ffmpeg"));
    */
    
}


void ofApp::update(){
    //time line
    float fadeoutDuration = 2.0;//5
    float startParticlesTime = 4.0;
    float changeParticlesTime = 19.0;
    float bumpStarttime = 32.0;
    float changeParticlesAgainTime = 34.0;
    float endTime = 60;
    float fadeOutTime = endTime - fadeoutDuration;
    
    
    //timer
    float time,dt;
    
    if(REAL_TIME) {
        time = ofGetElapsedTimef();
        dt = ofGetLastFrameTime();
    }
    else{
        float framePerSec = 30.0;
        time = ofGetFrameNum()/framePerSec;
        dt = 1.0/framePerSec;
    }
    
    if(time>endTime){
        std::exit(0);
    }
   
    //Lights!!!
    ofVec3f lightPos  = lightMainPosition;
    

    //fade in the lights
    if (time < fadeoutDuration) {
        
        float s = 1.0 - time/fadeoutDuration;
        knotSurface->bumpAmount = (1-s)*initialBumpAmount;
        cout << (1-s)*initialBumpAmount << endl;
        lightPos += s*lightDisplacement;
        fadeLights(1-s);
    }
    
    //fade out the lights
    if(time > fadeOutTime){
        float s = (time-fadeOutTime)/fadeoutDuration;
        lightPos += s*lightDisplacement;
        fadeLights(1-s);
    }
    
    light.setPosition(lightPos);
    light2.setPosition(ofVec3f(550,250,-50));
    
    //Camera!!!
    ofVec3f camDirection(sin(camTheta)*cos(camPhi),sin(camTheta)*sin(camPhi),cos(camTheta));
    ofVec3f camPosition = camCentre + camDirection * camDistance;
    cam.setPosition(camPosition);
    cam.lookAt(camCentre);
    light3.setPosition(0.7*camPosition + ofVec3f(100,-100,0) );
    
    
    //Action!!!
    if(time>startParticlesTime && updateParticles){
        if(time>changeParticlesTime && !hasChangedParticles){
            
            hasChangedParticles = true;
            cout << "change!" <<endl;
            for_each(inkSources.begin(), inkSources.end() ,[=](InkSource& p) {p.color.setSaturation(0);p.color.setBrightness(255);p.color.a = 0;p.size = 10 + ofRandom(300);} );
            
        }
        
        if(time>changeParticlesAgainTime && !hasChangedParticlesAgain){
            
            hasChangedParticlesAgain = true;
            cout << "change Aagin!" <<endl;
            for_each(inkSources.begin(), inkSources.end() ,[=](InkSource& p) {p.startHue = ofWrap(240 + ofRandom(80),0,360);p.color.setBrightness(2*255.0/3);p.color.setSaturation(2*255.0/3.0);;p.color.a = 0;p.size = 10 + ofRandom(80);} );
            
        }
        
        //we're calling draw on the particles, but this is really updateing the fluid. we're not drawing to screen.
        //This needs to happen before calling update on the fluid so that theres a bit of diffusion before displaying to screen.
        fluid->fbo->begin();
            for_each(inkSources.begin(), inkSources.end(),[time,dt](InkSource& p) {p.update(time,dt);} );
            for_each(inkSources.begin(), inkSources.end(),[](InkSource& p) {p.draw();} );
        fluid->fbo->end();
    }
  
    if(updateFluid) fluid->update(time,dt);
    
    if(time > bumpStarttime && shouldBumb){
        knotSurface->bumpAmount = initialBumpAmount + (1.0 - initialBumpAmount)*(0.5-0.5*cos(2*bumpTimer));
        //cout << knotSurface->bumpAmount <<endl;
        bumpTimer += dt;
        if(2*bumpTimer > TWO_PI){
            cout << "end bump" << endl;
            shouldBumb = false;
        }
    }
    
    if(updateKnot)  knotSurface->update(time,dt);
    
    if(!REAL_TIME && recording)
    {
        auto str = "frames/" + ofToString(ofGetFrameNum(),4,'0') + ".png";
        cout << str << endl;
        ofSaveScreen(str);
    }
    
    
}

void ofApp::fadeLights(float b){
    auto amb = ofColor();
    amb.setBrightness(70*b);
    ofSetGlobalAmbientColor(amb);
    
    auto c = light.getDiffuseColor();
    c.setBrightness(b*0.85);
    light.setDiffuseColor(c);
    auto c2 = light.getSpecularColor();
    c2.setBrightness(b*0.1);
    light.setSpecularColor(c2);
    
    auto c3 = light2.getDiffuseColor();
    c3.setBrightness(b*0.4);
    light2.setDiffuseColor(c3);
    auto c4 = light2.getSpecularColor();
    c4.setBrightness(b*0.2);
    light2.setSpecularColor(c4);
    
    auto c5 = light3.getDiffuseColor();
    c5.setBrightness(b*0.6);
    light3.setDiffuseColor(c5);
    auto c6 = light3.getSpecularColor();
    c6.setBrightness(b*0.1);
    light3.setSpecularColor(c6);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(DEBUG_FLUID) fluid->draw();
    
    
    
    if(DEBUG_FLUID) return;
    
    //pass the fluid texture to the knot
    ofTexture tex = fluid->fbo->getTexture();
    knotSurface->texture = &tex;
   
    ofEnableDepthTest();
        cam.begin();
            ofEnableLighting();
                light.enable();
                light2.enable();
                knotSurface->draw();
            ofDisableLighting();
        cam.end();
    ofDisableDepthTest();
    
}

//--------------------------------------------------------------
 
void ofApp::keyPressed(int key){
    cout << ofGetFrameRate() << endl;
    
    
    if(key == 's'){
        // s  to save screen
        doSaveScreen = !doSaveScreen;
    }

    if(key == 'r'){
        // r to record or stop recording
        recording = !recording;
    }
    
    if(key == 'f'){
        //f to log framerate
        cout << ofGetFrameRate() << endl;
    }
    
    if(key == 'c'){
        //c to clear the fluid
        fluid->fbo->begin();
            ofClear(255,255,255, 255);
        fluid->fbo->end();
    }
    
    //a and z for zoom
    if(key == 'a'){
        camDistance += 5;
        cout << camDistance  << endl;
    }
    
    if(key == 'z'){
        camDistance  -= 5;
        cout << camDistance  << endl;
    }
    
    
    
    
}
//--------------------------------------------------------------
/*
void ofApp::startRecord() {
    
    bRecording = true;
    
    if(bRecording && !vidRecorder.isInitialized()) {
        
        vidRecorder.setup("your-file-name.mp4", vidGrabber.getWidth(), vidGrabber.getHeight(), 30, 44100, 2);
        
    }
    
}
//--------------------------------------------------------------
void ofApp::stopRecord() {
    
    bRecording = false;
    
    vidRecorder.close();
    
}
*/
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    int dx = lastMouseX - x;
    int dy = lastMouseY - y;
    
    lastMouseX = x;
    lastMouseY = y;
    
    camTheta += dx*camDeltaAngle;
    camPhi += dy*camDeltaAngle;
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    lastMouseX = x;
    lastMouseY = y;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

/*
    Logs the vertex, fragment and geomtry shaders
*/

void logShaders() {
    
    auto renderer = ofGetCurrentRenderer();
    
    auto programableRenderer = std::static_pointer_cast<ofGLProgrammableRenderer>(renderer);
    
    auto shader = programableRenderer->getCurrentShader();
    
    auto vertexShader = shader.getShaderSource(GL_VERTEX_SHADER);
    auto fragmentShader = shader.getShaderSource(GL_FRAGMENT_SHADER);
    auto geomtryShader = shader.getShaderSource(GL_GEOMETRY_SHADER_EXT);
    
    
    cout  <<  "-----------------------------------------------------------------------------------" << endl;
    cout  <<  vertexShader << endl;
    cout  <<  "-----------------------------------------------------------------------------------" << endl;
    cout  <<  "-----------------------------------------------------------------------------------" << endl;
    cout  <<  fragmentShader << endl;
    cout  <<  "-----------------------------------------------------------------------------------" << endl;
    cout  <<  "-----------------------------------------------------------------------------------" << endl;
    cout  <<  geomtryShader << endl;
    cout  <<  "-----------------------------------------------------------------------------------" << endl;
}


