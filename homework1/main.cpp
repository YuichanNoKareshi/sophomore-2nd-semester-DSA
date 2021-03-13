#include "hash.h"
using namespace std;

int main ()
{
    for (int i=950;i>=400;i-=50)
    {
        hashtable test = hashtable(1000);
        test.hash("C:\\Codes++\\grade 2 term 2\\DSA\\homework1\\keys.txt",i);
        cout<<i<<"    "<<test.rate1<<"    "<<test.rate2<<endl;
    }

    return 0;
}