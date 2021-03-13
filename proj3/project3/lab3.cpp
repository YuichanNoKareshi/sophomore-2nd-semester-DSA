#include "common.h"
#include <algorithm>
using namespace std;

//You should only code here.Don't edit any other files in this 
int func1(int amount, vector<int>& coins)
{	
	int coins_size=coins.size();

	int** graph=new int*[2];//size个coin
	for (int i=0;i<2;i++)
	{
		graph[i]=new int[amount+1];
		for (int j=0;j<=amount;j++)
			graph[i][j]=0;
	}
	graph[0][0]=1;
	//2*(amount+1)矩阵，初始化结束
	
	for (int i=0; i<coins_size; i++)//对每一行
	{
		for (int j=0; j<=amount; j++)
		{
			int contain_num = j / coins[i];//j中包含多少个coins[i]
			for (int k=0;k<=contain_num;k++)
				graph[1][j] += graph[0][j-k*coins[i]];
		}

		for (int j=0; j<amount; j++)//调整矩阵的两行
		{
			graph[0][j]=graph[1][j];
			graph[1][j]=0;
		}
	}

	int result=graph[1][amount];
	for (int i=0;i<2;i++)
		delete [] graph[i];
	delete [] graph;

	return result;

}

int op(int i,int plus,int amount)
{
	return (i+plus+amount)%amount;
}

int func2(int amount, vector< vector<int> >& conquer)
{
	bool **graph=new bool* [amount];
	for (int i=0;i<amount;i++)
	{
		graph[i]=new bool [amount];
		for (int j=0;j<amount;j++)
			graph[i][j]=false;
		
		graph[i][op(i,1,amount)]=true;
	}
	//初始化完成

	for (int len1=2;len1<=amount;len1++)//从各自的下下个人开始
	{
		for (int i=0;i<amount;i++)//从第一个人开始依次判断
		{
			int player=op(i,len1,amount);
			//下面判断i和player是否能交手

			if (graph[i][player]==false) 
				for (int len2=1;len2<len1;len2++)
				{
					int temp_player=op(i,len2,amount);//工具人

					if(graph[i][temp_player]==true 
						&& graph[temp_player][player]==true 
						&& (conquer[i][temp_player]==1 || conquer[player][temp_player]==1) )
					{
						graph[i][player]=true;
						break;
					}
				}
		}
	}
	
	int count=0;
	for (int i=0;i<amount;i++)
		if (graph[i][i]==true) count++;

	for (int i=0;i<amount;i++)
		delete [] graph[i];
	delete [] graph;
	return count;
}

void gaussin(int n, double** a, double*& b, double** c)
{

    int i, j, k;
    //消元的整个过程如下，总共n-1次消元过程。
    for (k = 0; k < n - 1; k++)
    {
        //第K次的消元计算
        for (i = k + 1; i < n; i++)
        {
            b[i] = b[i] - c[k][i] * b[k];
        }
    }

	//解的存储数组
    double* x=new double [n];
    //先计算出最后一个未知数；
    x[n - 1] = b[n - 1] / a[n - 1][n - 1];
    //求出每个未知数的值
    for (i = n - 2; i >= 0; i--)
    {
        double sum = 0;
        for (j = i + 1; j < n; j++)
        {
            sum += a[i][j] * x[j];
        }
        x[i] = (b[i] - sum) / a[i][i];
    }

	for (int i=0;i<n;i++)
		b[i]=x[i];
	delete [] x;
	return ;

}

double func3(int n,int hp,vector<int>& damage,vector<int>& edges) 
{	
	int** temp_g=new int* [n+1];
	int i,j,k,l;
	for (i=0;i<n+1;i++)
	{
		temp_g[i]=new int [n+1];
		for (j=0;j<n+1;j++)
			temp_g[i][j]=0;
	}
	int edge_size=edges.size();
	for (i=0;i<edge_size;i+=2)
	{
		int p=edges[i];
		int q=edges[i+1];
		temp_g[p][q]=1;
		temp_g[q][p]=1;
	}
	
    vector< vector<int> > graph;
	vector<int> v0;//废弃的v0
	graph.push_back(v0);
	for (i=1;i<n+1;i++)//从第1个路口开始
	{
		vector<int> temp_v;//临时vector
		for (j=1;j<n+1;j++)//查看i能到的所有路口j
		{		
			if (temp_g[i][j]==1)
				temp_v.push_back(j);
		}
		graph.push_back(temp_v);
	}
	
	for (i=0;i<n+1;i++)
		delete [] temp_g[i];
	delete [] temp_g;
	//初始化结束，graph[0]废弃，graph[i]记录i能到的点的集合，graph[i].size()即为个数

	double** f=new double* [hp+1];
	for (i=0;i<hp+1;i++)
	{
		f[i]=new double [n+1];
		for (j=0;j<n+1;j++)
			f[i][j]=0;
	}
	//f矩阵记录每个点的经过次数期望,第0行和第0列废弃

	int d_size=damage.size();
	vector<int> safe_node;
	for (i=0;i<d_size;i++)
		if(damage[i]==0) safe_node.push_back(i+1);
	int num=safe_node.size();
	//safe_node记录哪些路口damage=0，num记录damage为0的路口的个数

	double** a=new double* [num];
	double* b=new double [num];
	for (i=0;i<num;i++)
		a[i]=new double [num];
	int line=0;
	//a是高斯消元的等号左边，b是高斯消元的等号右边，line记录填到a和b的第几行了

	for (j=1;j<=n;j++)//高斯消元法更新damage为0的f(i,j)
	{
		if (damage[j-1]==0)
		{
			for(k=1;k<=n;k++)
			{
				vector<int>::iterator iter=find(safe_node.begin(),safe_node.end(),k);
				int index=0;
				for (auto i=safe_node.begin();i!=iter;i++)
					index++;//index是k在safe_node中的索引，index记录k是safe_node中第几个元素
				if (k==n && k!=j) {a[line][index]=0.0;break;}
				if (k==j) 
					a[line][index]=1.0;
				else if (iter!=safe_node.end())//k在safe_num中
				{
					if (find(graph[j].begin(),graph[j].end(),k)==graph[j].end()) a[line][index]=0.0;//如果k不能到达j
					else { double g_size=graph[k].size(); a[line][index]=(1.0/g_size*(-1.0));}
				}
			}
			line++;
		}
	}//确定系数矩阵a

	double** c=new double* [num];
	for(i=0;i<num;i++)
	{
		c[i]=new double [num];
	}
	for (k = 0; k < num - 1; k++)
    {
        //求出第K次初等行变换的系数
        for (i = k + 1; i < num; i++)
            c[k][i] = a[i][k] / a[k][k];

        //第K次的消元计算
        for (i = k + 1; i < num; i++)
        {
            for (j = 0; j < num; j++)
            {
                a[i][j] = a[i][j] - c[k][i] * a[k][j];
            }
        }
    }//确定c

	for (i=hp;i>=0;i--)
	{
		for (j=1;j<=n;j++)//更新damage不为0的f(i,j)
		{
			if (i+damage[j-1]>hp || (i==0 && damage[j-1]==0)) {f[i][j]=0;continue;}
			if (damage[j-1]!=0)//先用一遍循环把所有damage不为0的点算出来
			{
				int j_size=graph[j].size();
				for(k=0;k<j_size;k++)
				{
					if (graph[j][k]==n) continue;
					double g_size=graph[graph[j][k]].size();
					f[i][j] += (f[ i+damage[j-1] ][ graph[j][k] ]/g_size);
				}
			}
		}

		if(i==0) break;//如果hp=0，就不用再算damage=0的点了

		for (l=0;l<num;l++)
			b[l]=0;
		line=0;//把b和line全部置零
		for (j=1;j<=n;j++)//高斯消元法更新damage为0的f(i,j)
		{
			if (damage[j-1]==0)
			{
				if(i==hp && j==1) //如果是f[1][1]
					b[line]+=1.0;
				for(k=1;k<=n;k++)
				{
					vector<int>::iterator iter=find(safe_node.begin(),safe_node.end(),k);
					int index=0;
					for (auto i=safe_node.begin();i!=iter;i++)
						index++;//index是k在safe_node中的索引，index记录k是safe_node中第几个元素
					if (k==n && k!=j) break;
					if (k==j || iter!=safe_node.end()) 
						continue;
					else if (find(graph[j].begin(),graph[j].end(),k)!=graph[j].end()) //k不在safe_num中，且k能到达j
					{ double g_size=graph[k].size(); b[line] += f[i][k]/g_size;}
				}
				line++;
			}
		}
		
		gaussin(num,a,b,c);
		for (k=0;k<num;k++)
			f[i][safe_node[k]]=b[k];
	}

	double res=0;
	for (i=0;i<hp+1;i++)
		res += f[i][n];

	delete [] b;
	delete [] c;
	
	for (i=0;i<num;i++)
		delete [] a[i];
	delete [] a;
	

	for (i=0;i<hp+1;i++)
		delete [] f[i];
	delete [] f;

    return res;
}


