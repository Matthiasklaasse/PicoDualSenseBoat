#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <stdbool.h>

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

extern volatile bool ControllerConnected;

extern volatile int SoftRumble;
extern volatile int Rumble;

extern volatile bool RumbleIsInSync;
extern volatile bool BatteryLedIsInSync;

extern volatile int LeftLedValue;
extern volatile int RightLedValue;

extern volatile int RightLedRed;
extern volatile int RightLedGreen;
extern volatile int RightLedBlue;

extern volatile int LeftLedRed;
extern volatile int LeftLedGreen;
extern volatile int LeftLedBlue;

extern volatile int Battery;
extern volatile int CurrentBattery;
extern volatile int BoatBattery;
extern volatile int ControllerBattery;

extern volatile int R;
extern volatile int G;
extern volatile int B;
extern volatile int _R;
extern volatile int _G;
extern volatile int _B;
#endif
