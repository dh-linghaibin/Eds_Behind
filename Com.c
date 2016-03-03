

#include "Com.h"
#include "Delay.h"

void ComInit(void) {
    //Watch
    PD_DDR_DDR4 = 0;
    PD_CR1_C14 = 0;
    PD_CR2_C24 = 1;  //test
    
    //�ⲿ�жϳ�ʼ��
	EXTI_CR1 &= ~BIT(6);//����PD���ж�
	EXTI_CR1 &= ~BIT(7);
}

#define COM_BIT_OUT 	PD_ODR_ODR4
#define COM_BIT_IN 	    PD_IDR_IDR4
#define COM_BIT_DR      PD_DDR_DDR4
#define COM_BIT_INT     PD_CR2_C24

u8 ComSend(u8 data[]) {
	u16 wait = 0;
	u8 data_t = 0;//������ʱֵ
	u8 i = 0,j = 0;
	
	COM_BIT_INT = 0;//�ж�
	COM_BIT_DR = 1;//����Ϊ���
	COM_BIT_OUT = 0;
	DelayUs(100);//����20ms˵�����߿�ʼ
	COM_BIT_DR = 0;//����Ϊ����
	DelayUs(1);
	while(COM_BIT_IN == 1) {//�ȴ��ӻ�����
		if(wait < 100) {
			wait++;
		} else {//��ʱ���˳�
		
			COM_BIT_INT = 1;//�ж�
			return 0;
		}
	}
	wait = 0;
	while(COM_BIT_IN == 0) {
		if(wait < 100) {
			wait++;
		} else {//��ʱ���˳�
		
			return 0;
		}
	}
	COM_BIT_DR = 1;//����Ϊ���
	for(j = 0;j < 5;j++) {
		data_t = data[j];
		for(i=0;i<8;i++) {
			COM_BIT_OUT = 0;
			if(data_t&0x80) {
				DelayUs(80);
			} else {
				DelayUs(40);
			}
			COM_BIT_OUT = 1;
			DelayUs(20);
			data_t<<=1;
		}
	}
	DelayUs(60);
	COM_BIT_OUT = 1;
	COM_BIT_INT = 1;//�ж�
	COM_BIT_DR = 0;//����Ϊ����
	return 0;
}

u8 ComRead(u8 data_s[]) {
    static u16 wait = 0;
    u8 i = 0,j = 0;
	u8 data = 0;
	wait = 0;

	COM_BIT_DR = 0;//����Ϊ����
	while(COM_BIT_IN == 0) {
		if(wait < 60) {
			wait++;
		} else {
			return 0x44;
		}
	}
	if(wait > 28) {
		wait = 0;
		COM_BIT_DR = 1;//����Ϊ���
		COM_BIT_OUT = 0;
		DelayUs(1);
		COM_BIT_DR = 0;//����Ϊ����
		//��ʼ��������
		for(j = 0;j < 5;j++) {
			for(i=0;i<8;i++) {  
				data<<=1; 
				while(COM_BIT_IN == 1) {
					if(wait < 200) {
						wait++;
					} else {
						return 0x44;
					}
				}
				wait = 0;
				while(COM_BIT_IN == 0) {
					if(wait < 200) {
						wait++;
					} else {
						return 0x44;
					}
				}
				if(wait > 30) {//Ϊ1
					data|=0x01;  
				}
				wait = 0;					
			}
			data_s[j] = data;
			data = 0;
		}
		if(data_s[4] == (data_s[0]+data_s[1]+data_s[2]+data_s[3])) {//�ۼ�У��
			if(data_s[4] != 0) {
				return 0x88;
			} else {
				return 0x44;
			}
		} else {
			return 0x44;
		}
	} else {//ʱ�䲻�� �Ƴ�
		return 0x44;
	}
} 


static u8 com_date[5];
static u8 rs_ok = 0;//whether it has received data

u8 ComGetData(u8 num) {
    return com_date[num];
}

void ComClearData(void) {
    u8 claer_i = 0;
    for( ; claer_i < 5; claer_i++) {
        com_date[claer_i] = 0x00;
    }
}

u8 ComGetFlag(void) {
    return rs_ok;
}

void ComClearFlag(void) {
    rs_ok = 0;
}

void ComSendCmd(u8 cmd,u8 par1,u8 par2,u8 par3) {
    u8 com_t_data[5] = {0,0,0,0,0};//ǰ��
	com_t_data[0] = cmd; //cmd
	com_t_data[1] = par1;
	com_t_data[2] = par2;
	com_t_data[3] = par3;
    com_t_data[4] = com_t_data[0]+com_t_data[1]+com_t_data[2]
                                    +com_t_data[3];
    INTOFF
	ComSend(com_t_data);
    INTEN
}

#pragma vector=8
__interrupt void EXTI_PORTD_IRQHandler(void)
{
    INTOFF
    if(ComRead(com_date) == 0x88) {
        rs_ok = 0x80;
    }
    INTEN
}