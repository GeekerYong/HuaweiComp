#ifndef __ANNEAL_H
#define __ANNEAL_H

#include <math.h>
#include <time.h>
#include <stdlib.h>

typedef struct _ANNEAL_STRUCT_
{
	int bestCost;			    //��¼��õ�����ֵ
	vector<int> bestServer;     //��¼��õķ�����ֵ
	int curCost;                //��¼��ǰ������ֵ
	vector<int> curServer;		//��¼��ǰ���������
	int preBestCost;            //��¼��һ����õ�����
	vector<int> preBestSever;   //��¼��һ����õķ�����
	int nextCost;               //��¼��һ������
    vector<int> nextServer;     //��¼��һ�η�����
	double temp;			    //��ǰ�¶�
}ANNEAL_STR;

//ģ���˻�ĺ���
double oneRand(); //����������ĺ���
vector<int> serverRand(); //����������������
ANNEAL_STR annealSolve(ANNEAL_STR& annealc, MCMF graphs); //ģ���˻���ƺ���

#endif
