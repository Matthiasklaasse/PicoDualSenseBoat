#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <stdbool.h>
#include <uni.h>

extern volatile bool XIsDown;
extern volatile bool OIsDown;
extern volatile bool TriangleIsDown;
extern volatile bool SquareIsDown;

extern volatile int LeftTrigger;
extern volatile int RightTrigger;

extern volatile int RightStickX;
extern volatile int RightStickY;
extern volatile int LeftStickX;
extern volatile int LeftStickY;

extern volatile int ControllerBattery;
extern volatile bool ControllerConnected;

#endif
