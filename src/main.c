#include <stdio.h>
#include <SDL2\SDL.h>
#include "tello.h"

#define JOYSTICK_MAX 32767.0

void quitApp();

int main(int argc, char *argv[]){
	int result = TELLO_Init();
	if (result != 0){
		quitApp();
		return result;
	}
	result = TELLO_Open();
	if (result != 0){
		quitApp();
		return result;
	}

	SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	printf("Startting joystick check\n");
	int joystickCount = SDL_NumJoysticks();
	printf("Joystick count: %d\n", joystickCount);

	if (joystickCount == 0) {
		quitApp();
		return 0;
	}

	if (!SDL_IsGameController(0)){
		printf("Not a game controller");
		quitApp();
		return 0;
	}
	
	SDL_GameController *joy = SDL_GameControllerOpen(0);
	if (!joy){
		quitApp();
		return 0;
	}


	printf("Joystick name: %s\n", SDL_GameControllerName(joy));

	printf("Is attached: %d\n", SDL_GameControllerGetAttached(joy));

	SDL_Window *window = SDL_CreateWindow("Hello world", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 400, SDL_WINDOW_SHOWN);
	

	int numAxis = SDL_JoystickNumAxes(SDL_GameControllerGetJoystick(joy));
	Sint8 shouldClose = 0;
	while (!shouldClose){
		SDL_UpdateWindowSurface(window);
		TELLO_Update();

		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if (event.type == SDL_WINDOWEVENT){
				if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
					shouldClose = 1;
				}
			}

			if (event.type == SDL_CONTROLLERBUTTONDOWN){
				if (event.cbutton.button == 0){
					TELLO_Takeoff();
				}else if (event.cbutton.button == 1){
					TELLO_Land();
				}
			}
		}

		int rawYaw = SDL_GameControllerGetAxis(joy, 0);
		int rawThrottle = SDL_GameControllerGetAxis(joy, 1);
		int rawRoll = SDL_GameControllerGetAxis(joy, 2);
		int rawPitch = SDL_GameControllerGetAxis(joy, 3);

		int yaw = (rawYaw / JOYSTICK_MAX) * 100.0;
		int throttle = -(rawThrottle / JOYSTICK_MAX) * 100.0;
		int roll = (rawRoll / JOYSTICK_MAX) * 100.0;
		int pitch = -(rawPitch / JOYSTICK_MAX) * 100.0;

		TELLO_Rc(roll, pitch, throttle, yaw);

		SDL_Delay(100);
	}

	SDL_GameControllerClose(joy);
	quitApp();
	return 0;
}

void quitApp(){
	SDL_Quit();
	TELLO_Close();
	TELLO_DeInit();
}