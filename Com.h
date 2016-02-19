

#ifndef COM_H
#define COM_H

#include "Type.h"

void ComInit(void);
u8 ComGetFlag(void);
void ComClearFlag(void);
u8 ComGetData(u8 num);
void ComClearData(void);
void ComSendCmd(u8 cmd,u8 par1,u8 par2,u8 par3);

#define add_stal 0xe1//加档
#define sub_stal 0xe2//减档
#define exchange_stal 0xe3//减档
#define ask_rear 0xa1//告诉前拨档位
#define add_setp 0x03//步进加
#define sub_setp 0x04//步进减
#define dce_gear 0x05//查询当前档位
#define dce_powe 0x06//查询电量
#define set_inti 0x07//设置初始位
#define set_end 0x08//设置结束位
#define reset 0x09//复位
#define stuck 0x0a//卡死


#endif
