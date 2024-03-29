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

/*变量*/
Graph_Data G1,G2,G3,G4,G5,G6,G7,G8,G9,G10,G11,G12,G13,G14,G15,G16,G17,G18, G19,G20,G21,G22,G23,G24,G25,G26,G27,G28 ,G29,G30;

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
		/* 给十字结构体赋值, */
		memset(&G1,0,sizeof(G1));memset(&G2,0,sizeof(G2));memset(&G3,0,sizeof(G3));
		memset(&G4,0,sizeof(G4));memset(&G5,0,sizeof(G5));memset(&G6,0,sizeof(G6));
		memset(&G7,0,sizeof(G7));memset(&G8,0,sizeof(G8));memset(&G9,0,sizeof(G9));
	
		memset(&G10,0,sizeof(G10));memset(&G11,0,sizeof(G11));memset(&G12,0,sizeof(G12));
		memset(&G13,0,sizeof(G13));memset(&G14,0,sizeof(G14));memset(&G15,0,sizeof(G15));
		memset(&G16,0,sizeof(G16));memset(&G17,0,sizeof(G17));memset(&G18,0,sizeof(G18));
	
		memset(&G19,0,sizeof(G19));memset(&G20,0,sizeof(G20));memset(&G21,0,sizeof(G21));
		memset(&G22,0,sizeof(G22));memset(&G23,0,sizeof(G23));memset(&G24,0,sizeof(G24));
		memset(&G25,0,sizeof(G25));memset(&G26,0,sizeof(G26));memset(&G27,0,sizeof(G27));
		memset(&G28,0,sizeof(G28));memset(&G29,0,sizeof(G29));memset(&G30,0,sizeof(G30));
	
		/* 设置机器人ID */
		UI_Set_Comparable_Id(UI_Data_CilentID_BEngineer);

		/* 设置静态UI--直线 */
		//存图像buff,标识，操作，图层，颜色，线宽，X,Y,X_E,Y_E
		Line_Draw(&line_t,"001",UI_Graph_ADD,8,UI_Color_Main,20,961,480,961,600);
		UI_ReFresh(SendBuff,1,line_t);
		UI_SendALL(SendBuff);

		/* 设置静态UI--矩形 */
		//存图像buff,标识，操作，图层，颜色，线宽，X,Y,X_E,Y_E
		Rectangle_Draw(&rectangle_t,"002",UI_Graph_ADD,8,UI_Color_Main,5,900,420,1020,600);
		UI_ReFresh(SendBuff,1,rectangle_t);
		UI_SendALL(SendBuff);

		/* 设置静态UI--整圆 */	
		//存图像buff,标识，操作，图层，颜色，线宽，圆心(X，Y)，R
		Circle_Draw(&circle_t,"003",UI_Graph_ADD,8,UI_Color_Main,5,961,540,150);
		UI_ReFresh(SendBuff,1,circle_t);
		UI_SendALL(SendBuff);

		/* 设置静态UI--圆弧 */
		//存图像buff,标识，操作，图层，颜色，开始角度，终止角度，线宽，圆心(X,Y)，X轴长，Y轴长
		Arc_Draw(&arc_t,"004",UI_Graph_ADD,8,UI_Color_Main,20,-20,20,961,540,50,50);
		UI_ReFresh(SendBuff,1,arc_t);
		UI_SendALL(SendBuff);

		/* 设置静态字符--HLL队名 */
		//存图像buff,标识，操作，图层，颜色，字号，字符个数，线宽，X，Y，字符串
		Char_Draw(&Char_t,"005",UI_Graph_ADD,8,UI_Color_Yellow,48,3,4,960,540,HLL_arr);
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
uint8_t F1_L,F2_L,F3_L,F4_L,F5_L,F6_L,F7_L;
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
		Char_Draw(&Char_t,"006",UI_Graph_ADD,9,UI_Color_Yellow,16,2,2,1400,800,"OV");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		//存图像buff，标识，图层操作，图层，颜色，字体，小数位数，线宽，开始坐标(x,y)，要显示的变量
		Float_Draw(&F1,"007",UI_Graph_ADD,0,UI_Color_Main,16,0,2,1470,800,robot_mode.mode_overturn);
		
		
		Char_Draw(&Char_t,"008",UI_Graph_ADD,9,UI_Color_Yellow,16,2,2,1400,770,"UP");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		Float_Draw(&F2,"009",UI_Graph_ADD,0,UI_Color_Main,16,0,2,1470,770,robot_mode.mode_up);
		
		
		Char_Draw(&Char_t,"010",UI_Graph_ADD,9,UI_Color_Yellow,16,3,2,1400,740,"RES");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		Float_Draw(&F3,"011",UI_Graph_ADD,0,UI_Color_Main,16,0,2,1470,740,robot_mode.mode_rescue);
		
		
		Char_Draw(&Char_t,"012",UI_Graph_ADD,9,UI_Color_Yellow,16,3,2,1400,710,"REV");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		Float_Draw(&F4,"013",UI_Graph_ADD,0,UI_Color_Main,16,0,2,1470,710,robot_mode.mode_revive);
			
//		/*HP*/
//		Char_Draw(&Char_t,"014",UI_Graph_ADD,9,UI_Color_Main,16,2,4,1400,620,"HP");
//		Char_ReFresh(SendBuff,Char_t);
//		UI_SendALL(SendBuff);
//		Float_Draw(&F5,"015",UI_Graph_ADD,0,UI_Color_Main,12,0,2,1470,620,judge_data->ext_game_robot_HP_t.blue_1_robot_HP);
		
		Char_Draw(&Char_t,"014",UI_Graph_ADD,9,UI_Color_Main,16,4,2,1400,680,"chip");
		Char_ReFresh(SendBuff,Char_t);
		UI_SendALL(SendBuff);
		Float_Draw(&F5,"015",UI_Graph_ADD,0,UI_Color_Main,16,0,2,1470,680,robot_mode.mode_chip);

//		Char_Draw(&Char_t,"016",UI_Graph_ADD,9,UI_Color_Yellow,16,3,2,1400,680,"OUP");
//		Char_ReFresh(SendBuff,Char_t);
//		UI_SendALL(SendBuff);
//		Float_Draw(&F6,"017",UI_Graph_ADD,0,UI_Color_Main,16,0,2,1470,680,robot_mode.mode_open_up);
//		
//		Char_Draw(&Char_t,"018",UI_Graph_ADD,9,UI_Color_Yellow,16,3,2,1400,650,"OST");
//		Char_ReFresh(SendBuff,Char_t);
//		UI_SendALL(SendBuff);
//		Float_Draw(&F7,"019",UI_Graph_ADD,0,UI_Color_Main,16,0,2,1470,650,robot_mode.mode_open_stretch);
			
//		if(F1_L==robot_mode.mode_overturn&&F2_L==robot_mode.mode_up&&F3_L==robot_mode.mode_rescue&&\
//			F4_L==robot_mode.mode_revive&&F6_L==robot_mode.mode_open_up&&F7_L==robot_mode.mode_open_stretch)
//		{

			UI_ReFresh(SendBuff,5,F1,F2,F3,F4,F5);
			UI_SendALL(SendBuff);
			
//		}
//		else
//		{
//			UI_Delete(SendBuff,UI_Data_Del_Layer,0);
//		  UI_SendALL(SendBuff);
//		}
		
		F1_L=robot_mode.mode_overturn;
		F2_L=robot_mode.mode_up;
		F3_L=robot_mode.mode_rescue;
		F4_L=robot_mode.mode_revive;
		F6_L=robot_mode.mode_open_up;
		F7_L=robot_mode.mode_open_stretch;
		
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
void CSGO_Cross_AIMLINE(uint8_t Color)
{
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
	
		Line_Draw(&G9,"099",UI_Graph_ADD,9,Color,2, 895,541,845,541);//左1
		Line_Draw(&G10,"100",UI_Graph_ADD,9,Color,2, 840,541,790,541);//左2
		Line_Draw(&G11,"101",UI_Graph_ADD,9,Color,2, 785,541,735,541);//左3
		Line_Draw(&G12,"102",UI_Graph_ADD,9,Color,2, 730,541,680,541);//左4
		Line_Draw(&G13,"103",UI_Graph_ADD,9,Color,2, 675,541,625,541);//左5
		
		Line_Draw(&G14,"104",UI_Graph_ADD,9,Color,2, 620,541,570,541);//左6
		Line_Draw(&G15,"105",UI_Graph_ADD,9,Color,2, 565,541,515,541);//左7
		Line_Draw(&G16,"106",UI_Graph_ADD,9,Color,2, 510,541,460,541);//左8
		Line_Draw(&G17,"107",UI_Graph_ADD,9,Color,2, 455,541,405,541);//左9
		Line_Draw(&G18,"108",UI_Graph_ADD,9,Color,2, 400,541,350,541);//左10
		
		Line_Draw(&G19,"109",UI_Graph_ADD,9,Color,2, 1025,541,1080,541);//右1
		Line_Draw(&G20,"110",UI_Graph_ADD,9,Color,2, 1085,541,1135,541);//右2
		Line_Draw(&G21,"111",UI_Graph_ADD,9,Color,2, 1140,541,1190,541);//右3
		Line_Draw(&G22,"112",UI_Graph_ADD,9,Color,2, 1195,541,1245,541);//右4
		Line_Draw(&G23,"113",UI_Graph_ADD,9,Color,2, 1250,541,1300,541);//右5
		
		Line_Draw(&G24,"114",UI_Graph_ADD,9,Color,2, 1305,541,1360,541);//右6
		Line_Draw(&G25,"115",UI_Graph_ADD,9,Color,2, 1365,541,1415,541);//右7
		Line_Draw(&G26,"116",UI_Graph_ADD,9,Color,2, 1420,541,1470,541);//右8
		Line_Draw(&G27,"117",UI_Graph_ADD,9,Color,2, 1475,541,1525,541);//右9
		Line_Draw(&G28,"118",UI_Graph_ADD,9,Color,2, 1530,541,1580,541);//右10
		/*打包进入TX_BUFF->UART8_TX*/
		UI_ReFresh(SendBuff,7,G1,G2,G3,G4,G5,G6,G7);
		uint16_t le1n=UI_Get_Send_Data_Len(SendBuff);
		UI_SendALL(SendBuff);
		
		UI_ReFresh(SendBuff,7,G8,G9,G10,G11,G12,G13,G14); 
		uint16_t le2n=UI_Get_Send_Data_Len(SendBuff);
		UI_SendALL(SendBuff);
		
		UI_ReFresh(SendBuff,7,G15,G16,G17,G18,G19,G20,G21); 
		uint16_t le3n=UI_Get_Send_Data_Len(SendBuff);
		UI_SendALL(SendBuff);
		
		UI_ReFresh(SendBuff,7,G22,G23,G24,G25,G26,G27,G28); 
		uint16_t le4n=UI_Get_Send_Data_Len(SendBuff);
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


