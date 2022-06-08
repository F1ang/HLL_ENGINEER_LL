/*********************************************************************
 * @file UI_Draw.c
 * @author ZQ-M (@HLL)
 * @brief 图层序号越小，显示优先级越高
 * @version
 * @date 2022-03-18
 * 
 * @copyright Copyright (c) 2022
 * 
*********************************************************************/

#include "UI_Draw.h"

/* 分别测试静态直线矩形圆圆弧和字符 */
Graph_Data line_t,rectangle_t,circle_t,arc_t;
String_Data Char_t;
/* 压力测试动态浮点字符 */
Graph_Data F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,F13,F14;
/* 数据帧发送上限113 */
uint8_t SendBuff[113];
/* 设置打印字符 */
char HLL_arr[3]="HLL";

/*********************************************************************
* @brief 获取长度并发送数据
* 
* @param ch 要发送的数组，这里固定是SendBuff
*********************************************************************/
void UI_SendALL(unsigned char ch[113])
{
	/* 获取发送数据长度 */ 
	uint16_t len = UI_Get_Send_Data_Len(ch);
	for(uint16_t i=0; i<len; i++)
		{
			UART8->DR = (uint8_t) ch[i];
			while((UART8->SR&0X40)==0);//循环发送,直到发送完毕 
		}
}

void Start_UI(void){
		/* 设置机器人ID */
		UI_Set_Comparable_Id(UI_Data_CilentID_BEngineer);

		/* 设置静态UI--直线 */
		//存图像buff,标识，操作，图层，颜色，线宽，X,Y,X_E,Y_E
		Line_Draw(&line_t,"001",UI_Graph_ADD,9,UI_Color_Main,20,961,480,961,600);
		UI_ReFresh(SendBuff,1,line_t);
		UI_SendALL(SendBuff);

		/* 设置静态UI--矩形 */
		//存图像buff,标识，操作，图层，颜色，线宽，X,Y,X_E,Y_E
		Rectangle_Draw(&rectangle_t,"002",UI_Graph_ADD,9,UI_Color_Main,5,900,420,1020,600);
		UI_ReFresh(SendBuff,1,rectangle_t);
		UI_SendALL(SendBuff);

		/* 设置静态UI--整圆 */	
		//存图像buff,标识，操作，图层，颜色，线宽，圆心(X，Y)，R
		Circle_Draw(&circle_t,"003",UI_Graph_ADD,9,UI_Color_Main,5,961,540,150);
		UI_ReFresh(SendBuff,1,circle_t);
		UI_SendALL(SendBuff);

		/* 设置静态UI--圆弧 */
		//存图像buff,标识，操作，图层，颜色，开始角度，终止角度，线宽，圆心(X,Y)，X轴长，Y轴长
		Arc_Draw(&arc_t,"004",UI_Graph_ADD,9,UI_Color_Main,20,-20,20,961,540,50,50);
		UI_ReFresh(SendBuff,1,arc_t);
		UI_SendALL(SendBuff);

		/* 设置静态字符--HLL队名 */
		//存图像buff,标识，操作，图层，颜色，字号，字符个数，线宽，X，Y，字符串
		Char_Draw(&Char_t,"005",UI_Graph_ADD,7,UI_Color_Yellow,48,3,4,960,540,HLL_arr);
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
}

/*
	1920*1080---X,Y
	1)采矿成功，main->green
	2)显示各个变量状态
	3)显示各个兵种HP
	4)可绘范围X:960±300 Y:540±280
*/
float Dynamic_Data = 0.12;
extern uint8_t chip_flag;
extern Judge_data_t *judge_data;
extern Robot_mode_t robot_mode;     
void Dynamic_UI(void){
		static char change_color;
		UI_Delete(SendBuff,UI_Data_Del_Layer,0);
		UI_SendALL(SendBuff);
	
		if(chip_flag==1)
		{
			change_color=UI_Color_Green;
		}
		else 
		{
			change_color=UI_Color_Main;
		}
		/*十字*/
		CSGO_Cross_AIMLINE(change_color);

		/*STATUS*/
		//存图像buff,标识，操作，图层，颜色，字号，字符个数，线宽，X，Y，字符串
		Char_Draw(&Char_t,"006",UI_Graph_ADD,0,UI_Color_Main,16,3,4,640,360,"Overturn");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		//存图像buff，标识，图层操作，图层，颜色，字体，小数位数，线宽，开始坐标(x,y)，要显示的变量
		Float_Draw(&F1,"007",UI_Graph_ADD,0,UI_Color_Main,12,1,2,660,340,robot_mode.mode_overturn);
		
		Char_Draw(&Char_t,"008",UI_Graph_ADD,0,UI_Color_Main,16,3,4,640,400,"Mode_up");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		Float_Draw(&F2,"009",UI_Graph_ADD,0,UI_Color_Main,12,1,2,640,380,robot_mode.mode_up);
		
		Char_Draw(&Char_t,"010",UI_Graph_ADD,0,UI_Color_Main,16,3,4,640,440,"Mode_rescue");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		Float_Draw(&F3,"011",UI_Graph_ADD,0,UI_Color_Main,12,1,2,640,420,robot_mode.mode_rescue);
		
		Char_Draw(&Char_t,"012",UI_Graph_ADD,0,UI_Color_Main,16,3,4,640,480,"Mode_revive");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		Float_Draw(&F4,"013",UI_Graph_ADD,0,UI_Color_Main,12,1,2,640,460,robot_mode.mode_revive);
		
		/*HP*/
		Char_Draw(&Char_t,"014",UI_Graph_ADD,0,UI_Color_Main,16,3,4,1200,360,"Blue_1");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		Float_Draw(&F5,"015",UI_Graph_ADD,0,UI_Color_Main,12,1,2,1200,340,judge_data->ext_game_robot_HP_t.blue_1_robot_HP);
		
		UI_ReFresh(SendBuff,5,F1,F2,F3,F4,F5);
		UI_SendALL(SendBuff);
		
		UI_Delete(SendBuff,UI_Data_Del_Layer,0);
		UI_SendALL(SendBuff);
}

/*********************************************************************
 * @brief 画一个CSGO十字准星，可以根据状态来改变他的颜色
 * 
 * @param Color 
 * @note 可用形参选项
 * UI_Color_Main          //红蓝主色
 * UI_Color_Yellow 
 * UI_Color_Green 
 * UI_Color_Orange 
 * UI_Color_Purplish_red  //紫红色
 * UI_Color_Pink 
 * UI_Color_Cyan          //青色
 * UI_Color_Black 
 * UI_Color_White 
*********************************************************************/
Graph_Data G1,G2,G3,G4,G5,G6,G7,G8,G9,G10,G11;
void CSGO_Cross_AIMLINE(uint8_t Color)
{
		/* 给每个结构体赋值 */
		memset(&G1,0,sizeof(G1));
		memset(&G2,0,sizeof(G2));
		memset(&G3,0,sizeof(G3));
		memset(&G4,0,sizeof(G4));
		memset(&G5,0,sizeof(G5));
		memset(&G6,0,sizeof(G6));
		memset(&G7,0,sizeof(G7));
		memset(&G8,0,sizeof(G8));
		/* CSGO十字准星 */
		//存图像buff,标识，操作，图层，颜色，线宽，X,Y,X_E,Y_E
		Line_Draw(&G1,"091",UI_Graph_ADD,9,Color,2, 961,550,961,600);//上
		Line_Draw(&G2,"092",UI_Graph_ADD,9,Color,2, 961,530,961,480);//下
		Line_Draw(&G3,"093",UI_Graph_ADD,9,Color,2, 950,541,900,541);//左
		Line_Draw(&G4,"094",UI_Graph_ADD,9,Color,2, 970,541,1020,541);//右

		Line_Draw(&G5,"095",UI_Graph_ADD,9,Color,2, 960,550,960,600);//上偏移
		Line_Draw(&G6,"096",UI_Graph_ADD,9,Color,2, 960,530,960,480);//下偏移
		Line_Draw(&G7,"097",UI_Graph_ADD,9,Color,2, 950,540,900,540);//左偏移
		Line_Draw(&G8,"098",UI_Graph_ADD,9,Color,2, 970,540,1020,540);//右偏移
		/*打包进入TX_BUFF->UART8_TX*/
		UI_ReFresh(SendBuff,7,G1,G2,G3,G4,G5,G6,G7);
		uint16_t le1n=UI_Get_Send_Data_Len(SendBuff);
		UI_SendALL(SendBuff);
		
		UI_ReFresh(SendBuff,1,G8); 
		uint16_t le2n=UI_Get_Send_Data_Len(SendBuff);
		UI_SendALL(SendBuff);
}

//void FRIC_CHAR_UI()
//{
//		Char_Draw(&CH_FLRB,"075",UI_Graph_ADD,7 ,UI_Color_Yellow,24,4,4,960,240,flrb_arr);
////		Char_Draw(&CH_FLRB,"075",UI_Graph_ADD,7 ,UI_Color_Yellow,24,1,4,960,240,&flrb_arr[2]);
//		Char_ReFresh(SendBuff,CH_FLRB);
//		uint16_t le1n=UI_Get_Send_Data_Len(SendBuff);
//		UI_SendALL(SendBuff,le1n);
//}
//float VN =0.43;
//Graph_Data TEST_Float;
//void FRIC_Float_UI()
//{
//UI_Delete(SendBuff,UI_Data_Del_Layer,1);
//uint16_t le1n=UI_Get_Send_Data_Len(SendBuff);
//UI_SendALL(SendBuff,le1n);
//	HAL_Delay(1000);
//Float_Draw1(&TEST_Float,"199",UI_Graph_ADD,1,UI_Color_Pink,30,100,1,960,110,VN);
//UI_ReFresh(SendBuff,1,TEST_Float);
//uint16_t le2n=UI_Get_Send_Data_Len(SendBuff);
//UI_SendALL(SendBuff,le2n);
//VN=VN+0.1;
//}


