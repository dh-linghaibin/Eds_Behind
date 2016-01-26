

#include "Sys.h"
#include "Com.h"
#include "Led.h"
#include "Moter.h"
#include "Timer.h"
#include <math.h>
#include "Delay.h"
#include "Control.h"
#include "Eeprom.h"


u8 i = 0;
u8 dr = 0;
u16 weizhi = 0;

int main( void ) {
    SysInit();
    EeepromInit();
    ControlInit();
    ComInit();
    LedInit();
    MoterInit();
    TimerInit();
    INTEN
    while(1) {
        //weizhi = MoterReadResistancePosition();//get position
        if(TimerGetTimeFlag() > 5) {
            TimerClearTimeFlag();
            if(i == 0) {
                i = 1;
            } else {
                i = 0;
            }
            ControlRunPosition(ControlCalculateGrating(i));
            
            if(i == 0) {DelayMs(2000);
                //ControlRunPosition(ControlCalculateGrating(2));
            } else {DelayMs(2000);
                //ControlRunPosition(ControlCalculateGrating(3));
            }
            TimerClearTimeFlag();
        }
        if(ComGetFlag() == 0x80) {
            ComClearFlag();
            switch(ComGetData(0)) {
                case add_stal:
                break;
                case sub_stal:
                break;
                case add_setp:
                break;
                case sub_setp:
                break;
                case dce_gear:
                break;
                case set_inti:
                break;
                default:break;
            }
        }
    }
}

