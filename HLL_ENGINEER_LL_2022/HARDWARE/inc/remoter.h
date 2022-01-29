#ifndef __REMOTER_H
#define __REMOTER_H

#include "stm32f4xx.h"
#include "bsp_usart.h"

/* 按键定义 */
#define KEY_W        0x01
#define KEY_A        0x04
#define KEY_S        0x02
#define KEY_D        0x08

#define KEY_CTRL     0x20
#define KEY_SHIFT    0x10

#define KEY_Z        0x800
#define KEY_X        0x1000
#define KEY_C        0x2000
#define KEY_V        0x4000
#define KEY_B        0x8000
#define KEY_F        0x200
#define KEY_G        0x400
#define KEY_Q        0x40
#define KEY_E        0x80
#define KEY_R        0x100

#define RC_KEY_PRESSED(rc_key,key) (rc_key & key)

typedef __packed struct
{
        __packed struct
        {
                int16_t ch0;
                int16_t ch1;
                int16_t ch2;
                int16_t ch3;
                int16_t ch4;
                uint8_t s1;
                uint8_t s2;
        } rc;
	
        __packed struct
        {
                int16_t x;
                int16_t y;
                int16_t z;
                uint8_t press_l;
                uint8_t press_r;
        } mouse;
				
        __packed struct
        {
                uint16_t value;
        } key;
				
        __packed struct
        {
                int16_t ch2; //左右
		            int16_t ch3; //前后
                int16_t ch0; //左右转
					      int16_t ch1; //抬头低头
        } virtual_rocker;

} Rc_ctrl_t;


void Parse_Remoter_Data(volatile const uint8_t *sbus_buf, Rc_ctrl_t *rc_ctrl);
uint8_t Remoter_Data_Check(Rc_ctrl_t *remote_controller);
void Rc_Data_Copy(Rc_ctrl_t* rc1, Rc_ctrl_t* rc2);
void Rc_Data_Reset(Rc_ctrl_t* rc);
void Wasd_Key_To_Virtual_Rocker(Rc_ctrl_t* rc);


#endif

