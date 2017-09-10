#pragma once
#include "ofMain.h"

#include "KnotSurface.hpp"
#include "FluidSym.hpp"
#include "InkSource.hpp"
#define DEBUG_FLUID false
#define REAL_TIME false //octaves = 5/4!?
class ofApp : public ofBaseApp{

	public:
    
    bool updateFluid = true;
    bool updateKnot = true;
    bool updateParticles = true;
    
    
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        //artwork
        FluidSym *fluid;
        KnotSurface *knotSurface;
        vector<InkSource> inkSources;
    
        //camera
        ofCamera cam; // add mouse controls for camera movement
        double camDistance;
        double camTheta;
        double camPhi;
        const double camTheta0 = 5.6;
        const double camPhi0 = 3.5;
        ofVec3f camCentre;
        double camDeltaAngle;
        double camDeltaSpace;
    
        int lastMouseX;
        int lastMouseY;
    
        //lights
        ofLight light;
        ofLight light2;
        ofLight light3;
    
        ofVec3f lightMainPosition;
        ofVec3f lightDisplacement;
        void fadeLights(float s);
    
        float initialBumpAmount = 0.3;
        bool doSaveScreen = false;
        
        float inkStartTime = 7;
        
        ofVec3f v;
    
        bool recording = false;
    
        /*
        //record
        ofVideoGrabber      vidGrabber;
        ofxVideoRecorder    vidRecorder;
        bool bRecording;
        void startRecord();
        void stopRecord();
         */
    
    
    
    
    
};
