#pragma once
#include <iostream>
#include <string>
using namespace std;

struct mem_searchtablenode//在memtable中暂时储存key、value、offset的
{    
    uint64_t key;
    string value;
    uint64_t offset;
    mem_searchtablenode(uint64_t k,string v,uint64_t o):key(k),value(v),offset(o){}
    ~mem_searchtablenode(){}
    bool operator < (mem_searchtablenode & b) {
       return key < b.key;//升序排列
    }
};

struct searchtablenode//在sstable中储存key、offset
{
    uint64_t key;
    uint64_t offset;
    searchtablenode(uint64_t k,uint64_t o):key(k),offset(o){}
    ~searchtablenode(){}
};

