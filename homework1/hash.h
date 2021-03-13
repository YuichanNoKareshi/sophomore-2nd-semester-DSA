#ifndef _HASH_H
#define _HASH_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <vector>
using namespace std;

class hashtable
{
private:
    int m;//格子个数
    int *table1;//hashtable
    int *table2;
    int *table3;
    int hashA (int key);
    int hashB (int key);
    int hashC (int key);
    int hashD (int key);
    

public:
    hashtable (int m);
    ~hashtable ();
    void hash (string filename,int n);
    void reset();
    
    double rate1;//第一种方法的假阳性率
    double rate2;//第二种的假阳性率

    
};



void hashtable::reset()
{
    for (int i=0;i<m;i++)
    {
        table1[i]=0;//全部置0
        table2[i]=0;
    }
    for (int i=0;i<2*m;i++) table3[i]=0;
    cout<<"reset successfully"<<endl;
}

hashtable::hashtable(int m)
{
    this->m=m;
    this->table1 = new int[m];
    this->table2 = new int[m];
    this->table3 = new int[2*m];
    this->reset();

}



hashtable::~hashtable ()
{
    delete table1;
    delete table2;
    delete table3;
}

int hashtable::hashA (int key)//直接取余
{

    return key;

}

int hashtable::hashB (int key)//各位加起来
{

    if (key<0) return false;
    stringstream ss;
    string str;
    ss<<key;
    ss>>str;

    int hash = 0;
    int i;
    for (i=0; i<str.length(); ++i) hash = 33*hash + str.at(i)-'0';

    return hash;
}

int hashtable::hashC (int key)//各位乘起来
{
    if (key<0) return false;
    stringstream ss;
    string str;
    ss<<key;
    ss>>str;

    int hash=str.length();
    int i;
    for (i=0; i<str.length(); ++i) hash = hash * (str.at(i)-'0');

    return hash;

}

int hashtable::hashD (int key)//平方取中
{
    if (key<0) return false;
    key = key * key;
    int hash;
    if (key<=m*10) hash=key;
    else hash=(key/10);

    return hash;
}


void hashtable::hash(string filename,int n)
{
    fstream file;
    file.open(filename.c_str(), ios::in);

    vector<int> v1;//v1储存n个数据
    vector<int> v2;//v2储存测试假阳性的数据
    string tmp;
    int temp;
    int i=0;

    while (file >> tmp)
    {
        i++;
        temp = atoi(tmp.c_str());//string -> int

        if (i <= n) v1.push_back(temp);
        else v2.push_back(temp);

        
    }

    for (auto x : v1)
    {
        int result1,result2,result3;

        int hash_resultA=hashA(x);
        result1=hash_resultA%m;
        result3=hash_resultA%(2*m);
        table1[result1]=1;
        table3[result3]=1;

        int hash_resultB=hashB(x);
        result1=hash_resultA%m;
        result3=hash_resultA%(2*m);
        table1[result1]=1;
        table3[result3]=1;

        int hash_resultC=hashC(x);
        result2=hash_resultA%m;
        result3=hash_resultA%(2*m);
        table2[result2]=1;
        table3[result3]=1;

        int hash_resultD=hashD(x);
        result2=hash_resultA%m;
        result3=hash_resultA%(2*m);
        table2[result2]=1;
        table3[result3]=1;
    }




    int lengthof_v2=0;
    int fault_num1=0;
    int fault_num2=0;
    for (auto y : v2)
    {
        lengthof_v2++;
        int hash_resulta=hashA(y);
        int hash_resultb=hashB(y);
        int hash_resultc=hashC(y);
        int hash_resultd=hashD(y);
        if (table1[hash_resulta%m]==1 && table1[hash_resultb%m]==1 && table2[hash_resultc%m]==1 && table2[hash_resultd%m]==1)
            fault_num1++;
        if (table3[hash_resulta%(2*m)]==1 && table3[hash_resultb%(2*m)]==1 && table3[hash_resultc%(2*m)]==1 && table3[hash_resultd%(2*m)]==1)
            fault_num2++;

    }

    cout<<lengthof_v2<<"    "<<fault_num1<<"    "<<fault_num2<<endl;
    this->rate1=(double) fault_num1/lengthof_v2;
    this->rate2=(double) fault_num2/lengthof_v2;


}



#endif