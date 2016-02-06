

#include "Led.h"

#define LED PD_ODR_ODR6

void LedInit(void) {
    PD_DDR_DDR6 = 1;
    PD_CR1_C16 = 1;
    PD_CR2_C26 = 1;
    
    LED = 0;
}

void LedSet(u8 cmd) {
    LED = cmd;
}

