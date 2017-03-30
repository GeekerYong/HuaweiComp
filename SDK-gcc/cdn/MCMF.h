#include <cstdio>
#include <algorithm>
#include <cstring>
#include <queue>

#include <iostream>
using namespace std;

const int nMax = 2005; //�ܽ������ֵ
const int lineMax = 20; //ÿ�е��ַ��������ֵ
const int nodeMax = 1005; //����������ѽڵ�����ֵ
const int INF = 0x7fffffff;
const char* splitStr = " \n";
const int maxLineNum = 1005 * 1005;

//�ߵ����ݽṹ
struct edge
{
    int from, to, cap, flow, cost;
};

//��С��������������ݽṹ
struct MCMF
{
    int n, m, s, t; //n��ʾ�ڵ������mΪ�ߵĸ���,sΪԴ�㣬tΪ���
    vector<edge> edges; //���бߵĴ洢
    vector<int> G[nMax]; //����ͼ�Ĵ洢
    int inq[nMax]; //��־λ�����ж����Ƿ��ڶ�����
    int d[nMax]; //Bellman-Ford�㷨��Դ�㵽����������С����
    int p[nMax]; //��һ����
    int a[nMax]; //���ԸĽ�����

    char** topo; //���ڴ�ӡ���
    vector<int> result; //���ڴ洢�����·��
    int resultCount;

    void init(int n)//��ʼ��
    {
        this->n=n;
        for(int i=0;i<n;i++)
            G[i].clear();
        edges.clear();
    }

    void addEdge(int from, int to, int cap, int cost)
    {
        edges.push_back((edge){from, to, cap, 0, cost});
        edges.push_back((edge){to, from, 0, 0, -cost});
        m = (int)(edges.size());
        G[from].push_back(m - 2);
        G[to].push_back(m - 1);
    }

    bool bellManFord(int s, int t, int &flow, int &cost, int costNeed, int isPr)
    {
        for (int i = 0; i < n; i ++) d[i] = INF;
        memset(inq, 0, sizeof(inq));
        d[s] = 0; inq[s] = 1; p[s] = 0; a[s] = INF;

        queue<int> q;
        q.push(s);
        while(!q.empty())
        {
            int u = q.front(); q.pop();
            inq[u] = 0;
            for (int i = 0; i < (int)(G[u].size()); i ++)
            {
                edge& e = edges[G[u][i]];
                if (e.cap > e.flow && d[e.to] > d[u] + e.cost)
                {
                    d[e.to] = d[u] + e.cost;
                    p[e.to] = G[u][i];
                    a[e.to] = min(a[u], e.cap - e.flow);
                    if(!inq[e.to])
                    {
                        q.push(e.to); inq[e.to] = 1;
                    }
                }
            }
        }

        if(d[t] == INF) return false;
        //��ӱ�־λ���������������������ѽڵ�����������ֹͣ����
        int flag = 0;
        if(flow + a[t] > costNeed)
        {
            a[t] = costNeed - flow;
            flag = 1;
        }
        //==========================================================
        flow += a[t];
        cost += d[t] * a[t];
        int u = t;

        resultCount = 0;
        result.clear();
        while (u != s)
        {
            edges[p[u]].flow += a[t];
            edges[p[u]^1].flow -= a[t];
            u = edges[p[u]].from;

            if(isPr == 1 && u != s)
            {
                resultCount ++;
                result.push_back(u);
            }
        }
        //�������
        if(isPr == 1)
        {
            resultCount ++;
            result.push_back(a[t]);
        }

        if(flag == 1) return false;
        return true;
    }
};

//��С����������ĺ���
int multiServerMinCost(vector<int> serverNodes, MCMF graphs);
int minCost(MCMF temp, int s, int t, int costNeed);

//�����������ĺ���
string int2Str(int temp);
string print(MCMF &graphs, vector<int> resultArr);
