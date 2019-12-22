#ifndef __key_H_
#define __key_H_
#include "stm32f10x.h"
/*
#define KEY0 PEin(4)
#define KEY1 PEin(3)
#define KEY2 PEin(2)
#define WK_UP PAin(0)
*/
//λ�����궨�尴����ȡ


#define KEY0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)
#define KEY1 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
#define KEY2 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)
#define WK_UP GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//�궨�尴����ȡ

#define KEY0_PRESS 1 
#define KEY1_PRESS 2
#define KEY2_PRESS 3
#define WK_UP_PRESS 4 //���°���


void key_init(void);
u8 KEY_SCAN(u8);

#endif

