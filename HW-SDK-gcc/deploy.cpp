#include "deploy.h"
#include "MCMF.h"

MCMF graphs;
int serverCost = 0; //用来存储每个服务器的代价
int costNeed[nmax]; //用来存储每个消费节点的带宽小号需求
const char* splitStr = " \n";
char** resultBuff = new char*;

//处理每一行的字符串，用全局变量resultBuff来存放结果
void dealLineStr(char* lineStr)
{
    int numCount = 0;
    memset(resultBuff, NULL, sizeof(resultBuff));

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
    int netCount, edgeCount, costCount;
    dealLineStr(topo[0]);
    netCount = str2Int(resultBuff[0]);
    edgeCount = str2Int(resultBuff[1]);
    costCount = str2Int(resultBuff[2]);
    graphs.init(netCount + costCount);
    //printf("%d %d %d\n", graphs.n, edgeCount, costCount);

    //处理第三行的数据
    dealLineStr(topo[2]);
    serverCost = str2Int(resultBuff[0]);
    //printf("%d\n", serverCost);

    //将每条边添加到MCMF结构中
    int from, to, cap, cost;
    for(int i = 4; i < edgeCount + 4; i ++)
    {
        dealLineStr(topo[i]);
        from = str2Int(resultBuff[0]);
        to = str2Int(resultBuff[1]);
        cap = str2Int(resultBuff[2]);
        cost = str2Int(resultBuff[3]);
        //printf("%d %d %d %d\n", from, to, cap, cost);
        graphs.addEdge(from, to, cap, cost);
    }

    //添加每个消费节点
    memset(costNeed, 0, sizeof(costNeed));
    for(int i = edgeCount + 5; i < edgeCount + costCount + 5; i ++)
    {
        dealLineStr(topo[i]);
        to = netCount + str2Int(resultBuff[0]); //把消费节点添加到网络节点后面
        from = str2Int(resultBuff[1]);
        cap = INF; cost = 0;
        graphs.addEdge(from, to, cap, cost);

        costNeed[to] = str2Int(resultBuff[2]);
        printf("%d %d %d\n", to, from, cap, temp);
    }
    //graphs.init();
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    init(topo, line_num);

	// 需要输出的内容
	char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);

}
