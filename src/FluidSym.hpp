#pragma once
//
//  FluidSym.hpp
//  FluidSym
//
//  Created by David Crooks on 20/08/2016.
//
//



#include "ofMain.h"

class FluidSym  {
    
    public:
        void setup(bool _seamless);//uses window dimensions.
        void setup(bool _seamless,int _width, int _height); //If seamless==true, width and height must be power of 2.
        void update(float time,float dt);
        void draw();
        void draw(int x,int y);
        ofFbo *fbo = new ofFbo();
    
        double advectionRate;
        double flowEvolutionRate;
        double flowScale;
    
        int width;
        int height;
    
        bool seamless = true;
    int counter;
    private:
        ofImage startImage;
        ofShader shader;
        ofFbo *fbo2 = new ofFbo();
        bool powerOfTwo(int n) ;
};

