#pragma once
#include "tmcl.h"
#include "ofMain.h"

#include "ofxGui.h"

#define RIGHT 1
#define LEFT 2


class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();
        void exit();


        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        void stopPressed();
		void modePressed();
		void salirPressed();

        tmcl* trinamic;
        tmcl tm;
        HANDLE RS232Handle;


        ofTrueTypeFont font;
        ofParameterGroup parameters1, parameters2;

		ofxIntSlider velocidadA;
		ofxIntSlider velocidadB;
		ofxIntSlider potenciaA;
		ofxIntSlider potenciaB;
		ofxIntSlider aceleracionA;
		ofxIntSlider aceleracionB;
		ofxIntSlider posIniA;
		ofxIntSlider posFinA;
		ofxFloatSlider periodo;
		float refresco;
        ofxButton stop;
		ofxButton mode;
		ofxButton salir;
		ofxPanel gui;



};
