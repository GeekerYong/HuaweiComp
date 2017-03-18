#include "deploy.h"
#include "MCMF.h"

MCMF graphs;
int serverCost, netCount, edgeCount, costCount; //服务器代价, 网络节点个数, 边的个数, 消费节点个数
int costNeed[nMax]; //用来存储每个消费节点的带宽需求
int costNeedTotal;
int totalIn, totalOut;

int flow = 0, cost = 0; //用来存储全局的流量和消费
int maxTempFlow = 0, maxTempFlowNum = -1; //存储流量和消费
int minTempCost = INF, minTempCostNum = -1;

int resultCost = INF; //存放花费最终结果
vector<int> resultArr; //存放最终的数组

int serverDp[nodeMax][nodeMax]; //服务器动态规划的数组
vector<int> serverStatus[nodeMax][nodeMax]; //用于存储服务器状态的数组

//处理每一行的字符串，用全局变量resultBuff来存放结果
char** resultBuff = new char*;
void dealLineStr(char* lineStr)
{
    int numCount = 0;

    char* p = NULL;
    char* str = lineStr;
    p = strtok(str, splitStr);
    while(p != NULL)
    {
        numCount ++;
        resultBuff[numCount - 1] = p;
        p = strtok(NULL, splitStr);
    }
}

//将读取的字符串转化为数字
int str2Int(char* str)
{
    int temp;
    sscanf(str, "%d", &temp);
    return temp;
}

//初始化函数
void init(char* topo[MAX_EDGE_NUM], int line_num)
{
    //处理第一行的数据
    dealLineStr(topo[0]);
    netCount = str2Int(resultBuff[0]);
    edgeCount = str2Int(resultBuff[1]);
    costCount = str2Int(resultBuff[2]);
    graphs.init(netCount + costCount + 2);

    //处理第三行的数据
    dealLineStr(topo[2]);
    serverCost = str2Int(resultBuff[0]);

    //将每条边添加到MCMF结构中
    int from, to, cap, cost;
    for(int i = 4; i < edgeCount + 4; i ++)
    {
        dealLineStr(topo[i]);
        from = str2Int(resultBuff[0]);
        to = str2Int(resultBuff[1]);
        cap = str2Int(resultBuff[2]);
        cost = str2Int(resultBuff[3]);
        graphs.addEdge(from, to, cap, cost);
    }

    //添加每个消费节点，并添加一个总汇点
    costNeedTotal = 0;
    totalIn = netCount + costCount;
    totalOut = netCount + costCount + 1;
    memset(costNeed, 0, sizeof(costNeed));
    for(int i = edgeCount + 5; i < edgeCount + costCount + 5; i ++)
    {
        dealLineStr(topo[i]);
        from = str2Int(resultBuff[1]);
        to = netCount + str2Int(resultBuff[0]); //把消费节点添加到网络节点后面
        costNeed[to] = str2Int(resultBuff[2]);
        cap = INF; cost = 0;
        graphs.addEdge(from, to, cap, cost);

        //添加一个总的汇点
        cap = costNeed[to];
        costNeedTotal += costNeed[to];
        graphs.addEdge(to, totalIn, cap, cost);
        //printf("totalIn = %d %d %d %d\n", to, totalIn, cap, cost);
    }
}

//判断是否重复
bool isRepeat(vector<int> temp, int a)
{
    for(int i = 0; i < temp.size(); i ++)
    {
        if(temp[i] == a)
            return true;
    }
    return false;
}

//用来处理服务器选址，暴力枚举
void serverChooseDP()
{
    memset(serverDp, INF, sizeof(int));
    for(int i = 0; i < costCount; i ++)
        for(int j = 0; j < netCount; j ++)
            serverDp[i][j] = INF;

    int tempCost, tempTotal;
    for(int i = 0; i < netCount; i ++) //初始化第一行的数据
    {
        serverStatus[0][i].push_back(i);
        tempCost = multiServerMinCost(serverStatus[0][i]);

        if(tempCost == INF) serverDp[0][i] = INF;
        else
        {
            serverDp[0][i] = cost + serverCost * serverStatus[0][i].size();
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
            maxTempFlow = 0, maxTempFlowNum = -1;
            //minTempCost = INF, minTempCostNum = -1;
            for(int k = 0; k < netCount; k ++) //对i-1层每个节点进行枚举
            {
                isRe = isRepeat(serverStatus[i - 1][k], j);
                if(isRe == true) continue;

                tempArr = serverStatus[i - 1][k];
                tempArr.push_back(j);
                tempCost = multiServerMinCost(tempArr);
                //若不存在路径，挑选流量最大的K来作为DP状态数组的更新,这个部分可以修改
                if(tempCost == INF)
                {
                    serverDp[i][j] = INF;
                    if(maxTempFlow < flow)
                    {
                        maxTempFlow = flow;
                        maxTempFlowNum = k;
                    }
                }
                else
                {
                    //若存在路径，挑选总花费最小的服务器节点
                    tempTotal = tempCost + serverCost * tempArr.size();
                    if(tempTotal < serverDp[i][j])
                    {
                        serverDp[i][j] = tempTotal;
                        serverStatus[i][j] = tempArr;
                    }
                }
            }
            //选择最小消费的路径
            if(serverDp[i][j] == INF)
            {
                serverStatus[i][j] = serverStatus[i-1][maxTempFlowNum];
                serverStatus[i][j].push_back(j);
            }
            //更新最终结果
            if(serverDp[i][j] < resultCost)
            {
                resultCost = serverDp[i][j];
                resultArr = serverStatus[i][j];
            }
        }
    }
    //对最后一层的节点进行汇总
    for(int i = 0; i < netCount; i ++)
    {
        if(serverDp[costCount - 1][i] < resultCost)
        {
            resultCost = serverDp[costCount - 1][i];
            resultArr = serverStatus[costCount - 1][i];
        }
    }
}

//计算多源多汇最短路径
int multiServerMinCost(vector<int> serverNodes)
{
    MCMF temp = graphs;
    //更新服务器周边对应的边的权值
    for(int i = 0; i < serverNodes.size(); i ++)
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

char* print()
{
    char* topo = (char*)malloc(maxLineNum);
    char* strTemp = (char*)malloc(maxLineNum - 10);

    //更新服务器周边对应的边的权值
    for(int i = 0; i < resultArr.size(); i ++)
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
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    init(topo, line_num);

    serverChooseDP();
    multiServerMinCost(resultArr);
//    for(int i = 0; i < resultArr.size(); i ++)
//        cout << resultArr[i] << " ";
//    cout << endl;
//    printf("%d\n", resultCost);

//    resultCost = 2408;
//    resultArr.push_back(38);
//    resultArr.push_back(3);
//    resultArr.push_back(15);
//    resultArr.push_back(0);
//    resultArr.push_back(43);
//    resultArr.push_back(22);

    char* topo_file = print();

	//需要输出的内容
	//char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	//直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
}
