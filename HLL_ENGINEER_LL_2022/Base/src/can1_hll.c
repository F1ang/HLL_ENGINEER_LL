#include "can1_hll.h"

/*********************************************************************
* @ ������ �� Can1_Init
* @ ����˵���� ��ʼ��CAN
* @ ���� ��	��
* @ ����ֵ �� 1����,2δ�����ʼ��
*********************************************************************/
int Can1_Init(void)//CAN��ʼ��
{
	/*��ʼ��PD0(CAN1_RX) PD1(CAN1_TX)*/
	RCC->AHB1ENR|=1<<3;//ʹ��GPIO PORTDʱ�� (1<<0����GPIOA 1<<1����GPIOD 1<<2����GPIOC)

	GPIOD->MODER |=2<<0|2<<2;//��Moder0��0x10 Moder1��0x10 /*�Һܺ���һ������һ������(�����ø��õ�ģʽ)����ô��?*/

	GPIOD->OTYPER &= 0xFFFFFFFC;//1-16˭����˭��0, 16��32Ϊ�գ���ɶ���У���Ĭ����ȫ��Ϊ0(��仰��һ��ϻ�)

	GPIOD->OSPEEDR |= 3<<0|3<<2;//PD0 -- PD1 �ٶ�100m

	SYSCFG->CMPCR = 0x00000001;//ʹ��IO������Ԫ/*��GPIO�ٶȳ���50M��ʱ��Ҫ����ʹ�ô�����(0λ��0�رղ�����1��������)*/
	
	GPIOD->PUPDR |=  1<<0|1<<2;//PD0 PD1�������
	
	/*�ٷ��Ĵ�����װʱ��Ϊ��0x20(AFRl)��0x24(AFRH)�����Ĵ����ֿ�����������һ��ײ�Ĵ���ָ����ʱ��ͨ��AFR[2]��ʵ�������Ĵ�����д��*/
	/*��Ϊ��01����ѡ���λAFR[0],��ѯAF����can1��can3��Ӧ��AF9��������Ҫ��0x1001*/ 
	GPIOD->AFR[0]|=9<<0;
	GPIOD->AFR[0]|=9<<4; 
	
	int i =0;
	RCC->APB1ENR|= 1<<25;//ʹ��CAN1ʱ��(1�Ǵ�)
	CAN1->MCR=0x0000;	//�˳�˯��ģʽ(�����һλ˯��λ)
	CAN1->MCR|=1<<0;		//����CAN�����ʼ��ģʽ
	
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100){return 2;}//�����ʼ��ģʽʧ��
	}
	CAN1->MCR|=0<<7;		//��ʱ�䴥��ͨ��ģʽ
	CAN1->MCR|=0<<6;		//����Զ����߹���
	CAN1->MCR|=0<<5;		//˯��ģʽͨ���������(���CAN1->MCR��SLEEPλ)
	CAN1->MCR|=1<<4;		//0��CAN Ӳ�����Զ��ط�����Ϣ��ֱ������ CAN ��׼��Ϣ���ͳɹ�1�����۷��ͽ����Σ��ɹ���������ٲö�ʧ������Ϣ��ֻ����һ��
	CAN1->MCR|=0<<3;		//���Ĳ�����,�µĸ��Ǿɵ�
	CAN1->MCR|=0<<2;		//0:���ȼ��ɱ��ı�ʶ������1:�������ʱ��˳�����
	CAN1->BTR=0x00000000;	//���ԭ��������.
	CAN1->BTR|=0<<30;	//ģʽ���� 0,��ͨģʽ;1,�ػ�ģʽ;
	CAN1->BTR|=0<<24; 	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ
	CAN1->BTR|=1<<20; 	//Tbs2=tbs2+1��ʱ�䵥λ
	CAN1->BTR|=5<<16;	//Tbs1=tbs1+1��ʱ�䵥λ
	CAN1->BTR|=4<<0;  	//��Ƶϵ��(Fdiv)Ϊbrp+1
							//������:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)=45/(2+6+1)/5
	CAN1->MCR&=~(1<<0);		//����CAN�˳���ʼ��ģʽ
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0xFFF0){return 3;}//�˳���ʼ��ģʽʧ��
	}	
	
	Can1_Filter();//�򿪹�����
//ʹ���жϽ���
#if CAN1_RX0_INT_ENABLE	
	CAN1->IER|=1<<1;		//FIFO0��Ϣ�Һ��ж�����.
	//����4
  u32 temp;		   
	temp=6;	  //�������ȼ�6
	temp|=0&(0x0f>>4);  //��Ӧ���ȼ�0
	temp&=0xf;								//ȡ����λ
	NVIC->ISER[CAN1_RX0_IRQn/32]|=1<<CAN1_RX0_IRQn%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
	NVIC->IP[CAN1_RX0_IRQn]|=temp<<4;				//������Ӧ���ȼ����������ȼ�
#endif
	return 0;
}
void Can1_Filter(void)//CAN���������޹���
{
	//��������ʼ��
	CAN1->FMR|=1<<0;		//�������鹤���ڳ�ʼ��ģʽ
	CAN1->FA1R&=~(1<<0);//������0������
	CAN1->FS1R|=1<<0; 	//������λ��Ϊ32λ.
	CAN1->FM1R|=0<<0;		//������0�����ڱ�ʶ������λģʽ
	CAN1->FFA1R|=0<<0;		//������0������FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32λID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32λMASK
	CAN1->FA1R|=1<<0;		//���������0
	CAN1->FMR&=0<<0;		//���������������ģʽ
}
/*********************************************************************
* @ ������ �� Motor_Can_Send
* @ ����˵���� ׼����CAN_TXʹ�õ�MAILBOX��������
* @ ���� ��	Distinguish_ID	CAN����ʶ��ID������M3508�����˵����0x00000200��0x000001FF��
*						len							���ݳ���(��׼֡8λ)
*						data 						��������
* @ ����ֵ �� ��
*********************************************************************/

u8 Motor_Can_Send(u32 Distinguish_ID,u8 len ,u8 *data)
{
	/*Ѱ�ҿ�����*/
	u8 FIFOBOX = 0;//�ĸ������ǿյĿ��Դ��FIFO
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
	else return 0XFF;					//�޿�����,�޷�����
	/*�򿪶�Ӧ����,stm32F4xx.h�ļ����ж�Ӧ�Ľṹ��*/
	/*CAN_TxMailBox_TypeDef��sTxMailBox[3]�ײ��ļ���ͨ����������ѡ��MAILBOX0����MAILBOX1����MAILBOX2*/
	CAN1->sTxMailBox[FIFOBOX].TIR=0;		//���֮ǰ������
		
	Distinguish_ID&=0x7ff;						//ȡ��11λstdid
	Distinguish_ID<<=21;							//ת��д��Ĵ�����ʶ��ID(��׼֡ID)
	
	CAN1->sTxMailBox[FIFOBOX].TIR|=Distinguish_ID;	//д��ʶ��ID	 
	CAN1->sTxMailBox[FIFOBOX].TIR|=0<<2;						//0:��׼֡��1:��չ֡	  
	CAN1->sTxMailBox[FIFOBOX].TIR|=0<<1;					//0:����֡  1:Զ��֡
	len&=0X0F;//�õ�����λ
	CAN1->sTxMailBox[FIFOBOX].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[FIFOBOX].TDTR|=len;		   //����DLC.
	
	/*���������ݴ������䣨д��Ĵ�����*/
	CAN1->sTxMailBox[FIFOBOX].TDHR=(((u32)data[7]<<24)|
								((u32)data[6]<<16)|
 								((u32)data[5]<<8)|
								((u32)data[4]));
	CAN1->sTxMailBox[FIFOBOX].TDLR=(((u32)data[3]<<24)|
								((u32)data[2]<<16)|
 								((u32)data[1]<<8)|
								((u32)data[0]));
	CAN1->sTxMailBox[FIFOBOX].TIR|=1<<0; //��������������
	return FIFOBOX;
}
/*********************************************************************
* @ ������ �� 	CAN1_Tx_Staus	��
* @ ����˵����	��÷���״̬,�Ƿ������
* @ ���� ��	FIFOBOX�����ж������䣰���䣱���䣲�е�һ����
* @ ����ֵ ��0x00,����;0X05,����ʧ��;0X07,���ͳɹ�.
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
			sta=0X05;//����Ų���,�϶�ʧ��.
		break;
	}
	return sta;
} 

/*********************************************************************
 * @brief ����M3508�������
 * 
 * @param all_id ���id��0x200���1234 0x199���5678��
 * @param data ����ֵ����
 * @return u8 1���ͳɹ� 0����ʧ��
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
  FIFOBOX=Motor_Can_Send(all_id,8,Cur_Data);//�������ݲ��һ�ȡ���ĸ����䷢�͵ģ�д��FIFOBOX��
	while((CAN1_Tx_Staus(FIFOBOX)!=0X07)&&(i<0XFFF))i++;//�ȴ����ͽ���
	if(i>=0XFFF)return 1;							//����ʧ��?
	return 0;										//���ͳɹ�;
}

/*********************************************************************
* @ ������ �� 	CAN1_Rx_Msg		
* @ ����˵�������ܾ����FIFO�������ݣ�δ��װ��
* @ ���� ��	data 				��������
*						fifox				�����
*						id 					��׼ID(11λ)/��չID(11λ+18λ)	
*						id:��׼ID(11λ)/��չID(11λ+18λ)	    
*						ide:0,��׼֡;1,��չ֡
*						rtr:0,����֡;1,Զ��֡
*						len:���յ������ݳ���(�̶�Ϊ8���ֽ�,��ʱ�䴥��ģʽ��,��Ч����Ϊ6���ֽ�)
*						dat:���ݻ�����
* @ ����ֵ �� ��
*********************************************************************/
void CAN1_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat)
{	   
	*ide=CAN1->sFIFOMailBox[fifox].RIR&0x04;//��2λIDE����ʶ����չ��λ�����������ʼ��ı�ʶ������0����׼��ʶ��1����չ��ʶ��λ1 RTR��Զ�̴�������0������֡1��Զ��֡
	
	*id=CAN1->sFIFOMailBox[fifox].RIR>>21;//��׼��ʶ��

	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;	//�õ�Զ�̷�������ֵ.
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;//�õ�DLC

	//��������(����û����ϸ�о�)
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN1->RF0R|=0X20;//�ͷ�FIFO0����
	else if(fifox==1)CAN1->RF1R|=0X20;//�ͷ�FIFO1����	 
}

//�õ���FIFO0/FIFO1�н��յ��ı��ĸ���.
//fifox:0/1.FIFO���;
//����ֵ:FIFO0/FIFO1�еı��ĸ���.
u8 CAN1_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}





