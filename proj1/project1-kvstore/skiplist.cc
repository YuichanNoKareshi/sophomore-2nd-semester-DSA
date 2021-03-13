#include "skiplist.h"
using namespace std;

static unsigned int seed = NULL; // 随机种子

bool SkipList::randomVal() {    
    if (seed == NULL) {
        seed = (unsigned)time(NULL);
    }
    ::srand(seed);
    int ret = ::rand() % 2;
    seed = ::rand();
    if (ret == 0) {
        return true;
    }
    else {
        return false;
    }
}

void SkipList::insert(listnode n) {
    ++numOfNode;
    sizeOfKVInSs += lengthOfData + n.mynode.value.length();
    
    /* 首先查找L1层 */
    listnode* cursor = bottom;//找到最底层L1层
    listnode* new_node = nullptr;
    // while (cursor->down != nullptr) {
    //     cursor = cursor->down;
    // }

    listnode* cur_head = bottom; // 当前层链表头
    while (cursor->right != nullptr) {
        if (n.mynode.key < cursor->right->mynode.key && new_node == nullptr) {//值一样的节点，后来的会插在右边
            new_node = new listnode(n);
            new_node->right = cursor->right;
            cursor->right = new_node;
            break;
        }
        cursor = cursor->right; // 向右移动游标
    }
    if (new_node == nullptr) {//插入的是最大值，插入到最右边
        new_node = new listnode(n);
        cursor->right = new_node;
    }
    /* L1层插入完成 */
    /* 上层操作 */
    int cur_lvl = 1; // 当前所在层
    while (randomVal()) {
        cur_lvl++;
        if (lvl_num < cur_lvl) { // 整个表需要增加一层
            lvl_num++;
            listnode* new_head = new listnode();
            new_head->down = head;
            head->up = new_head;
            head = new_head;
        }
        cur_head = cur_head->up; // 当前链表头上移一层
        cursor = cur_head; // 继续获取游标
        listnode* skip_node = nullptr; // 非L1层的节点
        while (cursor->right != nullptr) {
            if (n.mynode.key < cursor->right->mynode.key && skip_node == nullptr) {
                skip_node = new listnode(n);
                skip_node->right = cursor->right;
            }
            cursor = cursor->right;
        }
        if (skip_node == nullptr) {
            skip_node = new listnode(n);
            cursor->right = skip_node;
        }
        while (new_node->up != nullptr) {
            new_node = new_node->up;
        }
        /* 连接上下两个节点 */
        skip_node->down = new_node;
        new_node->up = skip_node;
    }

    
}

bool SkipList::isExist(uint64_t k) {
    listnode* cursor = nullptr;
    listnode* linehead = nullptr;//记录每行行首
    if (head == nullptr) {
        return false;
    }
    /* 初始化游标指针 */
    cursor = head;
    linehead = head;
    while (linehead->down != nullptr) { // 第一层循环游标向下
        while (cursor->right != nullptr) { // 第二层循环游标向右
            if (k < cursor->right->mynode.key) break;
            if (k == cursor->right->mynode.key) return true;
            cursor = cursor->right;
        }
        linehead = linehead->down;
        cursor = linehead;
    }
    while (cursor->right != nullptr) { // L1层循环开始具体查询
        if (k > cursor->right->mynode.key) {
            cursor = cursor->right; // 如果查找的值大于右侧值则游标可以继续向右
        } 
        else if (k == cursor->right->mynode.key) { // 如果等于则表明已经找到节点
            return true;
        }
        else if (k < cursor->right->mynode.key) { // 如果小于则表明不存在该节点
            return false;
        }
    }
    return false; // 完成遍历返回false;
}

string SkipList::getvalue(uint64_t k) {
    listnode* cursor = nullptr;
    listnode* linehead = nullptr;//记录每行行首
    if (head == nullptr) {
        return "";
    }
    /* 初始化游标指针 */
    cursor = head;
    linehead = head;
    while (linehead->down != nullptr) { // 第一层循环游标向下
        while (cursor->right != nullptr) { // 第二层循环游标向右
            if (k < cursor->right->mynode.key) break;
            if (k == cursor->right->mynode.key) return cursor->right->mynode.value;
            cursor = cursor->right;
        }
        linehead = linehead->down;
        cursor = linehead;
    }
    while (cursor->right != nullptr) { // L1层循环开始具体查询
        if (k > cursor->right->mynode.key) {
            cursor = cursor->right; // 如果查找的值大于右侧值则游标可以继续向右
        } 
        else if (k == cursor->right->mynode.key) { // 如果等于则表明已经找到节点
            return cursor->right->mynode.value;
        }
        else if (k < cursor->right->mynode.key) { // 如果小于则表明不存在该节点
            return "";
        }
    }
    return ""; // 完成遍历返回false;
}

void SkipList::remove(uint64_t k) {
    bool isDelete=false;

    listnode* cursor = head; // 获得游标
    listnode* pre_head = nullptr; // 上一行的头指针，删除行时使用
    while (true) {
        listnode* cur_head = cursor; // 当前行头指针
        if (pre_head != nullptr) {
            cur_head->up = nullptr;
            pre_head->down = nullptr; // 解除上下级的指针
            delete pre_head;
            pre_head = nullptr; // 指针归0
            lvl_num--; // 层数-1
            head = cur_head; // 重新指定起始指针
        }
        while (cursor != nullptr && cursor->right != nullptr) { // 在当前行中查询val
            if (k == cursor->right->mynode.key) {
                listnode* delptr = cursor->right;
                cursor->right = cursor->right->right;
  
                if (isDelete==false)//防止多次删除
                {
                    sizeOfKVInSs = sizeOfKVInSs - lengthOfData - delptr->mynode.value.length();
                    --numOfNode; 
                    isDelete=true;
                }

                delete delptr; // 析构找到的节点
            }
            cursor = cursor->right;
        }
        if (cur_head->right == nullptr) { // 判断当前行是否还存在其它元素，如果不存在则删除该行并将整个跳跃表降维
            pre_head = cur_head;
        }
        if (cur_head->down == nullptr) {
            break;
        }
        else {
            cursor = cur_head->down;
        }
    }

}

void SkipList::reset()
{
    /* 首先查找L1层 */
    listnode* cursor = bottom;
    listnode* new_node = nullptr;

    vector<uint64_t> vec;
    while (cursor->right != nullptr) {
        cursor = cursor->right; // 向右移动游标
        vec.push_back(cursor->mynode.key);
    }

    for (auto x:vec)
        this->remove(x);//remove之后表高自动为1

    lvl_num=1;
    this->sizeOfKVInSs=8;
    this->mem_searchtable.clear();
//末尾有一个key=LONG_MAX，value=“”的空节点
    
}

void SkipList::getSearchTable()
{
    /* 首先查找L1层 */
    listnode* cursor = bottom;

    uint64_t offset=8;//前八个byte记录的是索引区的offset
    while (cursor->right != nullptr) {
        cursor = cursor->right; // 向右移动游标，这样可以避免记录bottom
        mem_searchtablenode temp(cursor->mynode.key,cursor->mynode.value,offset);//第一个节点offset就是8
        offset += 8;//key的长度8byte
        offset += cursor->mynode.value.length();//value的长度
        mem_searchtable.push_back(temp);
    }
    uint64_t ending=LONG_MAX;
    mem_searchtablenode temp(ending,"",offset);//push进空值，便于取出最后一个节点的value
    mem_searchtable.push_back(temp);
}

void SkipList::writeToSstable(string file)
{
    ofstream outfile(file,ios::out | ios::binary);//二进制打开
    outfile.seekp(0,ios::beg);
    uint64_t offset_of_offset = 8 + sizeOfKVInSs;//索引区的offset=自己的长度+kv区的大小
    outfile.write((char*)&offset_of_offset,8);

    for (list<mem_searchtablenode>::iterator it = mem_searchtable.begin(); it != mem_searchtable.end(); ++it) {
        outfile.write((char*)&it->key,8);
        outfile.write((char*)&it->value[0],(it->value).length());
    }//写完了非索引区

    //写索引区
    for (list<mem_searchtablenode>::iterator it = mem_searchtable.begin(); it != mem_searchtable.end(); ++it) {
        outfile.write((char*)&it->key,8);
        outfile.write((char*)&it->offset,8);
    } 
    outfile.close();

}

//查看跳表的大小
uint64_t SkipList::get_size()
{
    uint64_t size=0;
	size += lengthOfData;//前8个byte记录索引区的offset
	size += sizeOfKVInSs;

	size = size + numOfNode * 2 * lengthOfData;//offset和key各8位
	return size;
}

void SkipList::printall()
{
    listnode* cursor = bottom;
    while (cursor->right != nullptr) {
        cursor = cursor->right; // 向右移动游标，这样可以避免记录bottom
        cout<<cursor->mynode.key<<" ";
    }

}