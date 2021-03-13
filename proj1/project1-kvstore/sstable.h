#pragma once
#include <iostream>
#include <list>
#include <filesystem>
#include "skiplist.h"
#include <string>
#include "tablenode.h"
using namespace std;
using namespace std::filesystem;



class sstable
{
private: 
    string filename;
    uint64_t id;
    list<searchtablenode> searchtable;//用来储存sstable的索引部分key、offset
    

public:
    sstable(string fname,int idd);
    ~sstable(){}
    string get_filename(){return filename;}
    uint64_t get_id(){return id;}
    list<searchtablenode>& get_searchtable(){return searchtable;}//传引用
    void setsearchtable(list<mem_searchtablenode> &mem_stable);//来自memtable的searchtable
    void setsearchtable();//来自自己的文件
    void sort_setSstable(list<mem_searchtablenode> &temp_table,uint64_t sizeOfKVInSs);
};