
#ifndef CONTROL_H
#define CONTROL_H

#include "Type.h"

void ControlInit(void);
int ControlCalculateGrating(u8 stalss,u8 cmd);
u8 ControlRunPosition(int num);

u8 ControlSetStallsSub(void);
u8 ControlSetStallsAdd(void);
u8 ControlGetStall(void);
void ControlSetStall(u8 cmd);
void ControlSetStart(void);
void ControlSetRear(u8 cmd);
u8 ControlGetStart(void);

#endif

