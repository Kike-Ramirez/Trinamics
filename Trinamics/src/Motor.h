#ifndef MOTOR_H
#define MOTOR_H

#include "ofParameterGroup.h"
#include "ofParameter.h"
#include "ofMain.h"

class Motor
{
public:
    Motor();

    void setup(string name);
    void update();
    void draw();

    ofParameterGroup parameters;
    ofParameter<float> size;
    ofParameter<int> number;
    ofParameter<ofVec2f> position;

    ofParameter<ofColor> color;


};

#endif // MOTOR_H

