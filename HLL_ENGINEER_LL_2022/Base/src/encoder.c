#include "encoder.h"


// ��ȡ��ʱ������ֵ
int read_tim2_encoder(void)
{
	int encoder_num111;
	encoder_num111 = (int)((int16_t)(TIM2->CNT)); // ����������Ҫע����������
	LL_TIM_SetCounter(TIM2,0); 
	return encoder_num111;
}

int read_tim4_encoder(void)
{
	int encoder_num;
	encoder_num = (int)((int16_t)(TIM4->CNT)); // ����������Ҫע����������
	LL_TIM_SetCounter(TIM4,0); 
	return encoder_num;
}

int read_tim5_encoder(void)
{
	int encoder_num111;
	encoder_num111 = (int)((int16_t)(TIM5->CNT)); // ����������Ҫע����������
	LL_TIM_SetCounter(TIM5,0); 
	return encoder_num111;
}



void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//����ж�
	{    				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
}
void TIM4_IRQHandler(void)
{ 		    		  			    
	if(TIM4->SR&0X0001)//����ж�
	{    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//����жϱ�־λ 	    
}

void TIM5_IRQHandler(void)
{ 		    		  			    
	if(TIM5->SR&0X0001)//����ж�
	{    				   				     	    	
	}				   
	TIM5->SR&=~(1<<0);//����жϱ�־λ 	    
}


