#include <studentttests.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    std::vector<double> sample;
    if (argc != 2){
	cout << "A file containing a sample set is required.\n";
	exit(1);
    }
    string line;
    ifstream myfile("test.txt");
    if (myfile.is_open()){
	while (myfile.good()){
	    getline(myfile, line);
	    cout << line << endl;
	}
	myfile.close();
    }
    return 0;
}
