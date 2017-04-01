#ifndef __ANNEAL_H
#define __ANNEAL_H

#include <math.h>
#include <time.h>
#include <stdlib.h>

typedef struct _ANNEAL_STRUCT_
{
	int bestCost;			    //记录最好的输入值
	vector<int> bestServer;     //记录最好的服务器值
	int curCost;                //记录当前服务器值
	vector<int> curServer;		//记录当前输入服务器
	int preBestCost;            //记录上一个最好的消费
	vector<int> preBestSever;   //记录上一次最好的服务器
	int nextCost;               //记录下一次消费
    vector<int> nextServer;     //记录下一次服务器
	double temp;			    //当前温度
}ANNEAL_STR;

//模拟退火的函数
double oneRand(); //产生随机数的函数
vector<int> serverRand(); //产生服务器的序列
ANNEAL_STR annealSolve(ANNEAL_STR& annealc, MCMF graphs); //模拟退火控制函数

#endif
