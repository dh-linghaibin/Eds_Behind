

#ifndef TYPE_H
#define TYPE_H

/*Data Types Reconstruction*/
typedef unsigned char    u8;
typedef unsigned int     u16;
typedef unsigned long    u32;
typedef signed char    s8;
typedef signed int     s16;
typedef signed long    s32;

#define BIT(x) (1 << (x))

#define INTEN   asm("rim");//open interrupt
#define INTOFF  asm("sim");//closs interrupt
#define MCUSLEEP asm("halt");//sleep

u8 TypeDecomposeU16(u16 data, u8 cmd);
u16 TypeCombinationU16(u8 data_h, u8 data_l);

//include file
#include "iostm8s103f3.h"


#endif

