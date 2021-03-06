//
//  FluidSym.cpp
//  FluidSym
//
//  Created by David Crooks on 20/08/2016.
//
//

#include "FluidSym.hpp"
#include "ofApp.h"

/*
    A simple generative fluid simulation. 
    All the magic happens in fluid.frag and fluidSeamless.frag.
    It takes the CURL of perlin noise to generate a divergence-free flow field, which it then uses to shift pixels around. See fluidSeamless.frag.
*/

void FluidSym::setup(bool _seamless) {
    setup( _seamless,ofGetWidth(), ofGetHeight());
}

void FluidSym::setup(bool _seamless,int _width, int _height){
    seamless = _seamless;
    width = _width;
    height = _height;
    
    
    advectionRate = 20.0;
    flowEvolutionRate = 0.08;
    flowScale = 0.3;
    
    
    /*
        advectionRate = 3.0;
        flowEvolutionRate = 0.05;
        flowScale = 0.7;
    */
    
    
    if(seamless) {
        //For seamless mode we want to use GL_REPEAT so we need to use power of two textures.
        if(!powerOfTwo(width) ){
            cout << "Warning! width (" << width << ")  must be power of two. Using 512." << endl;
            width = 512;
        }
        if(!powerOfTwo(height)){
            cout << "Warning! height (" << height << ") must be power of two. Using 512." << endl;
            height = 512;
        }
        ofDisableArbTex();
        shader.load("shaders/fluid.vert","shaders/fluidSeamless.frag");
    }
    else {
        shader.load("shaders/fluid");
    }
    
    auto image = ofImage("images/startimage3.png");
    
    fbo->allocate(width,height);
    fbo->begin();
        ofClear(255,255,255, 255);
        //image.draw(0,0,width,height);
        fbo->getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    fbo->end();
    
    fbo2->allocate(width,height);
    fbo2->begin();
        ofClear(255,255,255, 255);
        fbo2->getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    fbo2->end();
}

bool FluidSym::powerOfTwo(int n) {
    return (n>0 && ((n & (n-1)) == 0));
}

//--------------------------------------------------------------
void FluidSym::update(float time,float dt){
    //swap framebuffers
    //we're ping-ponging between framebuffers so we need to swap them around each frame.
    std::swap(fbo,fbo2);
       
    
    fbo->begin();
        shader.begin();
            shader.setUniform1f("time", flowEvolutionRate*time);
            shader.setUniform1f("advectionRate", advectionRate*dt);
            shader.setUniform2f("size", width,height);
            shader.setUniform2f("inverseSize", 1.0/width,1.0/height);
        
            if(seamless){
                //we obtain seamless noise by embedding a torus in 4-space and using 4d perlin noise.
                //we can tune the noise with the radii of the torus. We can adjust for the proportions of the fluid.
                shader.setUniform2f("torusRadii", (width/height)*flowScale,flowScale);
                
            }
            else {
                shader.setUniform1f("flowScale",flowScale);
            }

            fbo2->draw(0,0);
        shader.end();
    fbo->end();
}

//--------------------------------------------------------------
void FluidSym::draw(){
    fbo->draw(0,0);
}

void FluidSym::draw(int x,int y){
    fbo->draw(x,y);
}


