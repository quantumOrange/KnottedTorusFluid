#pragma once
//
//  InkSource.hpp
//  TrefoilKnot
//
//  Created by David Crooks on 25/08/2016.
//
//

#include "ofMain.h"
#include "Particle.hpp"

class InkSource:public Particle {
    public:
        void setup(int _width, int _height,double _mass,double _flowPerturbationAmount,double drag) override;
        void update(ofVec2f force,float time,float dt) override;
        void update(float time,float dt) override;
       
        double startHue;
        double hueRate;
        double sizeRate;
    
};

