
/*
1、CAN1过滤器配置
2、RX0挂起中断回调函数
3、CAN1发*2（于是最多可以发8个3508电机）
*/
#include "can_bsp.h"
CAN_RxHeaderTypeDef RX1message;
CAN_TxHeaderTypeDef TX1message;
uint8_t TXdata[8];
uint8_t RXdata[8];
uint32_t FlashTimer;//滴答时钟，后续会防止掉线操作（目前不影响）
//Filter 1
void CAN1_FILTER_CONFIG(CAN_HandleTypeDef* hcan)
{
	CAN_FilterTypeDef		CAN_FilterConfigStructure;		
	CAN_FilterConfigStructure.FilterBank = 0;
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = 0x7FFE;
	CAN_FilterConfigStructure.FilterIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterConfigStructure.SlaveStartFilterBank = 14;
	CAN_FilterConfigStructure.FilterActivation = ENABLE;
		
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterConfigStructure);
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);	
}
//Filter 2
void CAN2_FILTER_CONFIG(CAN_HandleTypeDef* hcan)
{
	CAN_FilterTypeDef		CAN_FilterConfigStructure;		
	CAN_FilterConfigStructure.FilterBank = 14;
	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = 0xFFFF;
	CAN_FilterConfigStructure.FilterIdLow = 0x7FFF;
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterConfigStructure.SlaveStartFilterBank = 14;
	CAN_FilterConfigStructure.FilterActivation = ENABLE;
		
		HAL_CAN_ConfigFilter(&hcan2,&CAN_FilterConfigStructure);
		HAL_CAN_Start(&hcan2);
		HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}


//RX0中断回调
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	if(HAL_GetTick() - FlashTimer>500)//can通信正常--开提示灯
  {	
		FlashTimer = HAL_GetTick();
  }
	//can1
	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RX1message, RXdata)==HAL_OK)
	{
		if(hcan->Instance == CAN1)      //CAN1依ID处理数据
		{
				switch(RX1message.StdId)
				{
					
				}
		}
		
	}
	//can2
	if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RX1message, RXdata)==HAL_OK)
	{
		if(hcan->Instance == CAN2)       
		{
			switch(RX1message.StdId)
			{ 
        
			}
		}
	}
}


//can1发->0x200  
void SET_CAN1Ahead_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4)    //CAN1 ID:1~4
{	
	TX1message.StdId = 0x200;                        //电调标识符
	TX1message.IDE = CAN_ID_STD;
	TX1message.RTR = CAN_RTR_DATA;
	TX1message.DLC = 8;                              //发送长度 (x字节)
	TX1message.TransmitGlobalTime = DISABLE;
	
	TXdata[0] = K1 >> 8;
	TXdata[1] = K1;
	TXdata[2] = K2 >> 8;
	TXdata[3] = K2;
	TXdata[4] = K3 >> 8;
	TXdata[5] = K3;
	TXdata[6] = K4 >> 8;
	TXdata[7] = K4;
	HAL_CAN_AddTxMessage(&hcan1, &TX1message, TXdata, CAN_FILTER_FIFO0);    //将数据储存进邮箱FIFOx
}
//can1发->0x1FF
void SET_CAN1Back_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4)    //CAN1	ID:5~8
{	
	TX1message.StdId = 0x1FF;
	TX1message.IDE = CAN_ID_STD;
	TX1message.RTR = CAN_RTR_DATA;
	TX1message.DLC = 8;
	TX1message.TransmitGlobalTime = DISABLE;
	
	TXdata[0] = K1 >> 8;
	TXdata[1] = K1;
	TXdata[2] = K2 >> 8;
	TXdata[3] = K2;
	TXdata[4] = K3 >> 8;
	TXdata[5] = K3;
	TXdata[6] = K4 >> 8;
	TXdata[7] = K4;
	HAL_CAN_AddTxMessage(&hcan1, &TX1message, TXdata, CAN_FILTER_FIFO0);
}

//can2发->0x200  
void SET_CAN2Ahead_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4)    //CAN2 ID:1~4
{	
	TX1message.StdId = 0x200;
	TX1message.IDE = CAN_ID_STD;
	TX1message.RTR = CAN_RTR_DATA;
	TX1message.DLC = 8;
	TX1message.TransmitGlobalTime = DISABLE;
	
	TXdata[0] = K1 >> 8;
	TXdata[1] = K1;
	TXdata[2] = K2 >> 8;
	TXdata[3] = K2;
	TXdata[4] = K3 >> 8;
	TXdata[5] = K3;
	TXdata[6] = K4 >> 8;
	TXdata[7] = K4;
	HAL_CAN_AddTxMessage(&hcan2, &TX1message, TXdata, CAN_FILTER_FIFO0);
}
//can2发->0x1FF
void SET_CAN2Back_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4)    //CAN2 ID:5~8
{	
	TX1message.StdId = 0x1FF;
	TX1message.IDE = CAN_ID_STD;
	TX1message.RTR = CAN_RTR_DATA;
	TX1message.DLC = 8;
	TX1message.TransmitGlobalTime = DISABLE;
	
	TXdata[0] = K1 >> 8;
	TXdata[1] = K1;
	TXdata[2] = K2 >> 8;
	TXdata[3] = K2;
	TXdata[4] = K3 >> 8;
	TXdata[5] = K3;
	TXdata[6] = K4 >> 8;
	TXdata[7] = K4;
	HAL_CAN_AddTxMessage(&hcan2, &TX1message, TXdata, CAN_FILTER_FIFO0);
}


