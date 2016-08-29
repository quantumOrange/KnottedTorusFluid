//
//  InkSource.cpp
//  TrefoilKnot
//
//  Created by David Crooks on 25/08/2016.
//
//

#include "InkSource.hpp"


void InkSource::update(ofVec2f appliedForce) {
    Particle::update(appliedForce);
    
    //we add a random start time so that each ink particle appears to change size independently, rather than all in sync.
    varyingSize =  size*ofNoise(noiseStartTime + sizeRate*ofGetElapsedTimef());
    
    //we're going to slowly cycle around the color wheel.
    double hue = ofWrap(startHue+hueRate*ofGetElapsedTimef(),0,360);
    color.setHueAngle(hue);
}

void InkSource::update() {
    update(ofVec2f());
}

void InkSource::setup(int _width, int _height,double _mass, double _flowPerturbationAmount, double _drag){
    Particle::setup(_width,_height,_mass,  _flowPerturbationAmount, _drag);
    sizeRate = 0.1;
    hueRate = 0.5;
    startHue = ofRandom(60); //start in the red-orange range.
    color.setBrightness(ofRandom(255));
    color.setSaturation(ofRandom(255));
}