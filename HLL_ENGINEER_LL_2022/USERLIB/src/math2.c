#include "math2.h"

/* ���ٿ��� */
float Quick_Sqrt(float num)
{
	float halfnum = 0.5f * num;
	float y = num;
	long i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (1.5f - (halfnum * y * y));
	return y;
}

/* б�º��� */
void Ramp_Calc(float *output, float input, float min_value, float max_value)
{
	*output += input;
	if (*output > max_value)
	{
		*output = max_value;
	}
	else if (*output < min_value)
	{
		*output = min_value;
	}
}
/* б�º���int ������*/
void Ramp_Calc_Int(int *output, int input, int tar_value)
{
	if(tar_value>*output)
	{
		*output += input;
		if(*output>tar_value)
		{
			*output=tar_value;
		}
	}
	else if(tar_value<*output)
	{
		*output -= input;
		if(*output<tar_value)
		{
			*output=tar_value;
		}
	}
	else *output=tar_value;
}
/* �����º����� */
void Steep_Calc(float *output, float *output_tmp, float input, float min_value, float max_value)
{
	*output_tmp += input;
	*output = *output_tmp;  //todo
	*output += input;
	if (*output > max_value)
	{
		*output = max_value;
	}
	else if (*output < min_value)
	{
		*output = min_value;
	}
}

/* int16�޷� */
void Int16_Constrain(int16_t* data, int16_t min_value, int16_t max_value)
{
	if(*data > max_value) *data = max_value;
	else if(*data < min_value) *data = min_value;
}

int16_t Int16_Limit(int16_t data, int16_t min_value, int16_t max_value)
{
	if(data > max_value) return max_value;
	else if(data < min_value) return min_value;
	return data;
}

/* float�޷� */
void Float_Constrain(float* data, float min_value, float max_value)
{
	if(*data > max_value) *data = max_value;
	else if(*data < min_value) *data = min_value;
}

//���������float
float Hex4_To_Float1(unsigned char *array)
{
	unsigned char array_copy[4];
	float f;
	
	for(unsigned int i=0; i<4; i++)
	{
		array_copy[i]=array[i];
	}
	f = *((float *)((array_copy)));
	return f;
	
}

uint16_t U8_Array_To_U16(uint8_t *array)
{	
	return ( (array[1]<<8) | array[0] );
}

uint32_t U8_Array_To_U32(uint8_t *array)
{	
	return ( (array[1]<<24)|(array[1]<<16)|(array[1]<<8) | array[0] );
}

/* �Ƕ�Pidʱ���ڻ�ȡtar��cur֮������ŵ��� */
void Handle_Angle8191_PID_Over_Zero(float *tar, float *cur)
{
	if(*tar - *cur > 4096)   //4096 ����Ȧ��е�Ƕ�
	{
		*cur += 8192;
	}
	else if(*tar - *cur < -4096)
	{
		*cur = *cur - 8192;
	}
	else
	{
		//pid->cur = cur;
	}
}

/* �Ƕ�Pidʱ���ڻ�ȡtar��cur֮������ŵ��� */
void Handle_Angle360_PID_Over_Zero(float *tar, float *cur)
{
	if(*tar - *cur > 180)   //4096 ����Ȧ��е�Ƕ�
	{
		*cur += 360;
	}
	else if(*tar - *cur < -180)
	{
		*cur = *cur - 360;
	}
}


/*
 * ��̨YAW�� GM6020����Ƕ�ת�����Ե��̵�ͷ�ĳ���Ϊ0��
 * 0~8191 -> 0~360
 * ROBOT_HEAD_ANGLE �궨���ǽ���̨��ͷ�͵��̵�ͷ���䣬������GM6020��е�Ƕ�
*/
#define ROBOT_HEAD_ANGLE 2735
float GM6020_YAW_Angle_To_360(uint16_t gm6020_angle)
{
	int16_t yaw_angle = gm6020_angle-2735;
	if(yaw_angle<0)
	{
		yaw_angle += 8191;
	}
	return (float)(yaw_angle*360/8191);
}


//Pitch�Ƕ��޷�
void Pitch_Angle_Limit(float* angle, float down_angle, float up_angle)
{
	if(down_angle > up_angle)
	{
		if(*angle > down_angle) *angle = down_angle;
		else if(*angle < up_angle) *angle = up_angle;	
	}
	else if(down_angle <= up_angle)
	{
	  if(*angle < down_angle) *angle = down_angle;
		else if(*angle > up_angle) *angle = up_angle;
	}
		
}

float KalmenFilter(float data, float Q, float R)
{
  static float x_last;
  float x_mid = x_last;
  float x_now;

  static double p_last;
  double p_mid ;
  double p_now;

  double kg;

  x_mid = x_last;                       //x_last=x(k-1|k-1),x_mid=x(k|k-1)
  p_mid = p_last+Q;                     //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����

  /*
   *  �������˲��������Ҫ��ʽ
   */
  kg = p_mid / (p_mid + R);                 //kgΪkalman filter��R Ϊ����
  x_now = x_mid + kg * (data - x_mid); //���Ƴ�������ֵ
  p_now = (1 - kg) * p_mid;                 //����ֵ��Ӧ��covariance
  p_last = p_now;                           //����covariance ֵ
  x_last = x_now;                           //����ϵͳ״ֵ̬

  return x_now;
}
