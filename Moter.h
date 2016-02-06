
#ifndef MOTER_H
#define MOTER_H

#include "Type.h"

void MoterInit(void);
void MoterSpeed(u8 cmd, u8 speed);
u16 MoterReadResistancePosition(void);
void MoterSetCodingSite(u8 cmd);
u16 MoterReadCodingSite(void);

void MoterSetSleep(u8 cmd);
u16 MoterGetSleep(void);

u16 MoterReadCurrent(void);
void MoterSleep(void);
void MoterOpen(void);

#endif

