#pragma once
#include "ofMain.h"

#include "KnotSurface.hpp"
#include "FluidSym.hpp"
#include "InkSource.hpp"
#define DEBUG_FLUID true
class ofApp : public ofBaseApp{

	public:
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
    
       
        FluidSym *fluid;
        KnotSurface *knotSurface;
        vector<InkSource> inkSources;
    
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
    
        ofLight light;
        ofLight light2;
};
