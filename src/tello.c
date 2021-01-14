#include "tello.h"

#define DEFAULT_BUFFLEN 1024
#define TELLO_IP "192.168.10.1"
#define TELLO_PORT 8889
#define LOCAL_PORT 8890

WSADATA wsa;
SOCKET commandSocket = INVALID_SOCKET;
struct sockaddr_in commandToAddress;
int commandToAddressLength = 0;
struct sockaddr_in commandLocalAddress;
int commandLocalAddressLength = 0;
int initialized = 0;
struct TelloState currentState;


void setSocketToBlockingMode(SOCKET s){
	ioctlsocket(s, FIONBIO, 0);
}

void setSocketToNonBlockingMode(SOCKET s){
	u_long val = 1;
	ioctlsocket(s, FIONBIO, &val);
}

int TELLO_Init(){
	if (initialized) return 0;

	// Init winsock
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0){
		printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
		return 1;
	}

	// Assign addresses
	commandLocalAddress.sin_family = AF_INET;
	commandLocalAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
	commandLocalAddress.sin_port = htons(LOCAL_PORT);
	commandLocalAddressLength = sizeof(commandLocalAddress);

	commandToAddress.sin_family = AF_INET;
	commandToAddress.sin_port = htons(TELLO_PORT);
	commandToAddress.sin_addr.s_addr = inet_addr(TELLO_IP);
	commandToAddressLength = sizeof(commandToAddress);

	initialized = 1;
	return 0;
}

int TELLO_Open(){
	if (initialized != 1) return 1;
	const char *recvbuf[DEFAULT_BUFFLEN];
	commandSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (commandSocket == SOCKET_ERROR){
		printf("socket() Failed with error code: %d\n", WSAGetLastError());
		return 1;
	}
	if (bind(commandSocket, (SOCKADDR*) &commandLocalAddress, commandLocalAddressLength) == SOCKET_ERROR){
		printf("bind() Failed with error code: %d\n", WSAGetLastError());
		return 1;
	}

	setSocketToBlockingMode(commandSocket);
	const char* message = "command";
	int ret = sendto(commandSocket, message, strlen(message), 0, (SOCKADDR*) &commandToAddress, commandToAddressLength);
	if (ret == SOCKET_ERROR){
		printf("Failed to send message to TELLO: %d\n", WSAGetLastError());
	}
	return 0;
}

void TELLO_Close(){
	if (initialized != 1) return;
	closesocket(commandSocket);
}

void TELLO_DeInit(){
	if (initialized != 1) return;
	WSACleanup();
}


int hasSocketData(SOCKET s){
	setSocketToNonBlockingMode(s);
	char recvbuf[DEFAULT_BUFFLEN];
	int numBytes = recv(s, recvbuf, DEFAULT_BUFFLEN, MSG_PEEK);
	if (numBytes == SOCKET_ERROR) {
		return 0;
	}
	return 1;
}

void processStateMessage(char* buffer){
	char * outerPtr;
	char * innerPtr;
	char * p = strtok_r(buffer, ";", &outerPtr);
	while(p != NULL){
		char *key = strtok_r(p, ":", &innerPtr);
		char *value = strtok_r(NULL, ":", &innerPtr);

		if (strcmp(key, "pitch") == 0){
			currentState.pitch = atoi(value);
		}else if(strcmp(key, "roll") == 0){
			currentState.roll = atoi(value);
		}else if(strcmp(key, "yaw") == 0){
			currentState.yaw = atoi(value);
		}else if (strcmp(key, "vgx") == 0){
			currentState.vgx = atoi(value);
		}else if (strcmp(key, "vgy") == 0){
			currentState.vgy = atoi(value);
		}else if (strcmp(key, "vgz") == 0){
			currentState.vgz = atoi(value);
		}else if (strcmp(key, "templ") == 0){
			currentState.templ = atoi(value);
		}else if (strcmp(key, "temph") == 0){
			currentState.temph = atoi(value);
		}else if (strcmp(key, "tof") == 0){
			currentState.tof = atoi(value);
		}else if (strcmp(key, "h") == 0){
			currentState.h = atoi(value);
		}else if (strcmp(key, "bat") == 0){
			currentState.bat = atoi(value);
		}else if(strcmp(key, "baro") == 0){
			currentState.baro = atof(value);
		}else if(strcmp(key, "time") == 0){
			currentState.time = atoi(value);
		}else if(strcmp(key, "agx") == 0){
			currentState.agx = atof(value);
		}else if(strcmp(key, "agy") == 0){
			currentState.agy = atof(value);
		}else if(strcmp(key, "agz") == 0){
			currentState.agz = atof(value);
		}

		p = strtok_r(NULL, ";", &outerPtr);
	}
	return;
}

void TELLO_Update() {
	int hasData = hasSocketData(commandSocket);

	while (hasData){
		setSocketToBlockingMode(commandSocket);
		char recvbuf[DEFAULT_BUFFLEN];
		memset(recvbuf, 0, DEFAULT_BUFFLEN);
		int numBytes = recv(commandSocket, recvbuf, DEFAULT_BUFFLEN, 0);
		if (numBytes == SOCKET_ERROR){
			if (WSAGetLastError() != WSAEWOULDBLOCK){
				printf("Socket read error: %d\n", WSAGetLastError());
			}
		}

		hasData = hasSocketData(commandSocket);
		if (!hasData){
			processStateMessage(recvbuf);
		}
	}
}

struct TelloState TELLO_GetState() {
	return currentState;
}

void TELLO_SendCommand(char * command){
	setSocketToBlockingMode(commandSocket);
	int ret = sendto(commandSocket, command, strlen(command), 0, (SOCKADDR*) &commandToAddress, commandToAddressLength);
	if (ret == SOCKET_ERROR){
		printf("Failed to send command: %s, %d\n", command, WSAGetLastError());
	}
}

void TELLO_Rc(int roll, int pitch, int throttle, int yaw) {
	char *command;
	sprintf(command, "rc %d %d %d %d", roll, pitch, throttle, yaw);
	TELLO_SendCommand(command);
}

void TELLO_Takeoff(){
	TELLO_SendCommand("takeoff");
}
void TELLO_Land(){
	TELLO_SendCommand("land");
}