#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <fstream>
#include <climits>
#include <vector>
#include <algorithm>
using namespace std;

struct Snode
{
	int end;//end表示已知最短路径的点
	int weight;//weight表示从0出发到end的最短距离
	vector<int> parent;
	Snode(int e,int w):end(e),weight(w){}
	~Snode(){}
	bool operator<(const Snode &b) const
	{
		return this->end<b.end;
	}
};
struct Unode
{
	int weight;
	bool isvisited;//isvisited代表已确定最短路径
	vector<int> parent;
	Unode(){weight=INT_MAX;isvisited=false;}
	Unode(int w,bool i):weight(w),isvisited(i){}
	~Unode(){}
};
struct MONOnode
{
	string s;
	int weight;
	MONOnode (string _s,int w):s(_s),weight(w){}
	~MONOnode(){} 
	bool operator<(const MONOnode &b) const{return this->weight<b.weight;}
};
/**
 * This file declare the main class of Project2:DijkstraProject2.
 * You should implement the methods:`readFromFile`,`run1`and`run2` in Dijkstra.cpp.
 * You can add anything in DijkstraProject2 class to implement Project2.
 */
class DijkstraProject2 {
private:
	//You can declare your graph structure here.
	int node_num; //得到节点数目
    int side_num; //得到边的数目
	int** graph;//graph[i][j]表示从i到j的路径的权重
	vector<Snode> S;//已求出最短路径的节点集合
	Unode* U;//未确定最短路径的节点集合

	vector<int> node_nums;
	vector<int> side_nums;
	vector<int**> graphs;//图的集合
	vector<vector<Snode>> Ss;
	vector<Unode*> Us;

	vector<string> min_road;//记录最短路径
	vector<MONOnode> mono_min_road;//记录单调最短路径
public:
	DijkstraProject2(){}
	~DijkstraProject2(){}
	/**
	 * Read graph from Param:`inputfile`.
	 * 
	 */
	void readFromFile(const char* inputfile="input.txt");

	void readfile(fstream &fin);
	
	void run(const char* outputFile = "output.txt");

	/**
	 * Part 1, implement Dijkstra algorithm to finish Part 1
	 * and save the result to Param:`outputFile`.
	 * Save the path as: node_1,node_2...node_n. (seperate nodes with comma)
	 *
	 */
	void run1(const char* outputFile = "output.txt");

	void doRun1(string s, int child);//run1的递归函数

	/**
	 * Part 2, find the monotonically increasing path to finish Part 2
	 * and save the result to Param:`outputFile`.
	 * Save the path as: node_1,node_2...node_n. (seperate nodes with comma)
	 *
	 */
	void run2(const char* outputFile = "output.txt");

	void doRun2_decreasing(string s,int child,int pre_weigth,int tot_weight);//run2的递归函数

	void doRun2_increasing(string s,int child,int pre_weigth,int tot_weight);//run2的递归函数

	void divide_string(string tmp, string &s, string &e, string &w);

	void getUandS();

	void updateU();//更新U中节点路径

	int S_i_weight(int i);//查询S中从0到节点i的距离

	// bool compRun1(const Snode &a,const Snode &b)
	// {
	// 	return a.end < b.end;
	// }

	// bool compRun2(const Snode &a,const Snode &b)
	// {
	// 	return a.weight < b.weight;
	// }
};
