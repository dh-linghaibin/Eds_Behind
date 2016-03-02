
#include "Control.h"
#include "Moter.h"
#include "Timer.h"
#include "Eeprom.h"
#include "Delay.h"

#define TRANDFORMATIONP 35

#define SPEEDP 2.5
                              //     25000  8000   16000    
                                //0,20000,6000,10000
                            //上链位置 下链位置 上链回来位置 下链回来位置
static const u16 stalls_add[4] = {0,30000,6000,10000};
                          //0,21000,4000,12000
                       //上链位置 
static const u16 stalls_read_add1[10] = {0,0,0,0,0,0,3000,3000,3000,3000};
static const u16 stalls_read_add2[10] = {0,0,0,0,5000,5000,5000,5000,5000,5000};//下链
static u16 stalls_start = 42000;
                        //45000
                        //47000
static u8 stalls = 0;
static u8 stalls_rear = 0;

void ControlInit(void) {
    if(EepromRead(10) != 0x55) {
        EepromWrite(10, 0x55);
        EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
        EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
        EepromWrite(13, stalls);
        EepromWrite(14, stalls_rear);
    }
    stalls = EepromRead(13);
    stalls_rear = EepromRead(14);
    stalls_start = TypeCombinationU16(EepromRead(11), EepromRead(12));
}

/*冒泡排序*/
u16 ConterResistancePositionFiltering(void) {
    u16 res[3] = {0,0,0};
    for(u8 i = 0; i < 3; i++) {
        res[i] = MoterReadResistancePosition();//get position
        DelayMs(100);
    }
    for(u8 i = 0; i < 3; i++) {
        for(u8 j = 0; i + j < 2; j++) {
            if(res[j] > res[j + 1]) {
                int temp = res[j];
                res[j] = res[j + 1];
                res[j + 1] = temp;
            }
        }
    }
    return res[1];
}

void ControlSetStart(void) {
    stalls_start = ConterResistancePositionFiltering();
    EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
    EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
    ControlSetStall(0);
}

void ControlSetRear(u8 cmd) {
    stalls_rear = cmd;
    EepromWrite(14, stalls_rear);
}

u8 ControlGetRear(void) {
    return stalls_rear;
}

u8 ControlGetStart(void) {
    return stalls_start;
}

static u8 stalls_fist = 0;

u8 ControlSetStallsSub(void) {
    if(stalls > 0) {
        stalls_fist = (stalls -1);
        return stalls_fist;
    } else {
        return 0x44;
    }
}

u8 ControlSetStallsAdd(void) {
    if(stalls < 1) {
        stalls_fist = (stalls + 1);
        return stalls_fist;
    } else {
        return 0x44;
    }
}

u8 ControlGetStall(void) {
    return stalls;
}

void ControlSetStall(u8 cmd) {
    stalls = cmd;
    EepromWrite(13, stalls);
}

int ControlCalculateGrating(u8 stalss,u8 cmd) {
    u16 res_position_absolutely = 0;
    u16 res_position_new = 0;
    u16 res_position_different = 0;
    u8 symbol_bit = 0;
    int rotate_num = 0;
    if(cmd == 0) {
        res_position_absolutely = stalls_start - stalls_add[stalss];//count position
    } else if(cmd == 1) {//下链
        res_position_absolutely = stalls_start - stalls_add[stalss] - stalls_read_add2[stalls_rear];//count position
    } else if(cmd == 2) {//上链
        res_position_absolutely = stalls_start - stalls_add[stalss] - stalls_read_add1[stalls_rear];//count position
    }
    res_position_new = ConterResistancePositionFiltering();//get position
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
    return rotate_num;
}


u8 ControlRunPosition(int num) {
    static u8 dr = 0;
    static u16 position_difference = 0;
    static u8 sleep_sub = 0;
    u16 current = 0,current_count = 0;
    MoterSetCodingSite(0);//clear
    if(num > 0) {
        dr = 1;
        MoterSpeed(dr,0xff);
    } else if(num < 0){
        dr = 2;
        num = - num;
        MoterSpeed(dr,0xff);
    } else {
        MoterSpeed(3,0);
        //ControlSetStall(stalls_fist);//suff
        return 0x80;
    }
    do {
        position_difference = num - MoterReadCodingSite();
        //MoterSpeed(dr, (u8)((100 - (TimerGetSpeed() + sleep_sub) ) * SPEEDP));
        if(position_difference < 10) {
            sleep_sub = 40 - position_difference * 5;
        }
        current = MoterReadCurrent();
        if(current > 50000) {
            if(current_count < 5000) {
                current_count++;
            } else {
                MoterSpeed(3,0);//stop
                return 0x44;
            }
        } else {
            current_count = 0;
        }
    } while(position_difference > 10);
    
    MoterSpeed(3,0);//stop
   // ControlSetStall(stalls_fist);//suff
    return 0x80;
}


//
int ControlCalculateGrating2(u16 stalss,u8 cmd) {
    u16 res_position_absolutely = 0;
    u16 res_position_new = 0;
    u16 res_position_different = 0;
    u8 symbol_bit = 0;
    int rotate_num = 0;
    /*
    if(cmd == 0) {
        res_position_absolutely = stalls_start - stalls_add[stalss];//count position
    } else if(cmd == 1) {//下链
        res_position_absolutely = stalls_start - stalls_add[stalss] - stalls_read_add2[stalls_rear];//count position
    } else if(cmd == 2) {//上链
        res_position_absolutely = stalls_start - stalls_add[stalss] - stalls_read_add1[stalls_rear];//count position
    }*/
    switch(cmd) {
        case 0:
            res_position_absolutely = stalls_start - stalls_add[stalss] - 3000;//count position
        break;
        case 1:
            res_position_absolutely = stalls_start - stalls_add[stalss];//count position
        break;
        case 2:
            res_position_absolutely = stalls_start - stalls_add[stalss] - stalls_read_add1[stalls_rear];
        break;
        case 3:
            res_position_absolutely = stalls_start - stalls_add[stalss] + 10000;//count position
        break;
        case 4:
            res_position_absolutely = stalls_start - stalls_add[stalss];//count position
        break;
        case 5:
            res_position_absolutely = stalls_start - stalls_add[stalss] - stalls_read_add2[stalls_rear];//count position
        break;
    }
    res_position_new = ConterResistancePositionFiltering();//get position
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
    return rotate_num;
}

