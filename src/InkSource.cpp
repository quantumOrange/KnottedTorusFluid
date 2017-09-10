//
//  InkSource.cpp
//  TrefoilKnot
//
//  Created by David Crooks on 25/08/2016.
//
//

#include "InkSource.hpp"


void InkSource::update(ofVec2f appliedForce,float time,float dt) {
    Particle::update(appliedForce,time,dt);
    
    //we add a random start time so that each ink particle appears to change size independently, rather than all in sync.
    varyingSize =  size*ofNoise(noiseStartTime + sizeRate*time);
    
    //we're going to slowly cycle around the color wheel.
    double hue = ofWrap(startHue ,0,360);
   
    color.setHueAngle(hue);
   
    
    if(color.a < 250){
        color.a += 3;
    }
    
}

void InkSource::update(float time,float dt) {
    update(ofVec2f(),time, dt);
}

void InkSource::setup(int _width, int _height,double _mass, double _flowPerturbationAmount, double _drag){
    Particle::setup(_width,_height,_mass,  _flowPerturbationAmount, _drag);
    color.a = 0;
    sizeRate = 0.1;
    hueRate = 0.5;
    size = 10+ofRandom(80);
    startHue = ofWrap(  210 + ofRandom(60),0,360); //start in the red-orange range.
    
    color.setBrightness(255.0/2.0);
    color.setSaturation(2*255.0/3.0);
}
