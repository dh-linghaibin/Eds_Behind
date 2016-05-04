
#include "Timer.h"
#include "Moter.h"

void TimerInit(void) {
    TIM4_IER = 0x00;//       
    TIM4_EGR = 0x01;// 
    TIM4_PSCR = 0x07;// 计数器时钟=主时钟/128=16MHZ/128
    TIM4_ARR = 0xFA;// 设定重装载时的寄存器值，255是最大值
    //TIM4_CNTR = 0x00;// 设定计数器的初值
    // 定时周期=(ARR+1)*64=16320uS
    TIM4_IER = 0x01;//   
    TIM4_CR1 = 0x01;  
}

static u16 time_flag = 0;

u16 TimerGetTimeFlag(void) {
    return time_flag;
}

void TimerClearTimeFlag(void) {
    time_flag = 0;
}

static u16 time_speed = 0;

u16 TimerGetSpeed(void) {
    return time_speed;
}

#pragma vector=0x19
__interrupt void TIM4_UPD_OVF_IRQHandler(void)
{
    INTOFF
    static u16 count_time = 0;
    static u8 count_sleep = 0;
    TIM4_SR = 0x00;
    
    if(count_time < 400) {//400 test for 2016/1/19
        count_time++;
    } else {
        count_time = 0;
        time_flag++;
    }
    
    if(count_sleep < 50) {
        count_sleep++;
    } else {
        count_sleep = 0;
        time_speed = MoterGetSleep();
        MoterSetSleep(0);
    }
    INTEN
    return;
}


