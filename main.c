

#include "Sys.h"
#include "Com.h"
#include "Led.h"
#include "Moter.h"
#include "Timer.h"
#include <math.h>
#include "Delay.h"
#include "Control.h"
#include "Eeprom.h"

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
        if(TimerGetTimeFlag() > 10) {
            TimerClearTimeFlag();
            MoterSleep();
            //ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
        }
        if(ComGetFlag() == 0x80) {
            ComClearFlag();
            TimerClearTimeFlag();
            MoterOpen();
            switch(ComGetData(0)) {
                case exchange_stal:
                    if(ControlGetStall() == 1) {
                        ControlSetStall(0);
                        ControlRunPosition(ControlCalculateGrating(1));
                        DelayMs(2000);
                        ControlRunPosition(ControlCalculateGrating(3));
                    } else {
                        ControlSetStall(1);
                        ControlRunPosition(ControlCalculateGrating(0));
                        DelayMs(2000);
                        ControlRunPosition(ControlCalculateGrating(2));
                    }
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case add_setp:
                    ControlRunPosition(5);
                break;
                case sub_setp:
                    ControlRunPosition(-5);
                break;
                case dce_gear:
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case set_inti:
                    ControlSetStart();
                break;
                default:break;
            }
            MoterSleep();
        }
    }
}

