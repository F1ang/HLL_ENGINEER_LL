#include "can2_hll.h"

/*********************************************************************
 * @brief CAN��ʼ��
 * 
 * @return int 
*********************************************************************/
int Can2_Init(void)//CAN��ʼ��
{
	/*��ʼ��PB12(CAN2_RX) PB13(CAN2_TX)*/
	RCC->AHB1ENR|=1<<1;//ʹ��GPIO PORTBʱ�� (1<<0����GPIOA 1<<1����GPIOB 1<<2����GPIOC)

	GPIOB->MODER |=2<<26|2<<24;//Moder12��0x10 Moder13��0x10 /*�Һܺ���һ������һ������(�����ø��õ�ģʽ)����ô��?*/

	GPIOB->OTYPER &= 0xFFFF;//(OT12,OT13��0)1-16˭����˭��0, 16��32Ϊ�գ���ɶ���У���Ĭ����ȫ��Ϊ0(��仰��һ��ϻ�)

	GPIOB->OSPEEDR |= 3<<26|3<<24;//PD0 -- PD1 �ٶ�100m

	SYSCFG->CMPCR = 0x00000001;//ʹ��IO������Ԫ(������Ԫ�ϵ�)/*��GPIO�ٶȳ���50M��ʱ��Ҫ����ʹ�ô�����(0λ��0�رղ�����1��������)*/
	
	GPIOB->PUPDR |=  1<<26|1<<24;//PB12 PB13�������
	
	/*�ٷ��Ĵ�����װʱ��Ϊ��0x20(AFRl)��0x24(AFRH)�����Ĵ����ֿ�����������һ��ײ�Ĵ���ָ����ʱ��ͨ��AFR[2]��ʵ�������Ĵ�����д��*/
	/*��Ϊ��12,13����ѡ���λAFR[1],��ѯAF����CAN1��can3��Ӧ��AF9��������Ҫ��0x1001*/ 
	GPIOB->AFR[1]|=9<<16;
	GPIOB->AFR[1]|=9<<20; 
	
	int i =0;
	RCC->APB1ENR|= 1<<26;//ʹ��CAN2ʱ��(λ26��1�Ǵ�can2ʱ��)(RCC APB1 ����ʱ��ʹ�ܼĴ���)
	CAN2->MCR=0x0000;	//�˳�˯��ģʽ(�����һλ˯��λ)
	CAN2->MCR|=1<<0;		//����CAN�����ʼ��ģʽ
	
	while((CAN2->MSR&1<<0)==0)   //CANӲ���˳���ʼ��ģʽʱ��CAN-MSR(CAN��״̬�Ĵ���)λ0 INAK��Ӳ������
	{
		i++;
		if(i>100){return 2;}//�����ʼ��ģʽʧ��
	}

	//CAN��״̬�Ĵ���MCR
	CAN2->MCR|=0<<7;		//��ֹʱ�䴥��ͨ��ģʽ
	CAN2->MCR|=0<<6;		//����Զ����߹����˳����߹ر�״̬��
	CAN2->MCR|=0<<5;		//˯��ģʽͨ���������(���CAN2->MCR��SLEEPλ)
	CAN2->MCR|=1<<4;		//0��CAN Ӳ�����Զ��ط�����Ϣ��ֱ������ CAN ��׼��Ϣ���ͳɹ�1�����۷��ͽ����Σ��ɹ���������ٲö�ʧ������Ϣ��ֻ����һ��
	CAN2->MCR|=0<<3;		//���Ĳ�����,�µĸ��Ǿɵ�
	CAN2->MCR|=0<<2;		//0:���ȼ��ɱ��ı�ʶ������1:�������ʱ��˳�����

	//CANλʱ��Ĵ�����bit timing register��
	CAN2->BTR=0x00000000;	//���ԭ��������.
	CAN2->BTR|=0<<30;	//ģʽ���ã� 0,��ͨģʽ;1,�ػ�ģʽ;
	CAN2->BTR|=0<<24; 	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ
	CAN2->BTR|=1<<20; 	//Tbs2=tbs2+1��ʱ�䵥λ
	CAN2->BTR|=5<<16;	//Tbs1=tbs1+1��ʱ�䵥λ
	CAN2->BTR|=4<<0;  	//��Ƶϵ��(Fdiv)Ϊbrp+1
							//������:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)=45/(2+6+1)/5
	CAN2->MCR&=~(1<<0);		//����CAN�˳���ʼ��ģʽ
	while((CAN2->MSR&1<<0)==1)
	{
		i++;
		if(i>0xFFF0){return 3;}//�˳���ʼ��ģʽʧ��
	}	
	
	Can2_Filter();//�򿪹�����
//ʹ���жϽ���    
#if CAN2_RX0_INT_ENABLE	
	CAN2->IER|=1<<1;		//FIFO0��Ϣ�Һ��ж�����.
	//����4
  u32 temp;		   
	temp=6;	  //�������ȼ�6
	temp|=0&(0x0f>>4);  //��Ӧ���ȼ�0
	temp&=0xf;								//ȡ����λ
	NVIC->ISER[CAN2_RX0_IRQn/32]|=1<<CAN2_RX0_IRQn%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
	NVIC->IP[CAN2_RX0_IRQn]|=temp<<4;				//������Ӧ���ȼ����������ȼ�
#endif
	return 0;
}
void Can2_Filter(void)//CAN���������޹���
{
	//��������ʼ��
	CAN2->FMR|=1<<0;		//λ0: 0������ģʽ��1���������鹤���ڳ�ʼ��ģʽ(CANɸѡ������ʱ��)
	CAN2->FA1R&=~(1<<12);//������12�����CAN1��PD0ΪRX��CAN2��RXΪPB12��������������
	CAN2->FS1R|=1<<12; 	//������λ��Ϊ32λ.
	CAN2->FM1R|=0<<12;		//������12�����ڱ�ʶ������λģʽ��1Ϊ�б�ģʽ��
	CAN2->FFA1R|=0<<0;		//������0������FIFO0
	CAN2->sFilterRegister[0].FR1=0X00000000;//32λID
	CAN2->sFilterRegister[0].FR2=0X00000000;//32λMASK
	CAN2->FA1R|=1<<0;		//���������0
	CAN2->FMR&=0<<0;		//���������������ģʽ
}

/*********************************************************************
 * @brief ׼����CAN_TXʹ�õ�MAILBOX��������
 * 
 * @param Distinguish_ID 	CAN����ʶ��ID������M3508�����˵����0x00000200��0x000001FF��
 * @param len  				���ݳ���(��׼֡8λ)
 * @param data 				��������
 * @return u8 �����
*********************************************************************/
u8 Motor_Can2_Send(u32 Distinguish_ID,u8 len ,u8 *data)
{
	/*Ѱ�ҿ�����*/
	//CAN_TSR:CAN����״̬�Ĵ���
	u8 FIFOBOX = 0;//�ĸ������ǿյĿ��Դ��FIFO
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
	else return 0XFF;					//�޿�����,�޷�����
	/*�򿪶�Ӧ����,stm32F4xx.h�ļ����ж�Ӧ�Ľṹ��*/
	/*CAN_TxMailBox_TypeDef��sTxMailBox[3]�ײ��ļ���ͨ����������ѡ��MAILBOX0����MAILBOX1����MAILBOX2*/
	CAN2->sTxMailBox[FIFOBOX].TIR=0;		//���֮ǰ������
		
	Distinguish_ID&=0x7ff;						//ȡ��11λstdid
	Distinguish_ID<<=21;							//ת��д��Ĵ�����ʶ��ID(��׼֡ID)
	
	CAN2->sTxMailBox[FIFOBOX].TIR|=Distinguish_ID;	//д��ʶ��ID	 
	CAN2->sTxMailBox[FIFOBOX].TIR|=0<<2;						//0:��׼֡��1:��չ֡	  
	CAN2->sTxMailBox[FIFOBOX].TIR|=0<<1;					//0:����֡  1:Զ��֡
	len&=0X0F;//�õ�����λ
	CAN2->sTxMailBox[FIFOBOX].TDTR&=~(0X0000000F);
	CAN2->sTxMailBox[FIFOBOX].TDTR|=len;		   //����DLC.
	
	/*���������ݴ������䣨д��Ĵ�����*/
	CAN2->sTxMailBox[FIFOBOX].TDHR=(((u32)data[7]<<24)|
								((u32)data[6]<<16)|
 								((u32)data[5]<<8)|
								((u32)data[4]));
	CAN2->sTxMailBox[FIFOBOX].TDLR=(((u32)data[3]<<24)|
								((u32)data[2]<<16)|
 								((u32)data[1]<<8)|
								((u32)data[0]));
	CAN2->sTxMailBox[FIFOBOX].TIR|=1<<0; //��������������
	return FIFOBOX;
}

/*********************************************************************
 * @brief ��÷���״̬,�Ƿ������
 * 
 * @param FIFOBOX ���ж������䣰���䣱���䣲�е�һ����
 * @return 0x00,����;0X05,����ʧ��;0X07,���ͳɹ�. 
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
			sta=0X05;//����Ų���,�϶�ʧ��.
		break;
	}
	return sta;
} 

/*********************************************************************
 * @brief ����M3508�������
 * 
 * @param data ����ֵ����
 * @return u8 1���ͳɹ� 0����ʧ��
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
  FIFOBOX=Motor_Can2_Send(0x00000200,8,Cur_Data);//�������ݲ��һ�ȡ���ĸ����䷢�͵ģ�д��FIFOBOX��
	while((CAN2_Tx_Staus(FIFOBOX)!=0X07)&&(i<0XFFF))i++;//�ȴ����ͽ���
	if(i>=0XFFF)return 1;							//����ʧ��?
	return 0;										//���ͳɹ�;
}



/*********************************************************************
 * @brief 	���ܾ����FIFO�������ݣ�δ��װ��
 * 
 * @param fifox 	�����
 * @param id 		��׼ID(11λ)/��չID(11λ+18λ)
 * @param ide 		0,��׼֡;1,��չ֡
 * @param rtr 		0,����֡;1,Զ��֡
 * @param len 		���յ������ݳ���(�̶�Ϊ8���ֽ�,��ʱ�䴥��ģʽ��,��Ч����Ϊ6���ֽ�)
 * @param dat 		���ݻ�������8��Ԫ��Ϊ���״̬
*********************************************************************/
void CAN2_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat)
{	   
	*ide=CAN2->sFIFOMailBox[fifox].RIR&0x04;//��2λIDE����ʶ����չ��λ�����������ʼ��ı�ʶ������0����׼��ʶ��1����չ��ʶ��λ1 RTR��Զ�̴�������0������֡1��Զ��֡
	
	*id=CAN2->sFIFOMailBox[fifox].RIR>>21;//��׼��ʶ��

	*rtr=CAN2->sFIFOMailBox[fifox].RIR&0x02;	//�õ�Զ�̷�������ֵ.
	*len=CAN2->sFIFOMailBox[fifox].RDTR&0x0F;//�õ�DLC

	//��������(����û����ϸ�о�)
	dat[0]=CAN2->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN2->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN2->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN2->sFIFOMailBox[fifox].RDLR>>24)&0XFF;

	dat[4]=CAN2->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN2->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN2->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN2->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN2->RF0R|=0X20;//�ͷ�FIFO0����
	else if(fifox==1)CAN2->RF1R|=0X20;//�ͷ�FIFO1����	 
}

//�õ���FIFO0/FIFO1�н��յ��ı��ĸ���.
//fifox:0/1.FIFO���;
//����ֵ:FIFO0/FIFO1�еı��ĸ���.
u8 CAN2_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN2->RF0R&0x03; 
	else if(fifox==1)return CAN2->RF1R&0x03; 
	else return 0;
}

/*********************************************************************
* @ ������ �� 	Query_Accept_Data		
* @ ����˵������ѯ��������(�ѷ�װ)
* @ ���� ��	buf 		�ݴ�����
* @ ����ֵ �� 0:δ�������ݻ�������һ�� ����:���ID
*********************************************************************/
u8 Query_Accept_Data2(u8 *buf)
{
	u32 ID;
	u8 ide,rtr,len; 
	if(CAN2_Msg_Pend(0)==0&&CAN2_Msg_Pend(1)==0){/*printf("û�����ݽ���");*/return 0;}		//û�н��յ�����,ֱ���˳� 	 
  	CAN2_Rx_Msg(0,&ID,&ide,&rtr,&len,buf); 	//��ȡ���ݣ����ݴ����ݷ�����buf��
    if(ide!=0||rtr!=0){/*printf("�����쳣")*/;return 0;}		//������Ǳ�׼֡;��������֡;�����쳣��ֱ���˳�; 
	
	return ID;	//���ص��ID������������˭������
}
/*********************************************************************
* @ ������ �� 	Printf_CAN2_Data		
* @ ����˵��������CAN2��������
* @ ���� ��	CAN2_Motor 		can.h�鿴�ṹ���ļ�
* @ ����ֵ ����
*********************************************************************/		


