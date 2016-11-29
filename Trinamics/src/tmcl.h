
#ifndef tmcl_h
#define tmcl_h

#include "StdAfx.h"

#include <math.h>



class tmcl
{
public:


	        // Constructor
        tmcl();

        // Methods
		HANDLE OpenRS232(const char * ComName, DWORD BaudRate);
		void CloseRS232(HANDLE Handle);
		void SendCmd(HANDLE Handle, UCHAR Address, UCHAR Command, UCHAR Type, UCHAR Motor, INT Value);

		UCHAR GetResult(HANDLE Handle, UCHAR *Address, UCHAR *Status, int *Value);
		int GetReply(HANDLE Handle);
		void SetSettings(HANDLE Handle,int motor);
		void SetVelocity(HANDLE Handle,int motor, int velocity);
		void SetAcceleration(HANDLE Handle,int motor, int acceleration);
		int GetStepsPerFrame(int vel, int frameRate);
		int GetStepsPerSecond(int vel);
		void moveTillSecond(HANDLE Handle,int motor, int seconds, int velocity);
		void moveTillFrame(HANDLE Handle, int motor, int frame, int velocity, int frameRate);
		void moveToAbs(HANDLE Handle,int motor, int velocity, int steps);
		void moveToRel(HANDLE Handle,int motor, int velocity, int steps);
		void moveRight(HANDLE Handle,int motor, int velocity);
		void moveLeft(HANDLE Handle,int motor, int velocity);
		int GetVelocity(HANDLE Handle,int motor);
		void restoreAcceleration(HANDLE Handle, int motor);
		void stop(HANDLE Handle,int motor);
		double SetTime(int steps, int velocity);
		int GetPosition(HANDLE Handle,int motor);
        void SetResolution (HANDLE Handle, int motor, int resolution); // De 0 a 6, siendo 6  resolucion maxima
        void SetPower (HANDLE Handle,int motor, int power); // IMPORTANTE -- valores de 0 a 255 siendo 255 el 100% pero puede causar daños en el motor.

        void Move(HANDLE handle,int motor, int direction, int velocity);





};

#endif
