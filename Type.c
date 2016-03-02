
#include "Type.h"

u8 TypeDecomposeU16(u16 data, u8 cmd) {
    u8 data_n = 0;
    if(cmd == 0) {
        data_n = (u8)data;
    } else {
        data_n = (u8)(data >> 8);
    }
    return data_n;
}

u16 TypeCombinationU16(u8 data_h, u8 data_l) {
    u16 data = 0;
    data = (u16)data_h;
    data |= (u16)(data_l << 8);
    return data;
}




