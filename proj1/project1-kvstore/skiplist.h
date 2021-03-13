#pragma once
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include "tablenode.h"
using namespace std;
#define lengthOfData 8


/* 节点元素 */
struct listnode {
    struct myNode {
    uint64_t key;
    string value;

    myNode(long k,string v) {key=k;value=v;}
    ~myNode(){}
    };

    listnode(long key=-1,string val="") :mynode(key,val), up(nullptr), down(nullptr), left(nullptr), right(nullptr) {}
    listnode(myNode n) :mynode(n), up(nullptr), down(nullptr), left(nullptr), right(nullptr) {}
    myNode mynode;
    // 设置4个方向上的指针
    struct listnode* up; // 上
    struct listnode* down; // 下
    struct listnode* left; // 左
    struct listnode* right; // 右

};


/* 简单跳跃表，它允许简单的插入和删除元素，并提供O(logn)的查询时间复杂度。 */
/*
    SkipList_Int的性质
    (1) 由很多层结构组成，level是通过一定的概率随机产生的，基本是50%的产生几率。
    (2) 每一层都是一个有序的链表，默认是升序，每一层的链表头作为跳点。
    (3) 最底层(Level 1)的链表包含所有元素。
    (4) 如果一个元素出现在Level i 的链表中，则它在Level i 之下的链表也都会出现。
    (5) 每个节点包含四个指针，但有可能为nullptr。
    (6) 每一层链表横向为单向连接，纵向为双向连接。
*/
// Simple SkipList_Int 表头始终是列表最小的节点
class SkipList {
private:
    
private:
    listnode* head; // 头节点，查询起始点，在最高层
    listnode* bottom;//在对底层
    int lvl_num; // 当前链表层数
    /* 随机判断 */
    bool randomVal();
    uint64_t numOfNode;//mentable最底层中node的数量（用来计算offset占的大小）
	uint64_t sizeOfKVInSs;//memtable转化为sstable后的大小（不包括offset）
    list<mem_searchtablenode> mem_searchtable;//在memtable中暂时储存key、value、offset，会转移到sstable中

public:
    SkipList(): lvl_num(1) {
        head = new listnode();
        bottom=head;
        numOfNode=1;
        sizeOfKVInSs=8;//末尾有一个key=long_max,value=""的空节点
    }
   
    uint64_t getNumOfNode(){return numOfNode;}
    uint64_t getSizeOfKVInSs(){return sizeOfKVInSs;}
    list<mem_searchtablenode>& get_mem_searchtable(){return mem_searchtable;}//传引用
    
    void reset();
    /* 插入新元素 */
    void insert(listnode n);
    /* 查询元素 */
    bool isExist(uint64_t k);//节点value存入s中
    /* 获得元素 */
    string getvalue(uint64_t k);
    /* 删除元素 */ 
    void remove(uint64_t k);

    /* 获得memtable的索引部分元素 */
    void getSearchTable();
    /* 向sstable中写入*/
    void writeToSstable(string file);

    uint64_t get_size();

    void printall();
};