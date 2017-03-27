#include "deploy.h"
#include "MCMF.h"
#include "sstream"
//MCMF graphs;
int serverCost, netCount, edgeCount, costCount; //服务器代价, 网络节点个数, 边的个数, 消费节点个数
int costNeed[nMax]; //用来存储每个消费节点的消费需求
int costNeighbor[nodeMax];
int costNeedTotal;
int totalIn, totalOut;

int flow = 0, cost = 0; //用来存储全局的流量和消费
int maxTempFlow = 0, maxTempFlowNum = -1; //存储流量和消费
int minTempCost = INF, minTempCostNum = -1;

int resultCost = INF; //存放花费最终结果

vector<int> resultArr; //存放最终的数组
int serverDp[2][nodeMax]; //服务器动态规划的数组
vector<int> serverStatus[2][nodeMax]; //用于存储服务器状态的数组

//判断是否重复
bool isRepeat(vector<int> temp, int a)
{
    for(int i = 0; i < (int)(temp.size()); i ++)
    {
        if(temp[i] == a)
            return true;
    }
    return false;
}

//用来处理服务器选址，暴力枚举
void  serverChooseDP(MCMF graphs)
{
    //初始化serverDp数组
    for(int i = 0; i < 2; i ++)
        for(int j = 0; j < netCount; j ++)
            serverDp[i][j] = INF;

    int tempCost, tempTotal;
    for(int i = 0; i < netCount; i ++) //初始化第一行的数据
    {
        serverStatus[0][i].push_back(i);
        tempCost = multiServerMinCost(serverStatus[0][i], graphs);

        if(tempCost == INF) serverDp[0][i] = INF;
        else
        {
            serverDp[0][i] = cost + serverCost * (int)(serverStatus[0][i].size());
            if(serverDp[0][i] < resultCost)
            {
                resultCost = serverDp[0][i];
                resultArr = serverStatus[0][i];
            }
        }
    }

    bool isRe;
    vector<int> tempArr;
    for(int i = 1; i < costCount; i ++) //枚举服务器的个数，不超过costCount个
    {
        for(int j = 0; j < netCount; j ++) //对第i层的每个节点进行枚举
        {
            int curDeep = i % 2, lastDeep = (i + 1) % 2;
            maxTempFlow = 0, maxTempFlowNum = -1;
            //minTempCost = INF, minTempCostNum = -1;
            for(int k = 0; k < netCount; k ++) //对i-1层每个节点进行枚举
            {
                isRe = isRepeat(serverStatus[lastDeep][k], j);
                if(isRe == true) continue;

                tempArr = serverStatus[lastDeep][k];
                tempArr.push_back(j);
                tempCost = multiServerMinCost(tempArr, graphs);
                //若不存在路径，挑选流量最大的K来作为DP状态数组的更新,这个部分可以修改
                if(tempCost == INF)
                {
                    serverDp[curDeep][j] = INF;
                    if(maxTempFlow < flow)
                    {
                        maxTempFlow = flow;
                        maxTempFlowNum = k;
                    }
                }
                else
                {
                    //若存在路径，挑选总花费最小的服务器节点
                    tempTotal = tempCost + serverCost * (int)(tempArr.size());
                    if(tempTotal < serverDp[curDeep][j])
                    {
                        serverDp[curDeep][j] = tempTotal;
                        serverStatus[curDeep][j] = tempArr;
                    }
                }
            }
            //若路径不存在，选择最小消费的路径
            if(serverDp[curDeep][j] == INF)
            {
                serverStatus[curDeep][j] = serverStatus[lastDeep][maxTempFlowNum];
                serverStatus[curDeep][j].push_back(j);
            }
            //更新最终结果
            if(serverDp[curDeep][j] < resultCost)
            {
                resultCost = serverDp[curDeep][j];
                resultArr = serverStatus[curDeep][j];
            }
        }
    }
    //对最后一层的节点进行汇总
    int finalDeep = (netCount + 1) % 2;
    for(int i = 0; i < netCount; i ++)
    {
        if(serverDp[finalDeep][i] < resultCost)
        {
            resultCost = serverDp[finalDeep][i];
            resultArr = serverStatus[finalDeep][i];
        }
    }
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
        //cout << serverNodes[i] << " ";
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

char* int2Str(int temp)
{
    char* b = new char;
    sprintf(b, "%d", temp);
    return b;
}

char* print(MCMF &graphs)
{
    char* topo = (char*)malloc(maxLineNum);
    char* strTemp = (char*)malloc(maxLineNum - 10);

    //更新服务器周边对应的边的权值
    for(int i = 0; i < (int)(resultArr.size()); i ++)
    {
        //添加超级源点和服务器之间的边
        int from = totalOut; int to = resultArr[i];
        int cap = INF; int cost = 0;
        graphs.addEdge(from, to, cap, cost);
    }

    int temp = 0;
    char* space = (char*)" ";
    char* enter = (char*)"\n";

    flow = cost = 0;
    while(graphs.bellManFord(totalOut, totalIn, flow, cost, costNeedTotal, 1))
    {
        graphs.result[0] -= netCount;
        for(int i = graphs.resultCount - 2; i >= 0; i --)
        {
            strTemp = strcat(strTemp, int2Str(graphs.result[i]));
            strTemp = strcat(strTemp, space);
        }
        strTemp = strcat(strTemp, int2Str(graphs.result[graphs.resultCount - 1]));
        strTemp = strcat(strTemp, enter);
        temp ++;
    }

    topo = strcat(topo, int2Str(temp));
    topo = strcat(topo, enter);
    topo = strcat(topo, enter);
    topo = strcat(topo, strTemp);

    return topo;
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{
    //定义图
    MCMF graphs;

    stringstream str;
    //处理第一行的数据
    str<<topo[0];
    str>>netCount>>edgeCount>>costCount;
    //cout<<netCount<<" "<<edgeCount<<" "<<costCount<<endl;
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
        //cout<<from<<" "<<to<<" "<<cap<<" "<<cost<<endl;
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
        //cout<<"consume:"<<from<<" "<<to<<" "<<cap<<" "<<cost<<endl;
        graphs.addEdge(from, to, cap, cost);

        //添加一个总的汇点
        cap = costNeed[to];
        costNeedTotal += costNeed[to];
        graphs.addEdge(to, totalIn, cap, cost);
        //printf("totalIn = %d %d %d %d\n", to, totalIn, cap, cost);
    }

    serverChooseDP(graphs);
    //cout<<resultArr.size()<<endl;
    multiServerMinCost(resultArr,graphs);

    //处理找不到服务器和边界的问题
    vector<int> resultTempArr;
    for(int i = 0; i < costCount; i ++)
    {
        cout << costNeighbor[i] << endl;
        resultTempArr.push_back(costNeighbor[i]);
        int resultCostTemp = multiServerMinCost(resultTempArr, graphs);
        if(resultCostTemp != INF)
        {
            resultCostTemp += serverCost * resultTempArr.size();
            if(resultCost > resultCostTemp)
            {
                resultCost = resultCostTemp;
                resultArr = resultTempArr;
            }
        }
    }

    //for(int i = 0; i < resultArr.size(); i ++) cout << resultArr[i] << " ";
    //cout << endl;
    //cout << resultCost << endl;

    char* topo_file = print(graphs);
    write_result(topo_file, filename);
}
