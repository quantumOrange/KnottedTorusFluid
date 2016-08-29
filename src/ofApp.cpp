#include "ofApp.h"







/*
    A fluid simulation on a torus tied in a knot.
*/

//--------------------------------------------------------------

void ofApp::setup(){
    
    fluid = new FluidSym();
    knotSurface = new KnotSurface();
    auto numberOfInksources = 14;
    inkSources.assign(numberOfInksources,InkSource());
    
    ofDisableArbTex();
    ofSetFrameRate(60);
    ofBackground(0,0,0);
    ofSetSmoothLighting(true);
    
    knotSurface->setup();
    
    //we want the fluid to have similar proportions to the knotted torus - which is longer along the length of the knot than the cicumferance.
    //But we have to stick to power of 2 so we can use GL_REPEAT.
    fluid->setup(true,2048,512);
    
    //We are going to drop some ink into the fluid so that we can see some fluid dynamics.
    //These inksource particles will leak ink into the fluid as they drift around.
    //They start off red-yellow and slowly shift hue over time.
    for_each(inkSources.begin(), inkSources.end(),[=](InkSource& p) {p.setup(fluid->width,fluid->height,1.0,40.0,0.005);} );
    
    
    //add a large splash of white
    inkSources[0].color.setSaturation(0);
    inkSources[0].color.setBrightness(255);
    inkSources[0].size = 150;
    inkSources[1].color.setSaturation(0);
    inkSources[1].color.setBrightness(255);
    inkSources[1].size = 200;
    
    //add a little of a dark complimentary color for contrast.
    inkSources[2].color.setBrightness(70);
    inkSources[2].color.setSaturation(255);
    inkSources[2].startHue = 210; //default start color averages at 30, so 180+30 is the complimentary color.
    
    
    //set up the camera
    cam.setScale(1,-1,1);
    camDistance = 1024;
    camTheta = camTheta0;
    camPhi = camPhi0;
    camDeltaAngle = 0.05;
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
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //Lights!
    light.setPosition(ofVec3f(400,-160,1150));
    light2.setPosition(ofVec3f(550,250,-50));
    
    double brightness = 0.5;
    ofSetGlobalAmbientColor(ofFloatColor(brightness,brightness,brightness));
     
    //Camera!
    ofVec3f camDirection(sin(camTheta)*cos(camPhi),sin(camTheta)*sin(camPhi),cos(camTheta));
    ofVec3f camPosition = camCentre + camDirection * camDistance;
    cam.setPosition(camPosition);
    cam.lookAt(camCentre);
    
   
    //Action!
    fluid->update();
    for_each(inkSources.begin(), inkSources.end(),[](InkSource& p) {p.update();} );
    knotSurface->update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(DEBUG_FLUID) fluid->draw();
    
    fluid->fbo->begin();
        for_each(inkSources.begin(), inkSources.end(),[](InkSource& p) {p.draw();} );
    fluid->fbo->end();
    
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
    if(key == 'r'){
        //reset cammera
        camTheta =camTheta0;
        camPhi = camPhi0;
    }
    if(key == 'f'){
        //framerate
        cout << ofGetFrameRate() << endl;
    }
    if(key == 'c'){
        //clear the fluid
        fluid->fbo->begin();
            ofClear(255,255,255, 255);
        fluid->fbo->end();
    }
}

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
    
    cout << "camera angles : ( " << camTheta << "," << camPhi << ")" << endl;
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
