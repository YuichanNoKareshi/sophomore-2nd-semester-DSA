#include <climits>
#include <fstream>
#include <cstdio>
#include <stack>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const int maxn = 500;

int node_num;
int side_num;

struct node {
	int end;
	int weight;

};

vector<node> adj[maxn];//邻接表存储DAG图
int outnode[maxn] = { 0 };//记录每个顶点的出度
int dp[maxn];//dp为某点到达出度为0的点的最长路径

void init() {//dp数组初始化
	fill(dp, dp + maxn, -1);
	for (int i = 0; i < node_num; i++) {   //初始化出度为0的dp为0
		if (outnode[i] == 0) {
			dp[i] = 0;
		}
	}
}


int path[maxn];//存放DAG最长路路径序列

int DFS(int from) {     //递归求解最长路径
	if (dp[from] == 0) return dp[from];//递归结束
	for (int i = 0; i < adj[from].size(); i++) {
		int diste = DFS(adj[from][i].end); //得到到达下个节点到达出度为0点的最长路径
		if (dp[from] < diste + adj[from][i].weight) { //更新最长路径
			dp[from] = diste + adj[from][i].weight;
			path[from] = adj[from][i].end; //from的后继结点是e(和Dijkstra算法记录前驱结点的思路差不多)
		}
	}
	return dp[from];
}

int main() {

    string filename="data.in";
    fstream fin;
    fin.open(filename.c_str(), ios::in);
    string tmp;//用来接受data.in的输入
    fin>>tmp;
    int node_num = atoi(tmp.c_str());//得到节点数目
    fin>>tmp;
    int side_num = atoi(tmp.c_str());//得到边的数目

    int start, end, weight;
    string s,e,w;
    while (fin>>s>>e>>w)//给图中每一边赋值
    {
        start=atoi(s.c_str());
        end=atoi(e.c_str());
        weight=atoi(w.c_str());
        node N;
		N.end = end;
		N.weight = weight;
		outnode[start]++;//更新结点的出度
		adj[start].push_back(N);
    }
	
	init();//初始化
	for (int i = 0; i < node_num; i++) {//初始化每个结点的后继结点为自身
		path[i]=i;
	}
	
	int maxdp = DFS(1);//记录最大的dp[i]的值
    cout << maxdp << "\n";
    int j = 1;
    while (path[j] != j) {//依次输出path中元素(每个节点的后继节点)
		cout<<j<<' ';
        j = path[j];
    }
    cout<<j;

	return 0;

}
