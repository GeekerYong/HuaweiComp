#include "deploy.h"
#include "MCMF.h"

MCMF graphs;
int serverCost, netCount, edgeCount, costCount; //服务器代价, 网络节点个数, 边的个数, 消费节点个数
int costNeed[nMax]; //用来存储每个消费节点的带宽需求
int costNeedTotal;
int totalIn, totalOut;

int flow = 0, cost = 0; //存储流量和消费
int serverDp[2][nodeMax][2]; //服务器动态规划的数组
vector<int> serverStatus[2][nodeMax][2]; //用于存储服务器状态的数组

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
        //printf("%d %d %d %d\n", from, to, cap, cost);
    }

    //添加每个消费节点，并添加一个总汇点
    costNeedTotal = 0;
    memset(costNeed, 0, sizeof(costNeed));
    for(int i = edgeCount + 5; i < edgeCount + costCount + 5; i ++)
    {
        dealLineStr(topo[i]);
        from = str2Int(resultBuff[1]);
        to = netCount + str2Int(resultBuff[0]); //把消费节点添加到网络节点后面
        costNeed[to] = str2Int(resultBuff[2]);
        cap = INF; cost = 0;
        graphs.addEdge(from, to, cap, cost);
        //printf("%d %d %d %d\n", from, to, cap, cost);

        //添加一个总的汇点
        totalIn = netCount + costCount;
        cap = costNeed[to];
        costNeedTotal += costNeed[to];
        graphs.addEdge(to, totalIn, cap, cost);
        //printf("totalIn = %d %d %d %d\n", to, totalIn, cap, cost);
    }
}

//用来处理服务器选址，暴力枚举
void serverChooseDP(int i)
{
    memset(serverDp, INF, sizeof(serverDp));
    for(int i = 0; i < netCount; i ++)
    {
        for(int j = 0; j < costCount; j ++)
        {

        }
    }
}

//计算多源多汇最短路径
void multiServerMinCost(vector<int> serverNodes)
{
    MCMF temp = graphs;
    totalOut = netCount + costCount + 1;

    //更新服务器周边对应的边的权值
    for(int i = 0; i < serverNodes.size(); i ++)
    {
        //添加超级源点和服务器之间的边
        int from = totalOut; int to = serverNodes[i];
        int cap = INF; int cost = 0;
        temp.addEdge(from, to, cap, cost);
    }

    //每次都要初始化flow和cost
    flow = cost = 0;
    minCost(temp, totalOut, totalIn, costNeedTotal);
    printf("%d %d\n", flow, cost);
}

void minCost(MCMF temp, int s, int t, int costNeed)
{
    while(temp.bellManFord(s, t, flow, cost, costNeed));
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    init(topo, line_num);

    vector<int> test;
    test.push_back(0);
    test.push_back(1);
    multiServerMinCost(test);

	// 需要输出的内容
	char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);

}
