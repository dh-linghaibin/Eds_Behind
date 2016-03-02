

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
        }
        if(ComGetFlag() == 0x80) {
            u16 delay_time = 2000; 
            ComClearFlag();
            TimerClearTimeFlag();
            MoterOpen();
            switch(ComGetData(0)) {
                case exchange_stal:
                    /*
                    if(ControlGetStall() == 1) {//下链条
                        ControlSetStall(0);
                        if( ControlRunPosition(ControlCalculateGrating(1,0)) == 0x44 ){
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        }
                        if(ControlGetStart() > 7) {
                            delay_time = 1000;
                        } else {
                            delay_time = 2000;
                        }
                        DelayMs(delay_time);
                        if( ControlRunPosition(ControlCalculateGrating(3,1)) == 0x44 ){
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        }
                    } else {//上链
                        ControlSetStall(1);
                        if( ControlRunPosition(ControlCalculateGrating(0,0)) == 0x44 ){
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        }
                        DelayMs(2000);
                        if( ControlRunPosition(ControlCalculateGrating(2,2)) == 0x44 ){
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        }
                    }*/
                    
                    if(ControlGetStall() == 1) {//下链条
                        ControlSetStall(0);
                        if( ControlRunPosition(ControlCalculateGrating2(1,3)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(2,2));
                            ControlSetStall(1);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        }
                        DelayMs(300);
                        if( ControlRunPosition(ControlCalculateGrating2(1,4)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(2,2));
                            ControlSetStall(1);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        }
                        if(ControlGetStart() > 7) {
                            delay_time = 600;
                        } else {
                            delay_time = 1000;
                        }
                        DelayMs(delay_time);
                        if( ControlRunPosition(ControlCalculateGrating2(3,5)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(2,2));
                            ControlSetStall(1);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        }
                    } else {//上链
                        ControlSetStall(1);
                        if( ControlRunPosition(ControlCalculateGrating2(0,0)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(3,5));
                            ControlSetStall(0);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        }
                        DelayMs(1000);
                        if( ControlRunPosition(ControlCalculateGrating2(0,1)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(3,5));
                            ControlSetStall(0);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        }
                        DelayMs(1200);
                        if( ControlRunPosition(ControlCalculateGrating2(2,2)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(3,5));
                            ControlSetStall(0);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        }
                    }
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case add_setp:
                    if( ControlRunPosition(30) == 0x44 ){
                        ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        break;
                    }
                break;
                case sub_setp:
                    if( ControlRunPosition(-20) == 0x44 ){
                        ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        break;
                    }
                break;
                case dce_gear:
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case set_inti:
                    ControlSetStart();
                break;
                case ask_rear:
                    ControlSetRear(ComGetData(1));
                    if(ControlGetStall() == 1) {//上链
                        if( (ControlGetRear() == 0x06) || (ControlGetRear() == 0x05) ) {
                            if( ControlRunPosition(ControlCalculateGrating(2,2)) == 0x44 ){
                                ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                                break;
                            }
                        }
                    } else {
                        if( (ControlGetRear() == 0x03) || (ControlGetRear() == 0x04) || (ControlGetRear() == 0x05) || 
                           (ControlGetRear() == 0x06) ||(ControlGetRear() == 0x07) || (ControlGetRear() == 0x08) ) {
                            if( ControlRunPosition(ControlCalculateGrating(3,1)) == 0x44 ){
                                ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                                break;
                            }
                        }
                    }
                break;
                default:break;
            }
            ComClearData();
            //MoterSleep();
        }
    }
}

