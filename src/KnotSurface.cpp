//
//  KnotSurface.cpp
//  TrefoilKnot
//
//  Created by David Crooks on 18/08/2016.
//
//
#include "ofApp.h"
#include <vector>
#include "KnotSurface.hpp"


/* 
    This class constructs and draws a torus tied in a trefoil knot.
*/


void KnotSurface::setup() {
    noiseRate = 0.05; //rate of  evolution of perlin noise, used to randomly deform the KnotSurface.
    totalKnotPoints = 250;
    totalLoopPoints = 50;
    pathScale = 300;
    tubeRadius = 80;
    knotTorusRadiusRatio = 0.5;
    textureDrift  = 0.0001*ofVec2f(4.0,1.0);
    textureOrigin = ofVec2f();
    pathPoints.assign(totalKnotPoints+1,ofVec3f());
    //for each point on the knot we have an orthoganal pair of normals which define a plane perpendicular to the path at that point.
    //We use these to contruct the surface by drawing a loop in the plane for each point along the knot path.
    normals.assign(2,vector<ofVec3f> (totalKnotPoints+1,ofVec3f()));
    surfacePoints.assign(totalKnotPoints+1,vector<ofVec3f> (totalLoopPoints+1,ofVec3f()));
    surfaceNormals.assign(totalKnotPoints+1,vector<ofVec3f> (totalLoopPoints+1,ofVec3f()));
    
    
    //precompute the knot points and normals
    for(int i=0; i <= totalKnotPoints ;i++) {
        auto theta = ofMap(i,0,totalKnotPoints,0,TWO_PI);
         
        pathPoints[i]   = pathPoint(knotTorusRadiusRatio,theta);
        normals[0][i]     = normal1(knotTorusRadiusRatio,theta);
        normals[1][i]     = normal2(knotTorusRadiusRatio,theta);
    }
    
    material.setShininess(120);
    material.setSpecularColor(ofColor(255, 255, 255, 255));
}

void KnotSurface::update() {
    auto time = ofGetElapsedTimef();
    textureOrigin += textureDrift;
    //We are going to create our surface by taking a trefoil knot in space (a closed loop) and thickening it out into a tube.
    
    for(int i=0; i <= totalKnotPoints;i++) {
        //Theta parameterizes the knot in space.
        auto theta = ofMap(i,0,totalKnotPoints,0,TWO_PI);
        
        auto knotPoint = pathPoints[i];
        auto n1 = normals[0][i];
        auto n2 = normals[1][i];
        
        for(int j=0; j <= totalLoopPoints;j++) {
            //Phi parameteriszes the surface in the direction that loops around the  knot
            double phi = ofMap(j,0,totalLoopPoints,0,TWO_PI);
            
            double radius = tubeRadius*radialDeformation(theta,phi,time);
            
            //As we iterate over j(~phi) we draw a loop in the plane perpendicular to the knot. The sum of these loops is our surface.
            //the normals (n1,n2) provide a natural coordinate system on the plane.
            //This is the equation of a unit circle in the plane through the origin:
            auto surfaceVec = n1*sin(phi) + n2*cos(phi);
    
            surfacePoints[i][j] =  knotPoint + radius*surfaceVec;
            surfaceNormals[i][j] = surfaceVec.normalize();
        }
    }
}

void KnotSurface::draw() {
    material.begin();
        for(int i=0; i<totalKnotPoints;i++) {

            auto x1 = ofMap(i,0,totalKnotPoints,0,1.0) + textureOrigin.x;
            auto x2 = ofMap(i+1,0,totalKnotPoints,0,1.0) + textureOrigin.x;
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            
            for(int j=0; j <= totalLoopPoints;j++) {
                double y = ofMap(j,0,totalLoopPoints,0,1.0)+ textureOrigin.y;
               
                ofVec3f u =  surfacePoints[i][j] ;
                ofVec3f v =  surfacePoints[i+1][j] ;
                
                ofVec3f n_u = surfaceNormals[i][j];
                ofVec3f n_v = surfaceNormals[i+1][j];
                
                ofVec2f tex_u = ofVec2f(x1, y);
                ofVec2f tex_v = ofVec2f(x2, y);
                
                mesh.addVertex(u);
                mesh.addNormal(n_u);
                mesh.addTexCoord(tex_u);
                
                mesh.addVertex(v);
                mesh.addNormal(n_v);
                mesh.addTexCoord(tex_v);
            }
            
            if(texture) texture->bind();
            mesh.draw();
            if(texture) texture->unbind();
        }
    material.end();
}

/*
    This radial function allows us to distort the surface and have it evolve over time.
    I'm just adding in some perlin noise and sending a periodic pulse around the knot.
*/
double KnotSurface::radialDeformation(double theta,double phi,double t) {
    auto r1 = 0.5;
    auto r2 = 0.5;
   
    //We can make the noise flow around the torus by making the angles time dependent;
    theta += t;
    phi += t;
    t *=  noiseRate;
   
    auto bulge = 0.4*pulse(theta);
    return 1.0 + 0.6*ofNoise(r1*cos(theta+t)+t,r1*sin(theta+t)+t ,r2*cos(phi+t)+t,r2*sin(phi+t)+t) + bulge;
}



double  KnotSurface::pulse(double theta) {
    return 0.5*(1 + cos(theta));
}





ofVec3f KnotSurface::pathPoint(double r, double t) {
    ofVec3f v = trefoilKnotPoint(r, t );
    return pathScale*v;
}

/*
    Maths for constructing the knot surface.
     
    Parametric equation for a trefoil knot in space.
    A trefoil knot is the simplest torus knot -  a closed path that winds around the surface of a torus.
    here t is the paramter for the path, in the range 0 - 2*Pi.
    r is the smaller radius of the torus around which the path winds (with the larger radius fixed at R = 1.0)
 */

ofVec3f KnotSurface::trefoilKnotPoint(double r, double t){
    ofVec3f v;
    
    v.x = cos(2*t)*(1 + r*cos(3*t));
    
    v.y = (1 + r*cos(3*t))*sin(2*t);
    
    v.z = r*sin(3*t);
    
    return v;
}


/*
     Normals.
     
     We want to draw a tube that follows the path of our knot. 
     So for each point of the knot we want to draw a circle ( or any other loop) around the knot.
     We should draw this circle in a plane perpendicular to the path of the knot at that point.
     The following functions give an orthoganal pair of normals to the plane at a given knot point.
*/
ofVec3f KnotSurface::normal1(double r, double t) {
    ofVec3f v;
    
    v.x =   -32*cos(2*t) - r*((60 + 51*r*r)*cos(t) + 108*cos(5*t) +
            r*(228*cos(2*t) + 18*cos(4*t) + 279*r*cos(5*t) - 2*r*cos(7*t) + 66*cos(8*t) +
            10*r*cos(11*t)));
    
    v.y =   3*r*(20 + 17*r*r)*sin(t) -
            4*(8 + 57*r*r)*sin(2*t) -
            r*(-18*r*sin(4*t) + 9*(12 + 31*r*r)*sin(5*t) +
            2*r*(r*sin(7*t) + 33*sin(8*t) + 5*r*sin(11*t)));
    
    v.z =   -18*r*(4 + 9*r*r + 4*r*cos(3*t))*sin(3*t);
    
    return v.normalize();
}


ofVec3f KnotSurface::normal2(double r, double t) {
    ofVec3f v;
    
    v.x =   3*r*(-8*(5 + 24*r*r)*sin(t) - 8*sin(5*t) +
            r*(6*(20 + 41*r*r)*sin(2*t) + 12*(-3*sin(4*t) + sin(8*t)) +
            r*(96*sin(5*t) - 6*sin(7*t) + 18*sin(11*t) +
            r*(-11*sin(4*t) + 77*sin(8*t) + sin(10*t) + 5*sin(14*t)))));
    
    
    v.y =   -3*r*(8*(5 + 24*r*r)*cos(t) - 8*cos(5*t) +
            r*(6*(20 + 41*r*r)*cos(2*t) + 12*(3*cos(4*t) + cos(8*t)) +
            r*(6*(16*cos(5*t) + cos(7*t) + 3*cos(11*t)) +
            r*(11*cos(4*t) + 77*cos(8*t) - cos(10*t) + 5*cos(14*t)))));
    
    
    v.z =   2*(32 + 348*r*r + 336*pow(r,4) + 4*r*(50 + 159*r*r)*cos(3*t) +
            r*r*(132 + 7*r*r)*cos(6*t) + 14*pow(r,3)*cos(9*t) -
            5*pow(r,4)*cos(12*t));
    
    return v.normalize();
    
}