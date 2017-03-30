#include "deploy.h"
#include "MCMF.h"
#include "sstream"
#include "anneal.h"

#define startTemp			100.0							//初始温度
#define stFact				1.0								//步长因子
#define minTemp				1				                //最低温度

int serverCost, netCount, edgeCount, costCount; //服务器代价, 网络节点个数, 边的个数, 消费节点个数
int costNeed[nMax]; //用来存储每个消费节点的消费需求
int costNeighbor[nodeMax];
int costNeedTotal;
int totalIn, totalOut;
int flow = 0, cost = 0; //用来存储全局的流量和消费

double oneRand()
{
    return (double)rand() / (double)RAND_MAX;
}

//方式1：用已知的服务器节点和未知的服务器节点进行交换
vector<int> exchangeServer(vector<int> curServer)
{
    int vis[netCount];
    memset(vis, 0, sizeof(vis));
    int curServerLen = (int)(curServer.size());
    for(int i = 0; i < curServerLen; i ++) vis[curServer[i]] = 1;

    int selectServer = rand() % curServerLen;
    int temp = (curServer[selectServer] + 1) % netCount;
    while(vis[temp] == 1)
        temp = (temp + 1) % netCount;

    curServer[selectServer] = temp;

    //for(int i = 0; i < curServerLen; i++)
    //{
	//cout << curServer[i] << " ";
    //}
    return curServer;
}

//方式2：从已知的服务器节点中随机去掉一个
vector<int> deleteServer(vector<int> curServer, int delStep)
{
    int vis[netCount];
    memset(vis, 0, sizeof(vis));

    int curServerLen = (int)(curServer.size());
    for(int i = 0; i < curServerLen; i ++) vis[curServer[i]] = 1;

    int selectServer = rand() % curServerLen;
    //cout << selectServer << endl;
    //根据迭代的步长决定删除的个数
    if(delStep == 1)
    {
        vis[curServer[selectServer]] = 0;
    }
    else if (delStep == 2)
    {
        vis[curServer[selectServer]] = 0;
        vis[(++ selectServer) % curServerLen] = 0;
    }
    else
    {
        vis[curServer[selectServer]] = 0;
        vis[(++ selectServer) % curServerLen] = 0;
        vis[(++ selectServer) % curServerLen] = 0;
    }

    vector<int> result;
    for(int i = 0; i < netCount; i++)
    {
        if(vis[i] == 1)
	{ 
	    result.push_back(i);
	    //cout << i << " ";
	}
    }
    //cout << endl;
    return result;
}

//方式3：从未知的服务器节点中随机添加一个
vector<int> addServer(vector<int> curServer)
{
    int vis[netCount];
    memset(vis, 0, sizeof(vis));

    int curServerLen = (int)curServer.size();
    for(int i = 0; i < curServerLen; i ++) vis[curServer[i]] = 1;

    int selectServer = rand() % curServerLen;
    int temp = (curServer[selectServer] + 1) % netCount;
    while(vis[temp] == 1)
        temp = (temp + 1) % netCount;

    curServer.push_back(temp);
    return curServer;
}

int deleteStep()
{
    int delStep = 1;
    if(costCount > 20 && costCount <= 200) delStep = 2;
    else delStep = 3;
    return delStep;
}

//模拟退火算法
ANNEAL_STR annealSolve(ANNEAL_STR& annealc, MCMF graphs)
{
	srand(time(NULL));							//随机数种子
	annealc.temp = startTemp;					//初始温度
	annealc.curCost = annealc.bestCost;
	annealc.curServer = annealc.bestServer;
    
    int delStep = deleteStep();
    int threshold = costCount - costCount / 10;

    int markovLen = 1000;
    double weakFact = 0.99;
    if(netCount < 100) 
    {
	markovLen = 1000;
	weakFact = 0.99;
    }
    else if(netCount < 200) 
    {
	markovLen = 500;
	weakFact = 0.95;
    }
    else if(netCount < 400)
    {
	markovLen = 300;
	weakFact = 0.9;
    }
    else
    {
	markovLen = 100;
	weakFact = 0.85;
    }

    while (annealc.temp > minTemp)				//当温度衰减到一定程度，认为已经达到平衡状态,视当前最优解为全局最优解
    {
	for (int i = 0; i < markovLen; i += 1)
	{
	    // 1).当前点扰动,直到输入点满足范围
	    /*
	    if(annealc.curServer.size() >= threshold)
            {
                int selectFunc = rand() % 2;
		//cout << selectFunc << endl;
                if(selectFunc == 0) annealc.nextServer = exchangeServer(annealc.curServer);
                else annealc.nextServer = deleteServer(annealc.curServer, delStep);
            }
            else
            {
		
                int selectFunc = rand() % 3;
		//cout << selectFunc << endl;
                if(selectFunc == 0) annealc.nextServer = exchangeServer(annealc.curServer);
                else if(selectFunc == 1) annealc.nextServer = deleteServer(annealc.curServer, delStep);
                else annealc.nextServer = addServer(annealc.curServer);
            }
	    */
	    int selectFunc = rand() % 3;
	    //cout << selectFunc << endl;
            if(selectFunc == 0) annealc.nextServer = exchangeServer(annealc.curServer);
            else if(selectFunc == 1) annealc.nextServer = deleteServer(annealc.curServer, delStep);
            else annealc.nextServer = addServer(annealc.curServer);

	    // 2).下个点是否为全局最优解
            annealc.nextCost = multiServerMinCost(annealc.nextServer, graphs);
            if(annealc.nextCost != INF)
            {
                annealc.nextCost += serverCost * annealc.nextServer.size();
                if(annealc.nextCost < annealc.bestCost)
                {
                    annealc.preBestCost = annealc.bestCost;
                    annealc.preBestSever = annealc.bestServer;
                    annealc.bestCost = annealc.nextCost;
                    annealc.bestServer = annealc.nextServer;
                    threshold = (int)(annealc.bestServer.size());
                }
                cout << annealc.nextCost << endl;
                cout << annealc.bestCost << endl;
            }

            // 3).Metropolis过程
            if(annealc.nextCost < annealc.curCost)
            {
                annealc.curCost = annealc.nextCost;
            }
            else
            {
		double p = exp( -1*(double)(annealc.nextCost - annealc.curCost) / (annealc.temp));			
		double temp = oneRand();
		if (temp < p)		//以概率p接受状态转换
		{
			annealc.curCost = annealc.nextCost;
			annealc.curServer = annealc.nextServer;
		}
            }
	}
        // 4).降温
	annealc.temp *= weakFact;
	cout << annealc.temp << endl;
	cout << annealc.bestCost << endl;
    }
    return annealc;
}

//计算多源多汇最短路径
int multiServerMinCost(vector<int> serverNodes, MCMF graphs)
{
    MCMF temp = graphs;
    //更新服务器周边对应的边的权值
    for(int i = 0; i < (int)(serverNodes.size()); i ++)
    {
        //添加超级源点和服务器之间的边
        int from = totalOut; int to = serverNodes[i];
        int cap = INF; int cost = 0;
        temp.addEdge(from, to, cap, cost);
    }
    //每次都要初始化flow和cost
    flow = cost = 0;
    return minCost(temp, totalOut, totalIn, costNeedTotal);
}

int minCost(MCMF temp, int s, int t, int costNeed)
{
    while(temp.bellManFord(s, t, flow, cost, costNeed, 0));
    if(flow < costNeedTotal) return INF;
    else return cost;
}

string int2Str(int temp)
{
    stringstream ss;
    ss << temp;
    string s1 = ss.str();
    return s1;
}

string print(MCMF &graphs, vector<int> resultArr)
{
    string topo;
    string strTemp;

    //更新服务器周边对应的边的权值
    for(int i = 0; i < (int)(resultArr.size()); i ++)
    {
        //添加超级源点和服务器之间的边
        int from = totalOut; int to = resultArr[i];
        int cap = INF; int cost = 0;
        graphs.addEdge(from, to, cap, cost);
    }

    int temp = 0;
    flow = cost = 0;
    while(graphs.bellManFord(totalOut, totalIn, flow, cost, costNeedTotal, 1))
    {
        graphs.result[0] -= netCount;
        for(int i = graphs.resultCount - 2; i >= 0; i --)
        {
            strTemp += int2Str(graphs.result[i]);
            strTemp += " ";
        }
        strTemp += int2Str(graphs.result[graphs.resultCount - 1]);
        strTemp += "\n";
        temp ++;
    }
    topo += int2Str(temp);
    topo += "\n\n";
    topo += strTemp;
    return topo;
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{
    //定义图
    MCMF graphs;
    stringstream str;

    str<<topo[0];
    str>>netCount>>edgeCount>>costCount;
    graphs.init(netCount + costCount + 2);

    //处理第三行的数据
    str<<topo[2];
    str>>serverCost;

    //将每条边添加到MCMF结构中
    int from, to, cap, cost;
    for(int i = 4; i < edgeCount + 4; i ++)
    {
        str<<topo[i];
        str>>from>>to>>cap>>cost;
        graphs.addEdge(from, to, cap, cost);
    }

    //添加每个消费节点，并添加一个总汇点
    costNeedTotal = 0;
    totalIn = netCount + costCount;
    totalOut = netCount + costCount + 1;
    memset(costNeed, 0, sizeof(costNeed));
    for(int i = edgeCount + 5; i < edgeCount + costCount + 5; i ++)
    {
        str<<topo[i];
        int temp=0;
        str>>temp;
        to = netCount + temp; //把消费节点添加到网络节点后面
        str>>from;
        str>>costNeed[to];
        cap = INF; cost = 0;
        costNeighbor[temp] = from; //把消费节点的邻居节点存储下来
        graphs.addEdge(from, to, cap, cost);

        //添加一个总的汇点
        cap = costNeed[to];
        costNeedTotal += costNeed[to];
        graphs.addEdge(to, totalIn, cap, cost);
    }

    ANNEAL_STR annealc;

    for(int i = 0; i < costCount; i ++) annealc.bestServer.push_back(costNeighbor[i]);
    annealc.bestCost = annealc.bestServer.size() * serverCost;

    annealc = annealSolve(annealc, graphs);

    string topo_file = print(graphs, annealc.bestServer);
    write_result(topo_file.data(), filename);
}
