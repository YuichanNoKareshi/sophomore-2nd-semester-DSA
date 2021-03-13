#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <ctime>
#include "test.h"
using namespace std;

class myTest : public Test {
private:
	const uint64_t NORMAL_TEST_MAX = 1000;
    
	void regular_test(uint64_t max)
	{
        store.reset();

		// Test multiple key-value pairs
        clock_t start1=clock();
		for (uint64_t i = 0; i < max; ++i) {
			store.put(i, std::string(i+1, 's'));
            if(i%10==9) 
                cout << (double)(clock() - start1) / CLOCKS_PER_SEC << endl ;
		}
        cout<<endl;
		phase();

		// Test after all insertions
        clock_t start2=clock();
		for (uint64_t i = 0; i < max; ++i)
        {
            EXPECT(std::string(i+1, 's'), store.get(i));
            if(i%10==9) 
                cout << (double)(clock() - start2) / CLOCKS_PER_SEC << endl ;
        }
		cout<<endl;	
		phase();

		// Test deletions
        clock_t start3=clock();
		for (uint64_t i = 0; i < max; ++i)
        {
            EXPECT(true, store.del(i));
            if(i%10==9) 
                cout << (double)(clock() - start3) / CLOCKS_PER_SEC << endl ;
        }
        cout<<endl;
		phase();
		report();
	}
    
    const uint64_t LARGE_TEST_MAX = 1000*100;
    void large_test(uint64_t max)
	{
        store.reset();

		// Test multiple key-value pairs
        clock_t start1=clock();
        int num1=0;
		for (uint64_t i = 0; i < max; ++i) {
			++num1;
			store.put(i, std::string(1024, 's'));
            if((double)(clock() - start1) / CLOCKS_PER_SEC >= 0.1) 
            {
                cout << num1 << endl ;
                num1=0;
                start1=clock();
            }
		}
		cout<<endl;
		phase();
		
		// // Test after all insertions
        // clock_t start2=clock();
        // int num2=0;
		// for (uint64_t i = 0; i < max; ++i,++num2)
        // {
        //     EXPECT(std::string(i+1, 's'), store.get(i));
        //     if((double)(clock() - start2) / CLOCKS_PER_SEC==1) 
        //     {
        //         cout << num2 << " " ;
        //         num2=0;
        //         start2=clock();
        //     }
        // }
		// cout<<endl;	
		// phase();

		// // Test deletions
        // clock_t start3=clock();
        // int num3=0;
		// for (uint64_t i = 0; i < max; ++i,++num3)
        // {
        //     EXPECT(true, store.del(i));
        //     if((double)(clock() - start3) / CLOCKS_PER_SEC==1) 
        //     {
        //         cout << num3 << " " ;
        //         num3=0;
        //         start3=clock();
        //     }
        // }
        // cout<<endl;
		// phase();
		report();
	}


public:
	myTest(const std::string &dir, bool v=true) : Test(dir, v)
	{
	}

	void start_test(void *args = NULL) override
	{
		cout << "KVStore My Test" << endl;

		cout << "[Normal Test]" << endl;
		regular_test(NORMAL_TEST_MAX);

		// cout << "[Large Test]" << endl;
		// large_test(LARGE_TEST_MAX);
	}
};

int main(int argc, char *argv[])
{
	bool verbose = (argc == 2 && std::string(argv[1]) == "-v");

	std::cout << "Usage: " << argv[0] << " [-v]" << std::endl;
	std::cout << "  -v: print extra info for failed tests [currently ";
	std::cout << (verbose ? "ON" : "OFF")<< "]" << std::endl;
	std::cout << std::endl;
	std::cout.flush();

	myTest test("./data", verbose);

	test.start_test();

	return 0;
}
