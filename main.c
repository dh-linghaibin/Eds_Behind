#include "Sys.h"
#include "Com.h"
#include "Led.h"
#include "Moter.h"
#include "Timer.h"
#include <math.h>
#include "Delay.h"
#include "Control.h"
#include "Eeprom.h"


u16 xxx = 0;

int main( void ) {
    SysInit();
    EeepromInit();
    ControlInit();
    ComInit();
    LedInit();
    MoterInit();
    TimerInit();
    DelayMs(100);//等待系统上电稳定
    INTEN
    while(1) {
        //WWDG_CR = 0xc0;
        if(TimerGetTimeFlag() > 800) {
            TimerClearTimeFlag();
            MoterSleep();
        }
        LedTimeService();
        if(ComGetFlag() == 0x80) {
            ComClearFlag();
            TimerClearTimeFlag();
            LedSetModeFlicker(1);
            MoterOpen();
            switch(ComGetData(0)) {
                case exchange_stal:
                    if(ControlGetStall() == 1) {//下链条
                        ControlSetStall(0);
                        if( ControlRunPosition(ControlCalculateGrating2(1,3)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(2,2));
                            ControlSetStall(1);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        }
                        DelayMs(600);
                        if( ControlRunPosition(ControlCalculateGrating2(1,4)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(2,2));
                            ControlSetStall(1);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        }
                        if( ControlRunPosition(ControlCalculateGrating2(3,5)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(2,2));
                            ControlSetStall(1);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        }
                    } else {//上链
                        u8 flag = 0;
                        u8 delay_flag = 0;
                        ControlSetStall(1);
                        flag = ControlRunPosition(ControlCalculateGrating2(0,0));
                        if( flag == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(3,5));
                            ControlSetStall(0);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        } else if(flag == 0x81){
                            delay_flag = 1;
                        }
                       // DelayMs(500);
                        flag = ControlRunPosition(ControlCalculateGrating2(0,1));
                        if( flag == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(3,5));
                            ControlSetStall(0);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        } 
//                        else if(flag == 0x81){
//                            delay_flag = 1;
//                        }
//                        if(delay_flag == 1) {
//                            DelayMs(3000);
//                        } else {
//                            DelayMs(600);
//                        }
                        //DelayMs(100);
                        if( ControlRunPosition(ControlCalculateGrating2(2,2)) == 0x44 ) {
                            ControlRunPosition(ControlCalculateGrating2(3,5));
                            ControlSetStall(0);
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                            break;
                        }
                    }
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case add_setp:
                    if( ControlRunPosition(15) == 0x44 ){
                        ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        break;
                    }
                break;
                case sub_setp:
                    if( ControlRunPosition(-15) == 0x44 ){
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
                    if(ControlGetRear() == ComGetData(1)) {
                        break;
                    }
                    ControlSetRear(ComGetData(1));
                    if(ControlGetStall() == 1) {//上链
//                        if(ControlGetRear() > ComGetData(1)) {
//                            if(ControlGetRear() == 5) {
//                                ControlRunPosition(100);
//                            }
//                        } else {
//                            if(ControlGetRear() == 5) {
//                                ControlRunPosition(-80);
//                            }
//                        }
                        if( ControlRunPosition(ControlCalculateGrating2(2,2)) == 0x44 ){
                                ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                                break;
                            }
//                        if( (ControlGetRear() == 0x06) || (ControlGetRear() == 0x05) || (ControlGetRear() == 0x04)) {
//                            if( ControlRunPosition(ControlCalculateGrating(2,2)) == 0x44 ){
//                                ComSendCmd(stuck, ControlGetStall() ,0 ,0);
//                                break;
//                            }
//                        }
                    } else {
//                        if(ControlGetRear() > ComGetData(1)) {
//                            if(ControlGetRear() == 5) {
//                                ControlRunPosition(150);
//                            }
//                        } else {
//                            if(ControlGetRear() == 5) {
//                                ControlRunPosition(-130);
//                            }
//                        }
                        if( ControlRunPosition(ControlCalculateGrating2(3,5)) == 0x44 ){
                                ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                                break;
                        }
//                        if( (ControlGetRear() == 0x03) || (ControlGetRear() == 0x04) || (ControlGetRear() == 0x05) || 
//                           (ControlGetRear() == 0x06) ||(ControlGetRear() == 0x07) || (ControlGetRear() == 0x08) ) {
//                            if( ControlRunPosition(ControlCalculateGrating(3,1)) == 0x44 ){
//                                ComSendCmd(stuck, ControlGetStall() ,0 ,0);
//                                break;
//                            }
//                        }
                    }
                    //ControlSetRear(ComGetData(1));
                break;
                default:break;
            }
            ComClearData();
            //MoterSleep();
        }
    }
}

