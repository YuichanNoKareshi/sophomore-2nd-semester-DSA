#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
using namespace std;
using namespace std::filesystem;


int main()
{
    string dir="./my.txt";
        
    ofstream outfile(dir,ios::out | ios::binary);
    uint64_t i=8;
    string s="hello,world";
    outfile.write((char*)&i,8);
    outfile.write((char*)&s[0],s.length());
    outfile.close();

    ifstream infile(dir,ios::in | ios::binary);
    uint64_t key;
    string value="";
    infile.read((char*)&key,8);

    char* reading=new char[s.length()];
    infile.read(reading,s.length());
    cout<<reading<<endl;
    // for(int i=0;i<s.length();i++) value += reading[i];
    value=reading;
    cout<<value;
    return 0;
    
}
