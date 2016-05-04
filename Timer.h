

#ifndef TIMER_H
#define TIMER_H

#include "Type.h"

void TimerInit(void);
u16 TimerGetTimeFlag(void);
void TimerClearTimeFlag(void);
u16 TimerGetSpeed(void);

#endif



