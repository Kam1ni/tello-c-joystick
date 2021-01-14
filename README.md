# TELLO-C-JOYSTICK

**WARNING** THIS IS STILL EARLY WIP JUST LIKE THIS README.

App made in C to control your tello drone with an XInput game controller. Tested using my Trust GXT 545, other controllers should work as well. I also have a TypeScript version [here](https://github.com/Kam1ni/tello-joystick/). This version is mainly used to improve my C skills and learn concepts like dependency management and stuff. I'm also planning to add the camera feed and show the drone state inside the window. I might even add a VR mode for it.

## Requirements

This app currently only works on Windows and is only tested with MinGW. Linux is planned for future releases. The only other dependency you have to have installed is SDL2. Use the gcc command to compile. Make sure to link mingw32, SDL2main, SDL2 and ws2_32.


## Controls

| description | binding |
| --- | --- |
| takeoff | A button|
| land | B button |
| ascend/descend | left analog stick y axis |
| yaw | left analog stick x axis |
| forward/backward | right analog stick y axis |
| strafe left/right | right analog stick x axis |
