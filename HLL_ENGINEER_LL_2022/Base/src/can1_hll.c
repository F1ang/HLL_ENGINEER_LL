#include "can1_hll.h"

/*********************************************************************
* @ 函数名 ： Can1_Init
* @ 功能说明： 初始化CAN
* @ 参数 ：	无
* @ 返回值 ： 1正常,2未进入初始化
*********************************************************************/
int Can1_Init(void)//CAN初始化
{
	/*初始化PD0(CAN1_RX) PD1(CAN1_TX)*/
	RCC->AHB1ENR|=1<<3;//使能GPIO PORTD时钟 (1<<0代表GPIOA 1<<1代表GPIOD 1<<2代表GPIOC)

	GPIOD->MODER |=2<<0|2<<2;//　Moder0是0x10 Moder1是0x10 /*我很好奇一个发送一个接受(即不用复用的模式)会怎么样?*/

	GPIOD->OTYPER &= 0xFFFFFFFC;//1-16谁推挽谁填0, 16到32为空（填啥都行），默认是全部为0(这句话是一句废话)

	GPIOD->OSPEEDR |= 3<<0|3<<2;//PD0 -- PD1 速度100m

	SYSCFG->CMPCR = 0x00000001;//使用IO补偿单元/*当GPIO速度超过50M的时候要考虑使用此设置(0位：0关闭补偿，1开启补偿)*/
	
	GPIOD->PUPDR |=  1<<0|1<<2;//PD0 PD1上拉输出
	
	/*官方寄存器封装时分为了0x20(AFRl)和0x24(AFRH)两个寄存器分开，但是在这一版底层寄存器指引的时候通过AFR[2]来实现两个寄存器的写入*/
	/*因为是01所以选择低位AFR[0],查询AF索引can1到can3对应的AF9所以这里要填0x1001*/ 
	GPIOD->AFR[0]|=9<<0;
	GPIOD->AFR[0]|=9<<4; 
	
	int i =0;
	RCC->APB1ENR|= 1<<25;//使能CAN1时钟(1是打开)
	CAN1->MCR=0x0000;	//退出睡眠模式(清除第一位睡眠位)
	CAN1->MCR|=1<<0;		//请求CAN进入初始化模式
	
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100){return 2;}//进入初始化模式失败
	}
	CAN1->MCR|=0<<7;		//非时间触发通信模式
	CAN1->MCR|=0<<6;		//软件自动离线管理
	CAN1->MCR|=0<<5;		//睡眠模式通过软件唤醒(清除CAN1->MCR的SLEEP位)
	CAN1->MCR|=1<<4;		//0：CAN 硬件将自动重发送消息，直到根据 CAN 标准消息发送成功1：无论发送结果如何（成功、错误或仲裁丢失），消息均只发送一次
	CAN1->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN1->MCR|=0<<2;		//0:优先级由报文标识符决定1:由请求的时间顺序决定
	CAN1->BTR=0x00000000;	//清除原来的设置.
	CAN1->BTR|=0<<30;	//模式设置 0,普通模式;1,回环模式;
	CAN1->BTR|=0<<24; 	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN1->BTR|=1<<20; 	//Tbs2=tbs2+1个时间单位
	CAN1->BTR|=5<<16;	//Tbs1=tbs1+1个时间单位
	CAN1->BTR|=4<<0;  	//分频系数(Fdiv)为brp+1
							//波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)=45/(2+6+1)/5
	CAN1->MCR&=~(1<<0);		//请求CAN退出初始化模式
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0xFFF0){return 3;}//退出初始化模式失败
	}	
	
	Can1_Filter();//打开过滤器
//使用中断接收
#if CAN1_RX0_INT_ENABLE	
	CAN1->IER|=1<<1;		//FIFO0消息挂号中断允许.
	//分组4
  u32 temp;		   
	temp=6;	  //抢断优先级6
	temp|=0&(0x0f>>4);  //响应优先级0
	temp&=0xf;								//取低四位
	NVIC->ISER[CAN1_RX0_IRQn/32]|=1<<CAN1_RX0_IRQn%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[CAN1_RX0_IRQn]|=temp<<4;				//设置响应优先级和抢断优先级
#endif
	return 0;
}
void Can1_Filter(void)//CAN过滤器，无过滤
{
	//过滤器初始化
	CAN1->FMR|=1<<0;		//过滤器组工作在初始化模式
	CAN1->FA1R&=~(1<<0);//过滤器0不激活
	CAN1->FS1R|=1<<0; 	//过滤器位宽为32位.
	CAN1->FM1R|=0<<0;		//过滤器0工作在标识符屏蔽位模式
	CAN1->FFA1R|=0<<0;		//过滤器0关联到FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32位ID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32位MASK
	CAN1->FA1R|=1<<0;		//激活过滤器0
	CAN1->FMR&=0<<0;		//过滤器组进入正常模式
}
/*********************************************************************
* @ 函数名 ： Motor_Can_Send
* @ 功能说明： 准备好CAN_TX使用的MAILBOX发送数据
* @ 参数 ：	Distinguish_ID	CAN总线识别ID（对于M3508电机来说就是0x00000200和0x000001FF）
*						len							数据长度(标准帧8位)
*						data 						发送数据
* @ 返回值 ： 无
*********************************************************************/

u8 Motor_Can_Send(u32 Distinguish_ID,u8 len ,u8 *data)
{
	/*寻找空邮箱*/
	u8 FIFOBOX = 0;//哪个邮箱是空的可以存放FIFO
	if(CAN1->TSR&(1<<26))
	{
		FIFOBOX=0;
	}
	else if(CAN1->TSR&(1<<27))
	{
		FIFOBOX=1;
	}
	else if(CAN1->TSR&(1<<28))
	{
		FIFOBOX=2;
	}
	else return 0XFF;					//无空邮箱,无法发送
	/*打开对应邮箱,stm32F4xx.h文件中有对应的结构体*/
	/*CAN_TxMailBox_TypeDef　sTxMailBox[3]底层文件中通过数组依次选择MAILBOX0或者MAILBOX1或者MAILBOX2*/
	CAN1->sTxMailBox[FIFOBOX].TIR=0;		//清除之前的设置
		
	Distinguish_ID&=0x7ff;						//取低11位stdid
	Distinguish_ID<<=21;							//转换写入寄存器的识别ID(标准帧ID)
	
	CAN1->sTxMailBox[FIFOBOX].TIR|=Distinguish_ID;	//写入识别ID	 
	CAN1->sTxMailBox[FIFOBOX].TIR|=0<<2;						//0:标准帧　1:扩展帧	  
	CAN1->sTxMailBox[FIFOBOX].TIR|=0<<1;					//0:数据帧  1:远程帧
	len&=0X0F;//得到低四位
	CAN1->sTxMailBox[FIFOBOX].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[FIFOBOX].TDTR|=len;		   //设置DLC.
	
	/*待发送数据存入邮箱（写入寄存器）*/
	CAN1->sTxMailBox[FIFOBOX].TDHR=(((u32)data[7]<<24)|
								((u32)data[6]<<16)|
 								((u32)data[5]<<8)|
								((u32)data[4]));
	CAN1->sTxMailBox[FIFOBOX].TDLR=(((u32)data[3]<<24)|
								((u32)data[2]<<16)|
 								((u32)data[1]<<8)|
								((u32)data[0]));
	CAN1->sTxMailBox[FIFOBOX].TIR|=1<<0; //请求发送邮箱数据
	return FIFOBOX;
}
/*********************************************************************
* @ 函数名 ： 	CAN1_Tx_Staus	　
* @ 功能说明：	获得发送状态,是否发送完成
* @ 参数 ：	FIFOBOX　（判断是邮箱０邮箱１邮箱２中的一个）
* @ 返回值 ：0x00,挂起;0X05,发送失败;0X07,发送成功.
*********************************************************************/
u8 CAN1_Tx_Staus(u8 FIFOBOX)
{	
	u8 sta=0;					    
	switch (FIFOBOX)
	{
		case 0: 
			sta |= CAN1->TSR&(1<<0);			//RQCP0
			sta |= CAN1->TSR&(1<<1);			//TXOK0
			sta |=((CAN1->TSR&(1<<26))>>24);	//TME0
			break;
		case 1: 
			sta |= CAN1->TSR&(1<<8)>>8;		//RQCP1
			sta |= CAN1->TSR&(1<<9)>>8;		//TXOK1
			sta |=((CAN1->TSR&(1<<27))>>25);	//TME1	   
			break;
		case 2: 
			sta |= CAN1->TSR&(1<<16)>>16;	//RQCP2
			sta |= CAN1->TSR&(1<<17)>>16;	//TXOK2
			sta |=((CAN1->TSR&(1<<28))>>26);	//TME2
			break;
		default:
			sta=0X05;//邮箱号不对,肯定失败.
		break;
	}
	return sta;
} 

/*********************************************************************
 * @brief 给予M3508电机电流
 * 
 * @param all_id 电调id（0x200电调1234 0x199电调5678）
 * @param data 电流值数组
 * @return u8 1发送成功 0发送失败
*********************************************************************/
u8 Can1_Send_4Msg(u32 all_id,s16 *data)  
{
	u8 FIFOBOX;
	u8 Cur_Data[8];
	u16 i=0;	
  Cur_Data[0]=CEBH(data[0]);
	Cur_Data[1]=CEBL(data[0]);
	Cur_Data[2]=CEBH(data[1]);
	Cur_Data[3]=CEBL(data[1]);
	Cur_Data[4]=CEBH(data[2]);
	Cur_Data[5]=CEBL(data[2]);
	Cur_Data[6]=CEBH(data[3]);
	Cur_Data[7]=CEBL(data[3]);
  FIFOBOX=Motor_Can_Send(all_id,8,Cur_Data);//发送数据并且获取是哪个邮箱发送的，写入FIFOBOX中
	while((CAN1_Tx_Staus(FIFOBOX)!=0X07)&&(i<0XFFF))i++;//等待发送结束
	if(i>=0XFFF)return 1;							//发送失败?
	return 0;										//发送成功;
}

/*********************************************************************
* @ 函数名 ： 	CAN1_Rx_Msg		
* @ 功能说明：接受具体的FIFO邮箱数据（未封装）
* @ 参数 ：	data 				接收数据
*						fifox				邮箱号
*						id 					标准ID(11位)/扩展ID(11位+18位)	
*						id:标准ID(11位)/扩展ID(11位+18位)	    
*						ide:0,标准帧;1,扩展帧
*						rtr:0,数据帧;1,远程帧
*						len:接收到的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
*						dat:数据缓存区
* @ 返回值 ： 无
*********************************************************************/
void CAN1_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat)
{	   
	*ide=CAN1->sFIFOMailBox[fifox].RIR&0x04;//第2位IDE：标识符扩展此位定义邮箱中邮件的标识符类型0：标准标识符1：扩展标识符位1 RTR：远程传输请求0：数据帧1：远程帧
	
//	*id=CAN1->sFIFOMailBox[fifox].RIR>>21;//标准标识符
	*id=(uint32_t)0x00000FF&(CAN1->sFIFOMailBox[fifox].RIR>>21);//标准标识符

	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;	//得到远程发送请求值.
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;//得到DLC

	//接收数据(抄的没有仔细研究)
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN1->RF0R|=0X20;//释放FIFO0邮箱
	else if(fifox==1)CAN1->RF1R|=0X20;//释放FIFO1邮箱	 
}

//得到在FIFO0/FIFO1中接收到的报文个数.
//fifox:0/1.FIFO编号;
//返回值:FIFO0/FIFO1中的报文个数.
u8 CAN1_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}





