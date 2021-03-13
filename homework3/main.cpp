#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include "map.h"
using namespace std;


int main ()
{
    int N=1000000;

    uniform_int_distribution<unsigned> u(0,100000000);
    default_random_engine e(time(0));
    
    RBTree<uint64_t>* treee=new RBTree<uint64_t>;
        
    treee=new RBTree<uint64_t>() ;
    int node_num=1;
    while (node_num<=N) 
    {
            
        int x= u(e);
        treee->insert(x);
        node_num++;
    }//节点插入完成
    
    // cout<<tree1[i]->preOrder()<<endl;
    // if (i%10==9) cout<<endl;    
        
    
    
    node_num=0;//重新插入N/100个
    int solveDoubleRed=0;
    clock_t start = clock();
    while (node_num<=N/100) 
    {
            
        int x= u(e);
        solveDoubleRed+=treee->insert(x);
        node_num++;
    }
    clock_t end = clock();

    cout << "const " << (double)(end - start) / CLOCKS_PER_SEC << " second" << endl;
    cout << "height of tree: "<<treee->getTreeHeight() <<endl;
    cout << "double red: "<<solveDoubleRed <<endl;
    return 0;
}

