#ifndef TELLO
#define TELLO

#include <stdio.h>
#include <WinSock2.h>
#include <string.h>

int TELLO_Init();
int TELLO_Open();
void TELLO_Close();
void TELLO_DeInit();
void TELLO_Update();
struct TelloState TELLO_GetState();
void TELLO_SendCommand(char * command);
void TELLO_Rc(int roll, int pitch, int throttle, int yaw);
void TELLO_Takeoff();
void TELLO_Land();
void TELLO_StreamOn();
void TELLO_StreamOff();

struct TelloState {
	int pitch;
	int roll;
	int yaw;
	int vgx;
	int vgy;
	int vgz;
	int templ;
	int temph;
	int tof;
	int h;
	int bat;
	float baro;
	int time;
	float agx;
	float agy;
	float agz;
};

#endif