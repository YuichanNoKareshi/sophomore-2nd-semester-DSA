#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
using namespace std;

// template <class Type>  
// void Swap(Type &x,Type &y);  
  
// inline int Random(int x, int y);  
  
// template <class Type>  
// void BubbleSort(Type a[],int p,int r);  
  
// template <class Type>  
// int Partition(Type a[],int p,int r,Type x);  
  
// template <class Type>  
// Type Linearelect(Type a[],int p,int r,int k);  
  
  
template <class Type>  
void Swap(Type &x,Type &y)  
{  
    Type temp = x;  
    x = y;  
    y = temp;  
}  
  
//冒泡排序  
template <class Type>  
void BubbleSort(Type a[],int p,int r)  
{  
     //记录一次遍历中是否有元素的交换     
     bool exchange;    
     for(int i=p; i<r;i++)    
     {    
        exchange = false ;    
        for(int j=0; j<=r-i; j++)    
        {    
            if(a[j]<a[j-1])    
            {    
                Swap(a[j],a[j-1]);   
                exchange = true;    
            }     
        }     
        //如果这次遍历没有元素的交换,那么排序结束     
        if(false == exchange)    
        {  
             break ;    
        }  
     }  
}  
  
template <class Type>  
int Partition(Type a[],int p,int r,Type x)//一轮快排
{  
    int i = p-1,j = r + 1;  
  
    while(true)  
    {  
        while(a[++i]<x && i<r);  
        while(a[--j]>x);  
        if(i>=j)  
        {  
            break;  
        }  
        Swap(a[i],a[j]);  
    }     
    return j;  
}  
  
  
template <class Type>  
Type Linearselect(Type a[],long p,long r,long k)  
{  
    if(r-p<200)  //长度当小于200时直接冒泡排序
    {  
        BubbleSort(a,p,r);  
        return a[p+k-1];  
    }  

    for(int i=0; i<=(r-p-4)/5; i++)  
    {  
        //将元素每5个分成一组，分别排序，并将该组中位数与a[p+i]交换位置  
        //使所有中位数都排列在数组最左侧，以便进一步查找中位数的中位数  
        BubbleSort(a,p+5*i,p+5*i+4);  
        Swap(a[p+5*i+2],a[p+i]);  
    }  
    //找中位数的中位数x  
    Type x = Linearselect(a,p,p+(r-p-4)/5,(r-p-4)/10);  
    int i = Partition(a,p,r,x);  //以中位数为基准进行快排
    int j = i-p+1;
    if(k<=j)//比较要找的第k小数与中位数的位置
    {  
        return Linearselect(a,p,i,k);  
    }  
    else  
    {  
        return Linearselect(a,i+1,r,k-j);  
    }  
}

void quicksort(int* &a, long left,long right) 
{ 
    int i,j,t,temp; 
    if(left>right) 
       return; 
                                
    temp=a[left]; //temp中存的就是基准数 
    i=left; 
    j=right; 
    while(i!=j) 
    { 
                   //顺序很重要，要先从右边开始找 
                   while(a[j]>=temp && i<j) 
                            j--; 
                   //再找右边的 
                   while(a[i]<=temp && i<j) 
                            i++; 
                   //交换两个数在数组中的位置 
                   if(i<j) 
                   { 
                        Swap(a[i],a[j]);
                   } 
    } 
    //最终将基准数归位 
    a[left]=a[i]; 
    a[i]=temp; 
                             
    quicksort(a, left,i-1);//继续处理左边的，这里是一个递归的过程 
    quicksort(a, i+1,right);//继续处理右边的 ，这里是一个递归的过程 
} 



int main()  
{  
    long N=1000000;
    int* a=new int[N];  
    uniform_int_distribution<unsigned> u(0,100000000);
    default_random_engine e(time(0));
    for(int i=0; i<N; i++)   
        a[i] = u(e);

    // for (int i=0;i<N;i++)
    //     a[i]=N-1-i;

    clock_t start1 = clock();
    cout<<"the 100th smallest element is "<<Linearselect(a,0,N-1,100)<<endl;  
    clock_t end1 = clock();
    cout << "const " << (double)(end1 - start1) / CLOCKS_PER_SEC << " second" << endl; //计时

    clock_t start2 = clock();
    quicksort(a,0,N-1);
    cout<<"the 100th smallest element is "<<a[99]<<endl;  
    clock_t end2 = clock();
    cout << "const " << (double)(end2 - start2) / CLOCKS_PER_SEC << " second" << endl; //计时

    return 0;
}  