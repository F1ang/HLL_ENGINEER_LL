#include "can2_hll.h"

/*********************************************************************
 * @brief CAN初始化
 * 
 * @return int 
*********************************************************************/
int Can2_Init(void)//CAN初始化
{
	/*初始化PB12(CAN2_RX) PB13(CAN2_TX)*/
	RCC->AHB1ENR|=1<<1;//使能GPIO PORTB时钟 (1<<0代表GPIOA 1<<1代表GPIOB 1<<2代表GPIOC)

	GPIOB->MODER |=2<<26|2<<24;//Moder12是0x10 Moder13是0x10 /*我很好奇一个发送一个接受(即不用复用的模式)会怎么样?*/

	GPIOB->OTYPER &= 0xFFFF;//(OT12,OT13是0)1-16谁推挽谁填0, 16到32为空（填啥都行），默认是全部为0(这句话是一句废话)

	GPIOB->OSPEEDR |= 3<<26|3<<24;//PD0 -- PD1 速度100m

	SYSCFG->CMPCR = 0x00000001;//使用IO补偿单元(补偿单元上电)/*当GPIO速度超过50M的时候要考虑使用此设置(0位：0关闭补偿，1开启补偿)*/
	
	GPIOB->PUPDR |=  1<<26|1<<24;//PB12 PB13上拉输出
	
	/*官方寄存器封装时分为了0x20(AFRl)和0x24(AFRH)两个寄存器分开，但是在这一版底层寄存器指引的时候通过AFR[2]来实现两个寄存器的写入*/
	/*因为是12,13所以选择高位AFR[1],查询AF索引CAN1到can3对应的AF9所以这里要填0x1001*/ 
	GPIOB->AFR[1]|=9<<16;
	GPIOB->AFR[1]|=9<<20; 
	
	int i =0;
	RCC->APB1ENR|= 1<<26;//使能CAN2时钟(位26：1是打开can2时钟)(RCC APB1 外设时钟使能寄存器)
	CAN2->MCR=0x0000;	//退出睡眠模式(清除第一位睡眠位)
	CAN2->MCR|=1<<0;		//请求CAN进入初始化模式
	
	while((CAN2->MSR&1<<0)==0)   //CAN硬件退出初始化模式时，CAN-MSR(CAN主状态寄存器)位0 INAK由硬件清零
	{
		i++;
		if(i>100){return 2;}//进入初始化模式失败
	}

	//CAN主状态寄存器MCR
	CAN2->MCR|=0<<7;		//禁止时间触发通信模式
	CAN2->MCR|=0<<6;		//软件自动离线管理（退出总线关闭状态）
	CAN2->MCR|=0<<5;		//睡眠模式通过软件唤醒(清除CAN2->MCR的SLEEP位)
	CAN2->MCR|=1<<4;		//0：CAN 硬件将自动重发送消息，直到根据 CAN 标准消息发送成功1：无论发送结果如何（成功、错误或仲裁丢失），消息均只发送一次
	CAN2->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN2->MCR|=0<<2;		//0:优先级由报文标识符决定1:由请求的时间顺序决定

	//CAN位时序寄存器（bit timing register）
	CAN2->BTR=0x00000000;	//清除原来的设置.
	CAN2->BTR|=0<<30;	//模式设置： 0,普通模式;1,回环模式;
	CAN2->BTR|=0<<24; 	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN2->BTR|=1<<20; 	//Tbs2=tbs2+1个时间单位
	CAN2->BTR|=5<<16;	//Tbs1=tbs1+1个时间单位
	CAN2->BTR|=4<<0;  	//分频系数(Fdiv)为brp+1
							//波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)=45/(2+6+1)/5
	CAN2->MCR&=~(1<<0);		//请求CAN退出初始化模式
	while((CAN2->MSR&1<<0)==1)
	{
		i++;
		if(i>0xFFF0){return 3;}//退出初始化模式失败
	}	
	
	Can2_Filter();//打开过滤器
//使用中断接收    
#if CAN2_RX0_INT_ENABLE	
	CAN2->IER|=1<<1;		//FIFO0消息挂号中断允许.
	//分组4
  u32 temp;		   
	temp=6;	  //抢断优先级6
	temp|=0&(0x0f>>4);  //响应优先级0
	temp&=0xf;								//取低四位
	NVIC->ISER[CAN2_RX0_IRQn/32]|=1<<CAN2_RX0_IRQn%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[CAN2_RX0_IRQn]|=temp<<4;				//设置响应优先级和抢断优先级
#endif
	return 0;
}
void Can2_Filter(void)//CAN过滤器，无过滤
{
	//过滤器初始化
	CAN2->FMR|=1<<0;		//位0: 0，工作模式；1，过滤器组工作在初始化模式(CAN筛选器主计时器)
	CAN2->FA1R&=~(1<<12);//过滤器12不激活（CAN1的PD0为RX，CAN2的RX为PB12）？？抱有疑问
	CAN2->FS1R|=1<<12; 	//过滤器位宽为32位.
	CAN2->FM1R|=0<<12;		//过滤器12工作在标识符屏蔽位模式（1为列表模式）
	CAN2->FFA1R|=0<<0;		//过滤器0关联到FIFO0
	CAN2->sFilterRegister[0].FR1=0X00000000;//32位ID
	CAN2->sFilterRegister[0].FR2=0X00000000;//32位MASK
	CAN2->FA1R|=1<<0;		//激活过滤器0
	CAN2->FMR&=0<<0;		//过滤器组进入正常模式
}

/*********************************************************************
 * @brief 准备好CAN_TX使用的MAILBOX发送数据
 * 
 * @param Distinguish_ID 	CAN总线识别ID（对于M3508电机来说就是0x00000200和0x000001FF）
 * @param len  				数据长度(标准帧8位)
 * @param data 				发送数据
 * @return u8 邮箱号
*********************************************************************/
u8 Motor_Can2_Send(u32 Distinguish_ID,u8 len ,u8 *data)
{
	/*寻找空邮箱*/
	//CAN_TSR:CAN发送状态寄存器
	u8 FIFOBOX = 0;//哪个邮箱是空的可以存放FIFO
	if(CAN2->TSR&(1<<26))
	{
		FIFOBOX=0;
	}
	else if(CAN2->TSR&(1<<27))
	{
		FIFOBOX=1;
	}
	else if(CAN2->TSR&(1<<28))
	{
		FIFOBOX=2;
	}
	else return 0XFF;					//无空邮箱,无法发送
	/*打开对应邮箱,stm32F4xx.h文件中有对应的结构体*/
	/*CAN_TxMailBox_TypeDef　sTxMailBox[3]底层文件中通过数组依次选择MAILBOX0或者MAILBOX1或者MAILBOX2*/
	CAN2->sTxMailBox[FIFOBOX].TIR=0;		//清除之前的设置
		
	Distinguish_ID&=0x7ff;						//取低11位stdid
	Distinguish_ID<<=21;							//转换写入寄存器的识别ID(标准帧ID)
	
	CAN2->sTxMailBox[FIFOBOX].TIR|=Distinguish_ID;	//写入识别ID	 
	CAN2->sTxMailBox[FIFOBOX].TIR|=0<<2;						//0:标准帧　1:扩展帧	  
	CAN2->sTxMailBox[FIFOBOX].TIR|=0<<1;					//0:数据帧  1:远程帧
	len&=0X0F;//得到低四位
	CAN2->sTxMailBox[FIFOBOX].TDTR&=~(0X0000000F);
	CAN2->sTxMailBox[FIFOBOX].TDTR|=len;		   //设置DLC.
	
	/*待发送数据存入邮箱（写入寄存器）*/
	CAN2->sTxMailBox[FIFOBOX].TDHR=(((u32)data[7]<<24)|
								((u32)data[6]<<16)|
 								((u32)data[5]<<8)|
								((u32)data[4]));
	CAN2->sTxMailBox[FIFOBOX].TDLR=(((u32)data[3]<<24)|
								((u32)data[2]<<16)|
 								((u32)data[1]<<8)|
								((u32)data[0]));
	CAN2->sTxMailBox[FIFOBOX].TIR|=1<<0; //请求发送邮箱数据
	return FIFOBOX;
}

/*********************************************************************
 * @brief 获得发送状态,是否发送完成
 * 
 * @param FIFOBOX （判断是邮箱０邮箱１邮箱２中的一个）
 * @return 0x00,挂起;0X05,发送失败;0X07,发送成功. 
*********************************************************************/
u8 CAN2_Tx_Staus(u8 FIFOBOX)
{	
	u8 sta=0;					    
	switch (FIFOBOX)
	{
		case 0: 
			sta |= CAN2->TSR&(1<<0);			//RQCP0
			sta |= CAN2->TSR&(1<<1);			//TXOK0
			sta |=((CAN2->TSR&(1<<26))>>24);	//TME0
			break;
		case 1: 
			sta |= CAN2->TSR&(1<<8)>>8;		//RQCP1
			sta |= CAN2->TSR&(1<<9)>>8;		//TXOK1
			sta |=((CAN2->TSR&(1<<27))>>25);	//TME1	   
			break;
		case 2: 
			sta |= CAN2->TSR&(1<<16)>>16;	//RQCP2
			sta |= CAN2->TSR&(1<<17)>>16;	//TXOK2
			sta |=((CAN2->TSR&(1<<28))>>26);	//TME2
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
 * @param data 电流值数组
 * @return u8 1发送成功 0发送失败
*********************************************************************/
u8 MotorM3508_Send_Ampere(s16 *data)
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
  FIFOBOX=Motor_Can2_Send(0x00000200,8,Cur_Data);//发送数据并且获取是哪个邮箱发送的，写入FIFOBOX中
	while((CAN2_Tx_Staus(FIFOBOX)!=0X07)&&(i<0XFFF))i++;//等待发送结束
	if(i>=0XFFF)return 1;							//发送失败?
	return 0;										//发送成功;
}



/*********************************************************************
 * @brief 	接受具体的FIFO邮箱数据（未封装）
 * 
 * @param fifox 	邮箱号
 * @param id 		标准ID(11位)/扩展ID(11位+18位)
 * @param ide 		0,标准帧;1,扩展帧
 * @param rtr 		0,数据帧;1,远程帧
 * @param len 		接收到的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
 * @param dat 		数据缓存区，8个元素为电机状态
*********************************************************************/
void CAN2_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat)
{	   
	*ide=CAN2->sFIFOMailBox[fifox].RIR&0x04;//第2位IDE：标识符扩展此位定义邮箱中邮件的标识符类型0：标准标识符1：扩展标识符位1 RTR：远程传输请求0：数据帧1：远程帧
	
	*id=CAN2->sFIFOMailBox[fifox].RIR>>21;//标准标识符

	*rtr=CAN2->sFIFOMailBox[fifox].RIR&0x02;	//得到远程发送请求值.
	*len=CAN2->sFIFOMailBox[fifox].RDTR&0x0F;//得到DLC

	//接收数据(抄的没有仔细研究)
	dat[0]=CAN2->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN2->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN2->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN2->sFIFOMailBox[fifox].RDLR>>24)&0XFF;

	dat[4]=CAN2->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN2->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN2->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN2->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN2->RF0R|=0X20;//释放FIFO0邮箱
	else if(fifox==1)CAN2->RF1R|=0X20;//释放FIFO1邮箱	 
}

//得到在FIFO0/FIFO1中接收到的报文个数.
//fifox:0/1.FIFO编号;
//返回值:FIFO0/FIFO1中的报文个数.
u8 CAN2_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN2->RF0R&0x03; 
	else if(fifox==1)return CAN2->RF1R&0x03; 
	else return 0;
}

/*********************************************************************
* @ 函数名 ： 	Query_Accept_Data		
* @ 功能说明：查询接受数据(已封装)
* @ 参数 ：	buf 		暂存数据
* @ 返回值 ： 0:未接受数据或者数据一场 其他:电调ID
*********************************************************************/
u8 Query_Accept_Data2(u8 *buf)
{
	u32 ID;
	u8 ide,rtr,len; 
	if(CAN2_Msg_Pend(0)==0&&CAN2_Msg_Pend(1)==0){/*printf("没有数据接受");*/return 0;}		//没有接收到数据,直接退出 	 
  	CAN2_Rx_Msg(0,&ID,&ide,&rtr,&len,buf); 	//读取数据，将暂存数据放置在buf中
    if(ide!=0||rtr!=0){/*printf("数据异常")*/;return 0;}		//如果不是标准帧;不是数据帧;接受异常，直接退出; 
	
	return ID;	//返回电调ID，区分这是是谁的数据
}
/*********************************************************************
* @ 函数名 ： 	Printf_CAN2_Data		
* @ 功能说明：解析CAN2反馈报文
* @ 参数 ：	CAN2_Motor 		can.h查看结构体文件
* @ 返回值 ：无
*********************************************************************/		


