#include "Dijkstra.h"
using namespace std;

/**
 * You should implement the methods:`readFromFile`,`run1`and`run2` here.
 */
void DijkstraProject2::readFromFile(const char* inputfile)
{
	cout << "readFromFile: " << inputfile << endl;
	//TODO
	string filename=inputfile;
    fstream fin;
    fin.open(filename.c_str(), ios::in);
	
	node_nums.clear();
	side_nums.clear();
	graphs.clear();
	Ss.clear();
	Us.clear();
	string isEOF;
	do
	{
		readfile(fin);
	}
	while(getline(fin,isEOF));//读掉空行
    
	fin.close();
}

void DijkstraProject2::readfile(fstream &fin)
{
	string tmp;//用来接受data.in的输入
	getline(fin,tmp,',');
	int node_num0 = atoi(tmp.c_str());//得到节点数目
	getline(fin,tmp);
    int side_num0 = atoi(tmp.c_str());//得到边的数目

	int** graph0=new int*[node_num0];
	Unode* U0=new Unode[node_num0];
	for (int i=0;i<node_num0;++i)
	{	
		U0[i].weight=INT_MAX;
		U0[i].isvisited=false;
		graph0[i]=new int [node_num0];
		for (int j=0;j<node_num0;++j)
			graph0[i][j]=-1;
	}
	Snode temp(0,0);//0到0的最短距离为0
	vector<Snode> S0;
	S0.push_back(temp);
	U0[0].isvisited=true;
	
    int start, end, weight;
    string s,w,e;
	int num=0;
    while (getline(fin,tmp))//给图中每一边赋值
    {
		divide_string(tmp,s,e,w);
        start=atoi(s.c_str());
        end=atoi(e.c_str());
        weight=atoi(w.c_str());

		graph0[start][end]=weight;
		num++;
		if (num==side_num0)
			break;
    }
	node_nums.push_back(node_num0);
	side_nums.push_back(side_num0);
	graphs.push_back(graph0);
	Ss.push_back(S0);
	Us.push_back(U0);

	return;
}

void DijkstraProject2::run(const char* outputFile)
{
	for (int i=0;i<graphs.size();++i)
	{
		node_num=node_nums[i];
		side_num=side_nums[i];
		graph=graphs[i];
		S=Ss[i];
		U=Us[i];
		run1();
		run2();
	}
}

void DijkstraProject2::run1(const char* outputFile)
{
	//TODO
	getUandS();
	sort(S.begin(), S.end());

	min_road.clear();
	string s="";
	doRun1(s,node_num-1);

	int min_road_length=S[node_num-1].weight;
	int min_road_num=min_road.size();
	string first_line=std::to_string(min_road_length);
	string second_line=std::to_string(min_road_num);

	string filename=outputFile;
	fstream fout;
	fout.open(filename, ios::app);
	fout<<first_line<<endl;
	fout<<second_line<<endl;
	for (int i=0;i<min_road.size();++i)
		fout<<min_road[i]<<endl;
	fout<<endl;

	fout.close();
	cout << "Save result to file:" << outputFile << endl;
}

void DijkstraProject2::doRun1(string s, int child)
{
	s = ',' + s;
	s = std::to_string(child) + s;//先把当前节点也加入路径中

	if (child==0)//停止递归的条件
	{
		s=s.substr(0, s.length() -1);//删除掉最后的一个','
		min_road.push_back(s);
		return;
	}

	for (int i=0;i<S[child].parent.size();++i)//遍历child的每个可能的父节点
	{
		child=S[child].parent[i];
		doRun1(s,child);
	}
}

void DijkstraProject2::run2(const char* outputFile)
{
	//TODO

	mono_min_road.clear();
	string s="";
	doRun2_decreasing(s,node_num-1,0,0);
	doRun2_increasing(s,node_num-1,INT_MAX,0);
	sort(mono_min_road.begin(),mono_min_road.end());

	int min_road_length = (mono_min_road.size()>0) ? mono_min_road[0].weight : -1;
	for(auto it=mono_min_road.begin();it!=mono_min_road.end();it++)
	{
		if(it->weight > min_road_length)
		{
			it = mono_min_road.erase(it);
			it--;
		}
	}
	int min_road_num=mono_min_road.size();

	string first_line=std::to_string(min_road_length);
	string second_line=std::to_string(min_road_num);

	string filename=outputFile;
	fstream fout;
	fout.open(filename, ios::app);
	fout<<first_line<<endl;
	fout<<second_line<<endl;
	for (int i=0;i<mono_min_road.size();++i)
		fout<<mono_min_road[i].s<<endl;
	fout<<"end"<<endl;
	fout<<endl;
	fout.close();

	cout << "Save result to file:" << outputFile << endl;
}

void DijkstraProject2::doRun2_decreasing(string s,int child,int pre_weight,int tot_weight)
{
	s = ',' + s;
	s = std::to_string(child) + s;//先把当前节点也加入路径中

	if (child==0)//停止递归的条件
	{
		s=s.substr(0, s.length() -1);//删除掉最后的一个','
		MONOnode temp(s,tot_weight);
		mono_min_road.push_back(temp);
		return;
	}

	for (int i=0;i<node_num;++i)//遍历graph的第child列
	{
		if ( graph[i][child]!=-1 && graph[i][child]>pre_weight 
			&&( s.find(to_string(i)+',')==string::npos || s.at(s.find(to_string(i)+',')-1)!=',') )//存在且递减且没走过
		{
			doRun2_decreasing(s,i,graph[i][child],tot_weight+graph[i][child]);
		}
			
	}
}

void DijkstraProject2::doRun2_increasing(string s,int child,int pre_weight,int tot_weight)
{
	s = ',' + s;
	s = std::to_string(child) + s;//先把当前节点也加入路径中

	if (child==0)//停止递归的条件
	{
		s=s.substr(0, s.length() -1);//删除掉最后的一个','
		MONOnode temp(s,tot_weight);
		mono_min_road.push_back(temp);
		return;
	}

	for (int i=0;i<node_num;++i)//遍历graph的第child列
	{
		if ( graph[i][child]!=-1 && graph[i][child]<pre_weight 
			&&( s.find(to_string(i)+',')==string::npos || s.at(s.find(to_string(i)+',')-1)!=',') )//存在且递增且没走过
		{
			doRun2_increasing(s,i,graph[i][child],tot_weight+graph[i][child]);
		}
			
	}
}

void DijkstraProject2::divide_string(string tmp, string &s, string &e, string &w)
{
	s="";
	w="";
	e="";
	int index=1;//1表示start，2表示weight，3表示end
	for (int i=0;i<tmp.size();++i)
	{
		switch(index)
		{
		case 1:
			if (tmp.at(i)!=',')
				s += tmp.at(i);
			else index++;
			break; 
		case 2:
			if (tmp.at(i)!=',')
				e += tmp.at(i);
			else index++;
			break; 
		case 3:
			if (tmp.at(i)!='\0' && tmp.at(i)!='\n')
				w += tmp.at(i);
			else index++;
			break; 
		default: break;
		}
		
	}
}

void DijkstraProject2::getUandS()//遍历所有节点，把所有节点加入S中
{
    while (S.size() < node_num)
	{
		updateU();

		//确定要从U加入S的节点
		int j=0;
		int min_wei=INT_MAX;//最短长度
		int min_U=0;//最短长度对应的U
		while(j<node_num)//遍历所有U中节点
		{
			if(U[j].isvisited==true) {j++;continue;}
			bool isreplace= min_wei > U[j].weight ;
			min_wei = isreplace ? U[j].weight : min_wei;
			min_U = isreplace ? j : min_U;
			j++;
		}

		//将min_U从U加入S中
		Snode temp(min_U,min_wei);
		temp.parent.assign(U[min_U].parent.begin(),U[min_U].parent.end());
		S.push_back(temp);
		U[min_U].isvisited=true;
	}
}

void DijkstraProject2::updateU()//更新U中节点的距离为：只经过S中节点的最短距离
{
	for (int i=0;i<node_num;++i)
		U[i].parent.clear();//清空U中所有节点的parent容器
	
	int j=0;
	while(j<node_num)//遍历所有U中节点
	{
		if(U[j].isvisited==true) {j++;continue;}
		int min_weight=INT_MAX;//最短长度
		for (int i=0;i<node_num;++i)
		{
			if(U[i].isvisited==true)//遍历每个已在S中的节点，判断哪个是j的父节点
			{
				bool isreplace= (min_weight > graph[i][j]+S_i_weight(i) && graph[i][j]!=-1);//记录i到j的距离是不是目前为止最小的
				min_weight = isreplace ? (graph[i][j]+S_i_weight(i)) : min_weight;
			}//此时S中i并不是有序排的，S[i]只是第i个，并不一定是节点i
		}	
		U[j].weight=min_weight;
		for (int i=0;i<node_num;++i)
		{
			if(U[i].isvisited==true && min_weight == graph[i][j]+S_i_weight(i) && graph[i][j]!=-1)//记录所有可能的parent
				U[j].parent.push_back(i);
		}	

		j++;
	}
}

int DijkstraProject2::S_i_weight(int i)
{
	for (int j=0;j<S.size();j++)
		if (S[j].end==i) return S[j].weight;
	return -1;
}
