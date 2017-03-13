#include <cstdio>
#include <algorithm>
#include <cstring>
#include <queue>
using namespace std;

const int nmax = 2005;
const int lineMax = 20;
const int INF = 1 << 32;

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
    vector<int> G[nmax]; //����ͼ�Ĵ洢
    int inq[nmax]; //��־λ�����ж����Ƿ��ڶ�����
    int d[nmax]; //Bellman-Ford�㷨��Դ�㵽����������С����
    int p[nmax]; //��һ����
    int a[nmax]; //���ԸĽ�����

    void init(int n)
    {
        this -> n = n;
        for (int i = 0; i < n; i ++) G[i].clear();
        edges.clear();
    }

    void addEdge(int from, int to, int cap, int cost)
    {
        edges.push_back((edge){from, to, cap, 0, cost});
        edges.push_back((edge){to, from, 0, 0, -cost});
        m = edges.size();
        G[from].push_back(m - 2);
        G[to].push_back(m - 1);
    }

    bool bellManFord(int s, int t, int &flow, int &cost)
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
            for (int i = 0; i < G[u].size(); i ++)
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
        flow += a[t];
        cost += d[t] * a[t];
        int u = t;
        while (u != s)
        {
            edges[p[u]].flow += a[t];
            edges[p[u]^1].flow -= a[t];
            u = edges[p[u]].from;
        }
        return true;
    }

    int MinCost(int s, int t)
    {
        int flow = 0, cost = 0;
        while(bellManFord(s, t, flow, cost));
        return cost;
    }
};

int str2Int(char* str);

void dealLineStr(char* str);

void init(char* topo[MAX_EDGE_NUM], int line_num);
