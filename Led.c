

#include "Type.h"

#define LED PA_ODR_ODR2

void LedInit(void) {
    PA_DDR_DDR2 = 1;
    PA_CR1_C12 = 1;
    PA_CR2_C22 = 1;
    
    LED = 0;
}

void LedSet(u8 cmd) {
    LED = cmd;
}

