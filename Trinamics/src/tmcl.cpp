#include "tmcl.h"



//Opcodes of all TMCL commands that can be used in direct mode
const int TMCL_ROR = 1;
const int TMCL_ROL = 2;
const int TMCL_MST = 3;
const int TMCL_MVP = 4;
const int TMCL_SAP = 5;
const int TMCL_GAP = 6;
const int TMCL_STAP = 7;
const int TMCL_RSAP = 8;
const int TMCL_SGP = 9;
const int TMCL_GGP = 10;
const int TMCL_STGP = 11;
const int TMCL_RSGP = 12;
const int TMCL_RFS = 13;
const int TMCL_SIO = 14;
const int TMCL_GIO = 15;
const int TMCL_SCO = 30;
const int TMCL_GCO = 31;
const int TMCL_CCO = 32;

//Opcodes of TMCL control functions (to be used to run or abort a TMCL program in the module)
const int TMCL_APPL_STOP = 128;
const int TMCL_APPL_RUN = 129;
const int TMCL_APPL_RESET = 131;

//Options for MVP commandds
const int MVP_ABS = 0;
const int MVP_REL = 1;
const int MVP_COORD = 0;

//Options for RFS command
const int RFS_START = 0;
const int RFS_STOP = 1;
const int RFS_STATUS = 2;

//const int FALSE = 0;
//const int TRUE = 1;

//Result codes for GetResult
const int TMCL_RESULT_OK = 0;
const int TMCL_RESULT_NOT_READY = 1;
const int TMCL_RESULT_CHECKSUM_ERROR = 2;

int st, vel;
float time;

tmcl::tmcl(void)
{
}

//Open serial interface
//Usage: ComHandle=OpenRS232("COM1", CBR_9600)

HANDLE tmcl::OpenRS232(const char * ComName, DWORD BaudRate)
{
	HANDLE ComHandle;
	DCB CommDCB;
	COMMTIMEOUTS CommTimeouts;

    ComHandle=CreateFileA(ComName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(GetLastError()!=ERROR_SUCCESS) return INVALID_HANDLE_VALUE;
	else
	{
		GetCommState(ComHandle, &CommDCB);

		CommDCB.BaudRate=BaudRate;
		CommDCB.Parity=NOPARITY;
		CommDCB.StopBits=ONESTOPBIT;
		CommDCB.ByteSize=8;

		CommDCB.fBinary=1;  //Binary Mode only
		CommDCB.fParity=0;
		CommDCB.fOutxCtsFlow=0;
		CommDCB.fOutxDsrFlow=0;
		CommDCB.fDtrControl=0;
		CommDCB.fDsrSensitivity=0;
		CommDCB.fTXContinueOnXoff=0;
		CommDCB.fOutX=0;
		CommDCB.fInX=0;
		CommDCB.fErrorChar=0;
		CommDCB.fNull=0;
		CommDCB.fRtsControl=RTS_CONTROL_TOGGLE;
		CommDCB.fAbortOnError=0;

		SetCommState(ComHandle, &CommDCB);

		//Set buffer size
		SetupComm(ComHandle, 100, 100);

		//Set up timeout values (very important, as otherwise the program will be very slow)
		GetCommTimeouts(ComHandle, &CommTimeouts);

		CommTimeouts.ReadIntervalTimeout=MAXDWORD;
		CommTimeouts.ReadTotalTimeoutMultiplier=0;
		CommTimeouts.ReadTotalTimeoutConstant=0;

		SetCommTimeouts(ComHandle, &CommTimeouts);

		return ComHandle;
  }
}

void tmcl::CloseRS232(HANDLE Handle)
{
	CloseHandle(Handle);
}

void tmcl::SendCmd(HANDLE Handle, UCHAR Address, UCHAR Command, UCHAR Type, UCHAR Motor, INT Value)
{
UCHAR TxBuffer[9];
DWORD BytesWritten;
int i;

TxBuffer[0]=Address;
TxBuffer[1]=Command;
TxBuffer[2]=Type;
TxBuffer[3]=Motor;
TxBuffer[4]=Value >> 24;
TxBuffer[5]=Value >> 16;
TxBuffer[6]=Value >> 8;
TxBuffer[7]=Value & 0xff;
TxBuffer[8]=0;
for(i=0; i<8; i++)
TxBuffer[8]+=TxBuffer[i];

//Send the datagram
WriteFile(Handle, TxBuffer, 9, &BytesWritten, NULL);

}


UCHAR tmcl::GetResult(HANDLE Handle, UCHAR *Address, UCHAR *Status, int *Value)
{
	UCHAR RxBuffer[9], Checksum;
	DWORD Errors, BytesRead;
	COMSTAT ComStat;
	int i;

	//Check if enough bytes can be read
	ClearCommError(Handle, &Errors, &ComStat);
    if(ComStat.cbInQue>8)
	{
		//Receive
		ReadFile(Handle, RxBuffer, 9, &BytesRead, NULL);

		Checksum=0;

		for(i=0; i<8; i++)
			Checksum+=RxBuffer[i];

		if(Checksum!=RxBuffer[8]) return TMCL_RESULT_CHECKSUM_ERROR;

		*Address=RxBuffer[0];
		*Status=RxBuffer[2];
		*Value=(RxBuffer[4] << 24) | (RxBuffer[5] << 16) | (RxBuffer[6] << 8) | RxBuffer[7];
	} else return  TMCL_RESULT_NOT_READY;

	return TMCL_RESULT_OK;
}


int tmcl::GetReply(HANDLE Handle){
	int Timeout = GetTickCount();
	int	Value, Command;
	UCHAR Address, Status;
    while (GetResult(Handle, &Address, &Status, &Value) != TMCL_RESULT_OK && abs(int(GetTickCount() - Timeout)) < 100);
	return Value;
}



void tmcl :: SetResolution (HANDLE Handle, int motor, int resolution)
{
    SendCmd(Handle,motor,5,140,0,resolution);
    GetReply(Handle);
}


int tmcl::GetStepsPerSecond(int vel){
	int steps = (16000000/(2048*32*8))*vel;
	return steps;

}



void tmcl::moveTillSecond(HANDLE Handle,int motor, int seconds, int velocity){
	int steps = GetStepsPerSecond(velocity)*seconds;
	moveToRel(Handle,motor, velocity,steps);
}


int tmcl::GetStepsPerFrame(int vel, int frameRate){
	int steps = (16000000/(2048*32*8))*vel;
	return steps/frameRate;
}

void tmcl::moveTillFrame(HANDLE Handle,int motor, int frame, int velocity,int frameRate){
	int steps = GetStepsPerFrame(velocity,frameRate)*frame;
	moveToRel(Handle,motor, velocity,steps);
}


void tmcl::SetSettings(HANDLE Handle,int motor){
	SendCmd(Handle, motor, 6, 4, 0, 1000);
    GetReply(Handle);
	SendCmd(Handle, motor, 6, 5, 0, 1000);
    GetReply(Handle);
	SendCmd(Handle, motor, 6, 140, 0, 8);
    GetReply(Handle);
}

void tmcl::SetVelocity(HANDLE Handle,int motor, int velocity){
	SendCmd(Handle, motor, 5, 4, 0, velocity);
    GetReply(Handle);
}

void tmcl::SetAcceleration(HANDLE Handle,int motor, int acceleration){
    SendCmd(Handle, motor, 5, 5, 0, acceleration);
    GetReply(Handle);
}

void tmcl::moveToAbs(HANDLE Handle,int motor, int velocity, int steps){
	SetVelocity(Handle,motor, velocity);
	SendCmd(Handle, motor, 4, 0, 0, steps);
    GetReply(Handle);
}

void tmcl::moveToRel(HANDLE Handle,int motor, int velocity, int steps){
	SetVelocity(Handle,motor, velocity);
	SendCmd(Handle, motor, 4, 1, 0, steps);
    GetReply(Handle);

}

 void tmcl::SetPower (HANDLE Handle,int motor, int power)
 {
        SendCmd(Handle,motor,5,6,0,power);
        GetReply(Handle);
 }

void tmcl :: Move(HANDLE Handle,int motor, int direction, int velocity)
{
    SendCmd(Handle,motor,direction,0,0,velocity);
    GetReply(Handle);
}

void tmcl::moveLeft(HANDLE Handle,int motor, int velocity){
	SendCmd(Handle, motor, 1, 0, 0, velocity);
    GetReply(Handle);
}

void tmcl::moveRight(HANDLE Handle,int motor, int velocity){
	SendCmd(Handle, motor, 2, 0, 0, velocity);
    GetReply(Handle);
}

int tmcl::GetPosition(HANDLE Handle,int motor){
	SendCmd(Handle, motor, 6, 1, 0, 0);
	return GetReply(Handle);
}

void tmcl::restoreAcceleration(HANDLE Handle,int motor){
	SendCmd(Handle, motor, 8, 5, 0, 0);
    GetReply(Handle);
}

void tmcl::stop(HANDLE Handle,int motor){
	SendCmd(Handle, motor, 3, 0, 0, 0);
    GetReply(Handle);
}

double tmcl::SetTime(int steps, int velocity){
	int st = GetStepsPerSecond(velocity);
	double time = (1/st);
	return st;
}

int tmcl::GetVelocity(HANDLE Handle,int motor){
	SendCmd(Handle, motor, 6, 3, 0, 0);
	return GetReply(Handle);
}
