#pragma once
//
//  Partical.hpp
//  FluidSym
//
//  Created by David Crooks on 24/08/2016.
//
//


#include "ofMain.h"

class Particle {
    public:
        virtual void setup(int _width, int _height,double _mass,double _flowPerturbationAmount,double drag);
        virtual void update(ofVec2f appliedForce);
        virtual void update();
        void draw();
    
        ofVec2f position;
        ofVec2f diffPosition;
        ofVec2f lastPosition;
        ofVec2f velocity;
        
        double mass;
        double dragCoeffiecent;
        double flowPerturbationAmount;
        ofImage image;
        double size;
        ofColor color;
    
        int width;
        int height;
    
        bool drawsSeamlessly = true;
        bool wrapsXY = true;
        double varyingSize;
    
        double noiseStartTime;
    
    private:
        void drawSeamless(ofVec2f drawPoint,double drawSize);
        void drawParticle(ofVec2f drawPoint,double drawSize);
    
        ofVec2f flowPerturbation();
        ofVec2f drag();
};

