#include "anneal.h"
#include "stdio.h"

/**********************************************************************************************************
最低温度 和 初始温度 不仅影响了模拟退火的收敛时间，也影响了模拟退火的稳定性
马尔科夫链代表了在同一温度(同一概率)下进行多少次扰动
衰减因子影响了模拟退火的时间
歩长因子为扰动大小，当一次函数求解不理想，无法收敛于全局最优解时，尝试调整歩长因子，以确实进行了全局搜索
***********************************************************************************************************/
#define StartTemp			100.0							//初始温度
#define WeakFact			0.99							//温度衰减因子
#define StFact				2.3								//步长因子
#define MinTemp				0.00000000000001				//最低温度
#define MarkovLen			1000							//马尔科夫链长度
#define MAX_X				10								//输入最大值
#define MIN_X				-10								//输入最小值
#define WATCH				1								//是否进行内部最优解修正的观察


static ANNEAL_STR	annealc;								//退火法控制器

//目标函数
double ObjectFun(double x)
{
//	return (x*x-20)*(x*x-20);
//	return (sin(x)-0.3)*(sin(x)-0.3);
	return sin(x)+cos(0.3*x);
}

/*********************************************************************************************
Function	:OneRand
Param		:no
Return		:返回随机数
Description	:获得归一化随机数
**********************************************************************************************/
static double OneRand(void)
{
	return (rand()/32768.0);
}

/*********************************************************************************************
Function	:AnnealSolve
Param		:px---最佳解, py---最佳解的目标函数值
Return		:no
Description	:退火模拟算法解目标函数最小值
**********************************************************************************************/
void AnnealSolve(double *px, double *py)
{
	double i=0;
	double p=0;

	srand(time(NULL));							//随机数种子
	annealc.Temp = StartTemp;					//初始温度
	annealc.X = OneRand()*MAX_X;				//获取任意随机数
	annealc.BestX = annealc.X;
	annealc.PreBestX = OneRand()*MAX_X;
	if (WATCH)	printf("%f, %f\n", annealc.BestX, ObjectFun(annealc.BestX));

	while (annealc.Temp > MinTemp)				//当温度衰减到一定程度，认为已经达到平衡状态,视当前最优解为全局最优解
	{
		for (i=0; i<MarkovLen; i+=1)
		{
			// 1).当前点扰动,直到输入点满足范围
			do{
				double delta = StFact * OneRand();
				p = OneRand();
				if (p>0.5)	delta *= -1;
				annealc.NextX = annealc.X + delta;
			}while(annealc.NextX > MAX_X || annealc.NextX < MIN_X);

			// 2).下个点是否为全局最优解
			if (ObjectFun(annealc.NextX) < ObjectFun(annealc.BestX))
			{
				annealc.PreBestX = annealc.BestX;			//保留上一个最优解
				annealc.BestX = annealc.NextX;				//更新最优解
				if (WATCH)	printf("%f, %f\n", annealc.BestX, ObjectFun(annealc.BestX));
			}

			// 3).Metropolis过程
			if (ObjectFun(annealc.NextX) < ObjectFun(annealc.X))		//如果下一个点，优于当前点，则状态必然转换
			{
				annealc.X = annealc.NextX;
			}
			else														//否则以Mepropolis概率接受状态转换
			{
				p = exp( -1*(ObjectFun(annealc.NextX)-ObjectFun(annealc.X))/annealc.Temp);
				if (OneRand() < p)		//以概率p接受状态转换
				{
					annealc.X = annealc.NextX;
				}
			}
		}

		// 4).降温
		annealc.Temp *= WeakFact;
	}
	*px = annealc.BestX ;
	*py = ObjectFun(*px);
}
