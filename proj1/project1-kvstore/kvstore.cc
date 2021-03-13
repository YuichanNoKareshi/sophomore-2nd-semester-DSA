#include "kvstore.h"
using namespace std;
using namespace std::filesystem;

KVStore::KVStore(const string &dir): KVStoreAPI(dir)
{
	memtable=new SkipList();
    this->dir=dir;
	//reset();
	string first=dir + "/L0";//第一次构造
	path pfirst(first);
	if (exists(pfirst)==false) 
	{
		create_directories(pfirst);
		vector<sstable> temp;
    	disk.push_back(temp);//L0层
	}
	else//说明已有文件
	{
		int i=0;
		while(1)
		{
			string layer=dir + "/L"+ to_string(i);//先找有哪些层
			path pvali(layer);
			if (exists(pvali)==true)
			{
				vector<sstable> tempi;
    			disk.push_back(tempi);
				getFiles(layer,i);
				i++;
			}
			else break;
		}  
	}
	

	
}

KVStore::~KVStore()
{                          
	delete [] memtable;
}

void KVStore::getFiles(string path,int i)//得到某文件夹下的所有文件，i表示disk的第i层
{
	//文件句柄  
    long hFile = 0;
    /*
        _finddata_t  存储文件各种信息的结构体，<io.h>;
    */  
    struct _finddata_t fileinfo;
    string p;//字符串，存放路径
    if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)//若查找成功，则进入
    {
        do
        {
            //如果是目录,迭代之（即文件夹内还有文件夹）  
            if ((fileinfo.attrib &  _A_SUBDIR))
            {
                //文件名不等于"."&&文件名不等于".."
                //.表示当前目录
                //..表示当前目录的父目录
                //判断时，两者都要忽略，不然就无限递归跳不出去了！
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) continue;
            }
            //如果不是,加入列表 
			else
			{
				int nums_in_name=0;
				for (int i=0;fileinfo.name[i]!='.';++i) nums_in_name++;
				string name=fileinfo.name;
				name.erase(nums_in_name);//1.txt，size为5，从nums_in_name=1处开始删
				uint64_t num=atol(name.c_str());

				sstable temp(p.assign(path).append("/").append(fileinfo.name),num);
				temp.setsearchtable();//根据文件设置searchtable
    			disk[i].push_back(temp);
			}

				

        } while (_findnext(hFile, &fileinfo) == 0);
        //_findclose函数结束查找
        _findclose(hFile);
    }
}

/**
 * Insert/Update the key-val
 * No return values for simplicity.ue pair.
 */
void KVStore::put(uint64_t key, const string &s)
{
    
	if (memtable->isExist(key)==true) 
		memtable->remove(key);//保证只有一个
	
    listnode tempnode=listnode(key,s);
	memtable->insert(tempnode);

	//下面判断是否需要存入sstable
	if (memtable->get_size() >= threshold-24)//需要将memtable存入sstable
	{
		
		memtable->getSearchTable();

		uint64_t idd=0;
		int layer_size=disk[0].size();
		if(layer_size!=0) idd=disk[0][layer_size-1].get_id()+1;
        string file=dir + "/L0" + "/" + to_string(idd) + ".txt";//永远都是先插入L0层
		sstable tmp(file,idd);//新建一个sstable变量

		memtable->writeToSstable(file);//写入sstable文件
		tmp.setsearchtable(memtable->get_mem_searchtable());//把mem_searchtable转移到sstable对象的searchtable中
		
		disk[0].push_back(tmp);//把sstable入栈
		
		for (int layer=0;layer<disk.size();++layer)//遍历每层
		{
			if (disk[layer].size() > powerOf2(layer+1))//需要合并()
			{

				uint64_t mymax=0;
				uint64_t mymin=LONG_MAX;
				list<string> erase_index1;
				
				int del_num=0;//本层要删除的文件的数量
				if(layer==0)//L0层时
				    del_num=disk[layer].size();
				else //非L0层时
					del_num=disk[layer].size()-powerOf2(layer+1);

				for(int k=0;k<del_num;++k)//得到layer中sstable包括的范围
				{
					int size=1;
					list<searchtablenode> ys=disk[layer][k].get_searchtable();
					list<searchtablenode>::iterator it1= ys.begin();
					while(size != ys.size()-1) 
					{	
						it1++;
						size++;
					}//最后一个节点是虚节点，所以要到倒数第二个节点
					mymin = mymin < ys.front().key ? mymin : ys.front().key;//比较每个最小的key
					mymax = mymax > it1->key ? mymax : it1->key;//比较每个最大的key
				}//得到layer中sstable包括的范围
				
				list<string> erase_index2;//下一层中要删除的sstable的index
				if (disk.size() > layer+1)//检查有没有下一层，若有
				{
					for(int k=0;k<disk[layer+1].size();++k)
					{
						list<searchtablenode> ys2=disk[layer+1][k].get_searchtable();
						list<searchtablenode>::iterator it2= ys2.begin();
						int size=1;
						while(size < ys2.size()-1)
						{
							it2++;
							size++;
						}
						uint64_t thismin = ys2.front().key;
						uint64_t thismax = it2->key;//比较每个最大的key
						bool isIn = (thismin <= mymin && thismax >= mymax) 
								 || (thismin >= mymin && thismin <= mymax)
								 || (thismax >= mymin && thismax <= mymax);
						if (isIn) 
						{
							sort_disk.push_back(disk[layer+1][k]);//push进sort_disk
							erase_index2.push_back(disk[layer+1][k].get_filename());//准备删除
						}
					}
				}
				
				for(int k=0;k<del_num;++k)//保证上一层出现在下一层的后方，因为后方的比较新，越新的越后进！！！！！！！！！
				{
					sort_disk.push_back(disk[layer][k]);//全部push进sort_disk
					erase_index1.push_back(disk[layer][k].get_filename());//准备删除
				}

				while (erase_index1.empty()==false) 
				{
					vector<sstable>::iterator it=disk[layer].begin();
					while(it!=disk[layer].end())
						if (it->get_filename() == *(erase_index1.begin())) 
        				{
							it = disk[layer].erase(it);
							erase_index1.pop_front();
						}	
    					else ++it;
					
				}

				while (erase_index2.empty()==false) 
				{
					vector<sstable>::iterator it=disk[layer+1].begin();
					while(it!=disk[layer+1].end())
						if (it->get_filename() == *(erase_index2.begin())) 
        				{
							it = disk[layer+1].erase(it);
							erase_index2.pop_front();
						}	
    					else ++it;
					
				}

				compaction();
				exporting(layer);
			}
			
		} 

		
		memtable->reset();
	}
	

}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
string KVStore::get(uint64_t key)
{
	if (memtable->isExist(key)==false) 
	{
		string s="";
		if (getInsstable(key,s)==true) return s;
		else return "";
	}

	return memtable->getvalue(key); 
}

bool KVStore::getInsstable(const uint64_t key,string &s)
{
	for (int i=0;i<disk.size();++i)
	{
		for (int j=disk[i].size()-1;j>=0;--j)
		{
			list<searchtablenode> temp=disk[i][j].get_searchtable();//得到索引区
			list<searchtablenode>::iterator it=temp.begin();
			while(it!=temp.end())
			{
				if(it->key == key)
				{
					list<searchtablenode>::iterator next=it;
					next++;//指向下一节点
					ifstream infile(disk[i][j].get_filename(),ios::in | ios::binary);//二进制打开
					infile.seekg(it->offset + 8,ios::beg);//跳过该节点的offset+key

					char* reading=new char[next->offset - it->offset - 8+1];
					infile.read((char*)&reading[0],next->offset - it->offset - 8);
					reading[next->offset - it->offset - 8]='\0';
					s=reading;

					// for (int i=0;i<next->offset - it->offset - 8;i++) s+=reading[i];
					delete [] reading;
					infile.close();
					return true;
				}
				it++;  
			}
		}
	}
	return false;
}

/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
    if (memtable->isExist(key) ) //memtable里有
	{
		if(memtable->getvalue(key)=="") return false;
		else memtable->remove(key);
	}
	else //检查sstable
	{
		string s="";
		if (getInsstable(key,s)==false || s=="") return false;//s=“”则表明已被删除
		else 
			put(key,"");
	}
	return true;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	memtable->reset();//memtable清空

	for(int i=0;i<disk.size();++i)
		this->disk[i].clear();
	disk.clear();//disk清空

	string first=dir + "/L0";//第一次构造
	path psrc(dir);
	remove_all(psrc);
	path pfirst(first);
	create_directories(pfirst);
	vector<sstable> temp;
    disk.push_back(temp);//L0层
	
	this->sort_table.clear();//sort_table清空
	sort_disk.clear();//sort_disk清空

}



int KVStore::powerOf2(int m)
{
	int result=1;
	while (m>0) {result*=2;m--;}
	return result;
}


void KVStore::compaction()//合并某层
{
	sort_table.clear();//先把sort_table清空
	for (int i=sort_disk.size()-1;i>=0;--i)//遍历sort_disk
	{
		ifstream infile(sort_disk[i].get_filename(),ios::in | ios::binary);//二进制打开
		infile.seekg(8,std::ios::beg);//跳过offset的offset

		list<searchtablenode> st=sort_disk[i].get_searchtable();//得到索引区
		list<searchtablenode>::iterator it=st.begin();
		list<searchtablenode>::iterator next=it;
		list<searchtablenode>::iterator end=st.end();
		next++;//next是下一个
		
		while(next!=end)//把索引区中所有元素兑换出来,最后一个空元素除外
		{
			
			uint64_t o=it->offset;
			
			uint64_t k=0;
			infile.read((char*)&k,8);

			char* reading=new char[next->offset - it->offset - 8+1];
			infile.read(reading,next->offset - it->offset - 8);
			reading[next->offset - it->offset - 8]='\0';
			string v=reading;
			// for(int i=0;i<next->offset - it->offset - 8;i++) v+=reading[i];
			delete [] reading;
			
			if(is_sorttable_contain(it->key)==true) 
			{
				it++;
				next++;//下一个节点
				continue;//如果sort_table中已存在该key的节点，跳过
			}
			it++;
			next++;//下一个节点
			mem_searchtablenode tempo(k,v,o);
			sort_table.push_back(tempo);
			
		}
		infile.close();
		path pval(sort_disk[i].get_filename());

		                       
		remove(pval);
	
	}
	//将sort_disk清空
	sort_disk.clear();
	sort_table.sort();
	
	// list<mem_searchtablenode>::iterator it=sort_table.begin();
	// while(it!=sort_table.end()) 
	// 	{cout<<it->key<<' ';it++;}
	
	
}

void KVStore::exporting(int layer)//发生溢出的层
{
    list<mem_searchtablenode> temp_table;//暂时储存key、value、offset，会转移到sstable中
	while (sort_table.empty()==false)
	{
		temp_table.clear();
		uint64_t numOfNode=1;//node的数量（用来计算offset占的大小）
		uint64_t sizeOfKVInSs=8;//sstable后的大小（不包括offset）
		uint64_t tot_size=0;
		while (tot_size < threshold && sort_table.empty()==false)
		{
			mem_searchtablenode newnode(sort_table.front().key,sort_table.front().value,sizeOfKVInSs);//此时offset应该重设
			temp_table.push_back(newnode);
			++numOfNode;
			sizeOfKVInSs += lengthOfData;
			sizeOfKVInSs += sort_table.front().value.length();
			tot_size += 24;
			tot_size += sort_table.front().value.length();
			sort_table.pop_front();
		}
		mem_searchtablenode ending(LONG_MAX,"",sizeOfKVInSs);
		temp_table.push_back(ending);//最后一个空节点
		
		uint64_t idd=0;
		if (disk.size() <= layer+1)//如果layer+1层不存在，新建layer+1层
		{
			string layer_file=dir + "/L"+ to_string(disk.size());
			path p2val(layer_file);
			create_directories(p2val); //创建目录 
	 
			vector<sstable> temp;
    		disk.push_back(temp);//layer+1层
		}
		else 
		{
			int layer_size=disk[layer+1].size();
			idd=disk[layer+1][layer_size-1].get_id()+1;
		}
		string file=dir + "/L" + to_string(layer+1) + "/" + to_string(idd) + ".txt";
		sstable tmp(file,idd);//新建一个sstable变量
		tmp.sort_setSstable(temp_table,sizeOfKVInSs);//写入sstable文件
		
		disk[layer+1].push_back(tmp);//把sstable入栈
	}
}

bool KVStore::is_sorttable_contain(uint64_t key)
{
	list<mem_searchtablenode>::iterator it=sort_table.begin();
	while (it!=sort_table.end())
	{
		if (it->key==key) return true;
		it++;
	}
	return false;
}

void KVStore::printmemtable()
{
	memtable->printall();
}

