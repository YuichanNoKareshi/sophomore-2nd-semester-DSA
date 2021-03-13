#pragma once
#include "skiplist.h"
#include "kvstore_api.h"
#include "sstable.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <list>
#include <string>
#include <io.h>

#define threshold 2097152 //2m对应2097152字节
using namespace std;
using namespace std::filesystem;


class KVStore : public KVStoreAPI {
	// You can add your implementation here
private:
    string dir;
    SkipList *memtable;
	vector<vector<sstable>> disk;//邻接表模拟disk的结构
	list<mem_searchtablenode> sort_table;//合并compaction时用来排序的容器
	vector<sstable> sort_disk;//合并时用来暂时存需要合并的sstable的容器
public:
	KVStore(const string &dir);

	~KVStore();

	void put(uint64_t key, const string &s) override;

	string get(uint64_t key) override;
	
	bool getInsstable(const uint64_t key,string &s);

	bool del(uint64_t key) override;

	void reset() override;

	int powerOf2 (int m);//2^m
    
	void compaction();//sort_disk中的sstable需要合并

	void exporting(int layer);//compaction之后导出

	bool is_sorttable_contain(uint64_t key);//查找sort_table中是否含有某个节点

	void getFiles(string path,int i);//得到某文件夹下的所有文件，i表示disk的第i层

	void printmemtable();
	
};





