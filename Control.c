
#include "Control.h"
#include "Moter.h"
#include "Timer.h"
#include "Eeprom.h"
#include "Delay.h"

#define TRANDFORMATIONP 25

#define SPEEDP 2.5
                              //     25000  8000   16000    
                                //0,20000,6000,10000
                            //上链位置 下链位置 上链回来位置 下链回来位置
static const u16 stalls_add[4] = {200,25000,0,22000};
                          //0,21000,4000,12000
                       //上链位置 
static const u16 stalls_read_add1[11] = {2000,2000,2000,3000,4000,5000,8000,8000,8000,8000,8000};
static const u16 stalls_read_add2[11] = {6000,6000,6000,6000,6000,0,0,0,0,0,0};//下链//{0,0,2000,2000,5000,5000,5000,8000,8000,8000};//下链
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
//
//int ControlCalculateGrating(u8 stalss,u8 cmd) {
//    u16 res_position_absolutely = 0;
//    u16 res_position_new = 0;
//    u16 res_position_different = 0;
//    u8 symbol_bit = 0;
//    int rotate_num = 0;
//    if(cmd == 0) {
//        res_position_absolutely = stalls_start - stalls_add[stalss];//count position
//    } else if(cmd == 1) {//下链
//        res_position_absolutely = stalls_start - stalls_add[stalss] - stalls_read_add2[stalls_rear];//count position
//    } else if(cmd == 2) {//上链
//        res_position_absolutely = stalls_start - stalls_add[stalss] - stalls_read_add1[stalls_rear];//count position
//    }
//    res_position_new = ConterResistancePositionFiltering();//get position
//    if(res_position_absolutely > res_position_new) {
//        symbol_bit = 0;
//        res_position_different = res_position_absolutely - res_position_new;
//    } else {
//        symbol_bit = 1;
//        res_position_different = res_position_new - res_position_absolutely;
//    }
//    rotate_num = (u16)(res_position_different/TRANDFORMATIONP);
//    if(symbol_bit == 1) {
//        rotate_num = 0 - rotate_num;
//    }
//    return rotate_num;
//}

int num_sss = 0; 
u16 unm_weizhiwe = 0;

u8 ControlRunPosition(int num) {
    static u8 dr = 0;
    static u16 position_difference = 0;
   // static u8 sleep_sub = 0;
    u16 current = 0,current_count = 0;//current_count2 = 0;
    MoterSetCodingSite(0);//clear
    num_sss = num;
    if(num > 0) {
        dr = 1;
        MoterSpeed(dr,0xff);
    } else if(num < 0){
        dr = 2;
        num = - num;
        MoterSpeed(dr,0xff);
    } else {
        MoterSpeed(3,0);
        return 0x80;
    }
    do {
        if(num > MoterReadCodingSite()) {
            position_difference = num - MoterReadCodingSite();
        } else {
            position_difference = 0;
        }
        current = MoterReadCurrent();
        if(current > 30000) {
            if(current_count < 30000) {
                current_count++;
            } else {
                MoterSpeed(3,0);//stop
                return 0x44;
            }
        } else {
            current_count = 0;
        }
    } while(position_difference > 40);
    MoterSpeed(3,0);//stop
    DelayMs(50);
    MoterSpeed(dr,220);
    do {
        if(num > MoterReadCodingSite()) {
            position_difference = num - MoterReadCodingSite();
        } else {
            position_difference = 0;
        }
        current = MoterReadCurrent();
        if(current > 30000) {
            if(current_count < 30000) {
                current_count++;
            } else {
                MoterSpeed(3,0);//stop
                return 0x44;
            }
        } else {
            current_count = 0;
        }
    } while(position_difference > 30);
    MoterSpeed(3,0);//stop
    DelayMs(50);
    MoterSpeed(dr,200);
    do {
        if(num > MoterReadCodingSite()) {
            position_difference = num - MoterReadCodingSite();
        } else {
            position_difference = 0;
        }
        current = MoterReadCurrent();
        if(current > 30000) {
            if(current_count < 30000) {
                current_count++;
            } else {
                MoterSpeed(3,0);//stop
                return 0x44;
            }
        } else {
            current_count = 0;
        }
    } while(position_difference > 5);
    MoterSpeed(3,0);//stop
    DelayMs(50);
    unm_weizhiwe = ConterResistancePositionFiltering();
    return 0x80;
}

u16 unm_xxxxx = 0;

//
int ControlCalculateGrating2(u16 stalss,u8 cmd) {
    u16 res_position_absolutely = 0;
    u16 res_position_new = 0;
    u16 res_position_different = 0;
    u8 symbol_bit = 0;
    int rotate_num = 0;
    switch(cmd) {
        case 0://上链条
            res_position_absolutely = stalls_start + stalls_add[stalss] - 3000;//count position -3000
        break;
        case 1:
            res_position_absolutely = stalls_start + stalls_add[stalss];//count position
        break;
        case 2:
            res_position_absolutely = stalls_start + stalls_add[stalss] - stalls_read_add1[stalls_rear];
        break;
        case 3://下链条
            res_position_absolutely = stalls_start - stalls_add[stalss] + 6000;//count position +4000
        break;
        case 4:
            res_position_absolutely = stalls_start - stalls_add[stalss];//count position
        break;
        case 5:
            res_position_absolutely = stalls_start - stalls_add[stalss] + stalls_read_add2[stalls_rear];//count position
        break;
    }
    unm_xxxxx = res_position_absolutely;
    res_position_new = ConterResistancePositionFiltering();//get position
    if(res_position_absolutely > res_position_new) {
        symbol_bit = 0;
        res_position_different = res_position_absolutely - res_position_new;
    } else {
        symbol_bit = 1;
        res_position_different = res_position_new - res_position_absolutely;
    }
    rotate_num = (u16)(res_position_different/TRANDFORMATIONP);
    if(rotate_num > 1000) {
        return 0x00;
    }
    if(symbol_bit == 1) {
        rotate_num = 0 - rotate_num;
    }
    return rotate_num;
}

void ControlDelay(u16 count) {
    /*
    u16 current = 0,current_count = 0;
    DelayMs(50);
    while(current_count < 30000) {
        current_count++;
        current = MoterReadCurrent();
        if(current > 50000) {
            if(current_count < 30000) {
                current_count++;
            } else {
                return 0x44;
            }
        } else {
            current_count = 0;
        }
    }*/
}
