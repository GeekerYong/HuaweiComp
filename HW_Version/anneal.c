#include "anneal.h"
#include "stdio.h"

/**********************************************************************************************************
����¶� �� ��ʼ�¶� ����Ӱ����ģ���˻������ʱ�䣬ҲӰ����ģ���˻���ȶ���
����Ʒ�����������ͬһ�¶�(ͬһ����)�½��ж��ٴ��Ŷ�
˥������Ӱ����ģ���˻��ʱ��
�i������Ϊ�Ŷ���С����һ�κ�����ⲻ���룬�޷�������ȫ�����Ž�ʱ�����Ե����i�����ӣ���ȷʵ������ȫ������
***********************************************************************************************************/
#define StartTemp			100.0							//��ʼ�¶�
#define WeakFact			0.99							//�¶�˥������
#define StFact				2.3								//��������
#define MinTemp				0.00000000000001				//����¶�
#define MarkovLen			1000							//����Ʒ�������
#define MAX_X				10								//�������ֵ
#define MIN_X				-10								//������Сֵ
#define WATCH				1								//�Ƿ�����ڲ����Ž������Ĺ۲�


static ANNEAL_STR	annealc;								//�˻𷨿�����

//Ŀ�꺯��
double ObjectFun(double x)
{
//	return (x*x-20)*(x*x-20);
//	return (sin(x)-0.3)*(sin(x)-0.3);
	return sin(x)+cos(0.3*x);
}

/*********************************************************************************************
Function	:OneRand
Param		:no
Return		:���������
Description	:��ù�һ�������
**********************************************************************************************/
static double OneRand(void)
{
	return (rand()/32768.0);
}

/*********************************************************************************************
Function	:AnnealSolve
Param		:px---��ѽ�, py---��ѽ��Ŀ�꺯��ֵ
Return		:no
Description	:�˻�ģ���㷨��Ŀ�꺯����Сֵ
**********************************************************************************************/
void AnnealSolve(double *px, double *py)
{
	double i=0;
	double p=0;

	srand(time(NULL));							//���������
	annealc.Temp = StartTemp;					//��ʼ�¶�
	annealc.X = OneRand()*MAX_X;				//��ȡ���������
	annealc.BestX = annealc.X;
	annealc.PreBestX = OneRand()*MAX_X;
	if (WATCH)	printf("%f, %f\n", annealc.BestX, ObjectFun(annealc.BestX));

	while (annealc.Temp > MinTemp)				//���¶�˥����һ���̶ȣ���Ϊ�Ѿ��ﵽƽ��״̬,�ӵ�ǰ���Ž�Ϊȫ�����Ž�
	{
		for (i=0; i<MarkovLen; i+=1)
		{
			// 1).��ǰ���Ŷ�,ֱ����������㷶Χ
			do{
				double delta = StFact * OneRand();
				p = OneRand();
				if (p>0.5)	delta *= -1;
				annealc.NextX = annealc.X + delta;
			}while(annealc.NextX > MAX_X || annealc.NextX < MIN_X);

			// 2).�¸����Ƿ�Ϊȫ�����Ž�
			if (ObjectFun(annealc.NextX) < ObjectFun(annealc.BestX))
			{
				annealc.PreBestX = annealc.BestX;			//������һ�����Ž�
				annealc.BestX = annealc.NextX;				//�������Ž�
				if (WATCH)	printf("%f, %f\n", annealc.BestX, ObjectFun(annealc.BestX));
			}

			// 3).Metropolis����
			if (ObjectFun(annealc.NextX) < ObjectFun(annealc.X))		//�����һ���㣬���ڵ�ǰ�㣬��״̬��Ȼת��
			{
				annealc.X = annealc.NextX;
			}
			else														//������Mepropolis���ʽ���״̬ת��
			{
				p = exp( -1*(ObjectFun(annealc.NextX)-ObjectFun(annealc.X))/annealc.Temp);
				if (OneRand() < p)		//�Ը���p����״̬ת��
				{
					annealc.X = annealc.NextX;
				}
			}
		}

		// 4).����
		annealc.Temp *= WeakFact;
	}
	*px = annealc.BestX ;
	*py = ObjectFun(*px);
}
