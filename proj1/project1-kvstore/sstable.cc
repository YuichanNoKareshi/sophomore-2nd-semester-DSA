#include "sstable.h"

sstable::sstable(string fname,int idd)
{
    filename=fname;
    id=idd;
    this->searchtable.clear();
}

void sstable::setsearchtable(list<mem_searchtablenode> &mem_stable)
{
    while(mem_stable.empty()==false) 
    {
        searchtablenode temp(mem_stable.begin()->key,mem_stable.begin()->offset);
        searchtable.push_back(temp);
        mem_stable.pop_front();
    }
}

void sstable::setsearchtable()
{
    searchtable.clear();//先把sort_table清空

	ifstream infile(filename,ios::in | ios::binary);//二进制打开
	
    uint64_t offset_of_offset;
    infile.read((char*)&offset_of_offset,8);
    infile.seekg(offset_of_offset,ios::beg);//跳到offset
    
    while(infile.eof()==false)
    {
        uint64_t k,o;
        infile.read((char*)&k,8);
        infile.read((char*)&o,8);
        searchtablenode temp(k,o);
        this->searchtable.push_back(temp);
    }
}

void sstable::sort_setSstable(list<mem_searchtablenode> &temp_table,uint64_t sizeOfKVInSs)
{
    /* 先写入文件 */


    ofstream outfile(filename,ios::out | ios::binary);//二进制打开
    uint64_t offset_of_offset = 8 + sizeOfKVInSs;//索引区的offset
    outfile.write((char*)&offset_of_offset,8);
    for (list<mem_searchtablenode>::iterator it = temp_table.begin(); it != temp_table.end(); ++it) {
        outfile.write((char*)&it->key,8);
        outfile.write((char*)&it->value[0],(it->value).length());
    }//写完了非索引区
 
    //写索引区
    for (list<mem_searchtablenode>::iterator it = temp_table.begin(); it != temp_table.end(); ++it) {
        outfile.write((char*)&it->key,8);
        outfile.write((char*)&it->offset,8);
    } 
    outfile.close();
    /* */


    /* 再构建sstable的searchtable */

    this->searchtable.clear();
    while(temp_table.empty()==false) 
    {
        searchtablenode temp(temp_table.begin()->key,temp_table.begin()->offset);
        this->searchtable.push_back(temp);
        temp_table.pop_front();
    }
    /* */



}
