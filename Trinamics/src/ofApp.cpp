#include "ofApp.h"
#define motorUno 1
#define motorDos 2

float posA;
float posB;
unsigned long long tiempo; 
unsigned long long tiempo2;
unsigned long long timer;
unsigned long long timer2;

bool cambio;
bool loop;

int TARGET = -35000;

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);

	ofNoFill();
	ofSetColor(255);

	RS232Handle = trinamic->OpenRS232("COM4", 9600);

	stop.addListener(this, &ofApp::stopPressed);
	mode.addListener(this, &ofApp::modePressed);
	salir.addListener(this, &ofApp::salirPressed);

	gui.setup(); // most of the time you don't need a name
	gui.add(velocidadA.setup("velocidad A", 150, 1, 2000));
	gui.add(potenciaA.setup("potencia A", 200, 1, 400));
	gui.add(aceleracionA.setup("aceleracion A", 5, 1, 100));
	gui.add(periodo.setup("Periodo A", 1, 0, 6));
	gui.add(posIniA.setup("Pos. Ini. A", 3 * 360 * 32, 0, 10 * 360 * 32));
	gui.add(posFinA.setup("Pos. Fin A", -3 * 360 * 32, -10 * 360 * 32, 0));

	gui.add(velocidadB.setup("velocidad B", 150, 1, 2000));
	gui.add(potenciaB.setup("potencia B", 200, 1, 400));
	gui.add(aceleracionB.setup("aceleracion B", 5, 1, 100));	
	gui.add(stop.setup("Stop"));
	gui.add(mode.setup("Loop"));
	gui.add(salir.setup("Exit"));

	refresco = 30;

	gui.loadFromFile("settings.xml");

	trinamic->SetAcceleration(RS232Handle, motorUno, aceleracionA);
	trinamic->SetPower(RS232Handle, motorUno, potenciaA);
	trinamic->SetVelocity(RS232Handle, motorUno, velocidadA);
	trinamic->SetResolution(RS232Handle, motorUno, 6);
	trinamic->SetSettings(RS232Handle, motorUno);

	trinamic->SetAcceleration(RS232Handle, motorDos, aceleracionB);
	trinamic->SetPower(RS232Handle, motorDos, potenciaB);
	trinamic->SetVelocity(RS232Handle, motorDos, velocidadB);
	trinamic->SetResolution(RS232Handle, motorDos, 6);
	trinamic->SetSettings(RS232Handle, motorDos);

	posA = 10;
	posB = 0;
	tiempo = ofGetElapsedTimeMillis();
	tiempo2 = ofGetElapsedTimeMillis(); 
	cambio = false;
	loop = false;

}

//--------------------------------------------------------------
void ofApp::update() {

	timer = periodo * 1000;
	timer2 = refresco;

	if (ofGetElapsedTimeMillis() - tiempo2 > timer2) {
		tiempo2 = ofGetElapsedTimeMillis();
		posA = trinamic->GetPosition(RS232Handle, motorUno) / 32;
		posB = trinamic->GetPosition(RS232Handle, motorDos) / 32;
 

	}
	
	if (loop) {

		
		if (ofGetElapsedTimeMillis() - tiempo > timer) {
			tiempo = ofGetElapsedTimeMillis();

			if (cambio) {

				trinamic->moveToAbs(RS232Handle, motorUno, velocidadA, 0);
				cambio = false;
			}

			else {
				trinamic->moveToAbs(RS232Handle, motorUno, velocidadA, TARGET);
				cambio = true;
			}
		}
	}
	




}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(0, 0, 0);

	ofSetColor(255);
	ofPushMatrix();
	ofTranslate(ofGetWidth() / 3, ofGetHeight() / 2);
	
	string info = "";
	info += "MOTOR A: Letra H " + ofToString(posA);
	ofDrawBitmapString(info, -150, 170);

	ofEllipse(0, 0, 300, 300);
	ofEllipse(0, 0, 5, 5);
	ofRotate(posA+90);
	ofLine(0, 0, 150, 0);
	ofTranslate(150, 0);
	ofSetColor(255, 0, 0);
	ofEllipse(0, 0, 40, 40);
	ofPopMatrix();

	ofSetColor(255);
	ofPushMatrix();
	ofTranslate(ofGetWidth() * 2 / 3, ofGetHeight() / 2);
	info = "";
	info += "MOTOR B: Letra Y" + ofToString(posB);
	ofDrawBitmapString(info, -150, 170);
	ofEllipse(0, 0, 300, 300);
	ofEllipse(0, 0, 5, 5);
	ofRotate(posB + 90);
	ofLine(0, 0, 150, 0);
	ofTranslate(150, 0);
	ofSetColor(0, 0, 255);
	ofEllipse(0, 0, 40, 40);
	ofPopMatrix();

	gui.draw();


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key)
	{
	case ' ':
		if (loop)
			stopPressed();
		else
			modePressed();
		break;
	case '1':
		trinamic->moveToAbs(RS232Handle, motorUno, velocidadA, TARGET);
		break;
	case '0':
		trinamic->moveToAbs(RS232Handle, motorUno, velocidadA, 0);
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
void ofApp::exit() {

	mode.removeListener(this, &ofApp::modePressed);
	stop.removeListener(this, &ofApp::stopPressed);
	trinamic->stop(RS232Handle, motorDos);
	trinamic->stop(RS232Handle, motorUno);
	trinamic->CloseRS232(RS232Handle);

}



void ofApp::stopPressed() {

	loop = false;
	trinamic->stop(RS232Handle, motorDos);
	trinamic->stop(RS232Handle, motorUno);

}

void ofApp::modePressed() {

	if (!loop) {
		tiempo = ofGetElapsedTimeMillis();
		loop = true;
		trinamic->moveRight(RS232Handle, motorDos, velocidadB);
		trinamic->SetPower(RS232Handle, motorDos, potenciaB);
		trinamic->SetAcceleration(RS232Handle, motorDos, aceleracionB);

		trinamic->SetPower(RS232Handle, motorUno, potenciaA);
		trinamic->SetAcceleration(RS232Handle, motorUno, aceleracionA);

	}
	

}

void ofApp::salirPressed() {

	ofExit();

}