//
//  Partical.cpp
//  FluidSym
//
//  Created by David Crooks on 24/08/2016.
//
//

#include "Particle.hpp"

void Particle::setup(int _width, int _height,double _mass, double _flowPerturbationAmount, double _drag){
    flowPerturbationAmount =_flowPerturbationAmount;
    dragCoeffiecent = _drag;
    width = _width;
    height = _height;
    mass = _mass;
    image.load("images/particle.png");
    
    size = 50;
    color = ofColor(255,0,0,255);
    
    position = ofVec2f(ofRandom(width), ofRandom(height));
    velocity = 0.1*ofVec2f(-width/2 + ofRandom(width),-height/2 +  ofRandom(height));
    
    lastPosition = position;
    
    noiseStartTime = ofRandom(5.0);
}

void Particle::update(){
    update(ofVec2f());
}

void Particle::update(ofVec2f appliedForce){
    ofVec2f totalForce = appliedForce + flowPerturbation() + drag();
    
    lastPosition = position;
    double dt = ofGetLastFrameTime();
    ofVec2f acceleration = totalForce/mass;
    
    velocity += acceleration *dt;
    position += velocity * dt;
    if(wrapsXY){
        position.x = ofWrap(position.x, 0, width);
        position.y = ofWrap(position.y, 0, height);
    }
    diffPosition = velocity * dt;
}


ofVec2f Particle::flowPerturbation() {
    //This is the same idea as the fluid simulation: Create a flow field from the curl of noise. See comments in the fragment shader.
    
    double time = noiseStartTime + ofGetElapsedTimef();
    double x = position.x/width;
    double y =  position.y/height;
    double epsilon = 0.05;
    
    //derivitives of noise
    double dNdx = 0.5*(ofNoise(x+epsilon,y,time) - ofNoise(x-epsilon,y,time))/epsilon;
    double dNdy = 0.5*(ofNoise(x,y+epsilon,time) - ofNoise(x,y-epsilon,time))/epsilon;
    
    //curl of noise
    ofVec2f curlN = ofVec2f(dNdy, -dNdx);
    
    return curlN*flowPerturbationAmount;
}

ofVec2f Particle::drag() {
    double speed = velocity.length();
    ofVec2f v = velocity.getNormalized();
    
    return  - dragCoeffiecent * speed * speed * v ;
}


void Particle::draw(){
    double length = diffPosition.length();
    int n_draw_points = ceil(length/3);

    auto dp = diffPosition/n_draw_points ;
    ofSetColor(color);
    for(int i = 1; i <= n_draw_points; i++){
        auto p = lastPosition + i*dp;
        
        if(drawsSeamlessly){
            drawSeamless(p,varyingSize);
        }
        else {
            drawParticle(p, varyingSize);
        }
    }
}

void Particle::drawParticle(ofVec2f drawPoint,double drawSize) {
    int imageX = drawPoint.x - drawSize * 0.5;
    int imageY = drawPoint.y - drawSize * 0.5;
    
    image.draw(imageX, imageY, drawSize, drawSize);
}
/*
    If we want to maintain a seamless image we need to draw each point multiple times.
    For example, a point very close to the lower left corner should draw in all four corners.
*/
void Particle::drawSeamless(ofVec2f drawPoint,double drawSize) {
    ofVec2f p = ofVec2f();
    
    for(int i = -1; i<=1 ;i++) {
        p.x = drawPoint.x + i*width;
        
        for(int j = -1; j<=1 ;j++) {
           
            p.y = drawPoint.y + j*height;
            drawParticle(p, varyingSize);
        }
    }
}
