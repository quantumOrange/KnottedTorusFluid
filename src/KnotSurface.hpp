#pragma once

//
//  KnotSurface.hpp
//  TrefoilKnot
//
//  Created by David Crooks on 18/08/2016.
//
//


#include "ofMain.h"


class KnotSurface {
    public:
        void setup();
        void update(float time,float dt);
        void draw();
        ofTexture *texture; //you can pass me a texture if you like.
        int quality;
        float bumpAmount;
    private:
    
        void updateLights() const;
        void updateMaterial() const;
        int totalKnotPoints;
        int totalLoopPoints;
        double pathScale;
        double tubeRadius;
        double knotTorusRadiusRatio;
        double radialDeformation(double theta,double phi, double t);
        double pulse(double theta);
        double noiseRate; //rate of  evolution of perlin noise, used to randomly deform the KnotSurface
        ofVec3f pathPoint(double s, double t);
        ofVec3f trefoilKnotPoint(double s, double t);
        ofVec3f normal1(double s, double t);
        ofVec3f normal2(double s, double t);
    
        ofVec2f textureOrigin;
        ofVec2f textureDrift;
    
        vector<vector<ofVec3f>> normals;
        vector<vector<ofVec3f>> surfacePoints;
        vector<vector<ofVec3f>> surfaceNormals;
        vector<vector<ofVec3f>> surfaceTangents;
        vector<ofVec3f> pathPoints;
        ofMaterial material;
    
        ofShader shader;
        ofFloatColor ambient;
        ofFloatColor diffuse;
        ofFloatColor specular;
        ofFloatColor emissive;
    
    
        ofVboMesh mesh;
    
        float shininess;
    
    int counter;
};



