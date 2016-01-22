
#include "Control.h"
#include "Moter.h"
#include "Timer.h"
#include "Eeprom.h"

#define TRANDFORMATIONP 50

#define SPEEDP 2.5

static u16 stalls_add[11] = {0,2000,5000,6500,8500,10500,12000,14500,16500,19000,22000};
static u16 stalls_start = 45000;

void ControlInit(void) {
    if(EepromRead(10) != 0x55) {
        EepromWrite(10, 0x55);
        EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
        EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
    }
    //stalls_start = TypeCombinationU16(EepromRead(11), EepromRead(12));
}

void ControlSetStart(u16 data) {
    stalls_start = data;
    EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
    EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
}

static int num = 0;

int ControlCalculateGrating(u8 stalss) {
    u16 res_position_absolutely = 0;
    u16 res_position_new = 0;
    u16 res_position_different = 0;
    u8 symbol_bit = 0;
    int rotate_num = 0;
    res_position_absolutely = stalls_add[stalss] + stalls_start;//count position
    res_position_new = MoterReadResistancePosition();//get position
    if(res_position_absolutely > res_position_new) {
        symbol_bit = 0;
        res_position_different = res_position_absolutely - res_position_new;
    } else {
        symbol_bit = 1;
        res_position_different = res_position_new - res_position_absolutely;
    }
    rotate_num = (u16)(res_position_different/TRANDFORMATIONP);
    if(symbol_bit == 1) {
        rotate_num = 0 - rotate_num;
    }
    num = rotate_num;
    return rotate_num;
}

u16 current = 0;

u8 ControlRunPosition(int num) {
    static u8 dr = 0;
    static u16 position_difference = 0;
    static u8 sleep_sub = 0;
    MoterSetCodingSite(0);//clear
    if(num > 0) {
        dr = 1;
        //MoterSpeed(1,MOTOSLEEP);
    } else if(num < 0){
        dr = 2;
        //MoterSpeed(2,MOTOSLEEP);
        num = - num;
    } else {
        MoterSpeed(3,0);
        return 0x80;
    }
    do {
        position_difference = num - MoterReadCodingSite();
        MoterSpeed(dr, (u8)((100 - (TimerGetSpeed() + sleep_sub) ) * SPEEDP));
        if(position_difference < 10) {
            sleep_sub = 40 - position_difference * 5;
        }
        //current = MoterReadCurrent();
    } while(position_difference > 3);
    
    MoterSpeed(3,0);//stop
    
    return 0x80;
}
