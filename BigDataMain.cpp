/*

C++ program to read, scrub and output a large data file

*/


#include <iostream>   

#include "BigDataManager.h"
#include "BigDataMultiThreadMgr.h"


using namespace std;

const std::string USAGE = "BigData_HW1 input_file signal_file error_file [no_of_records_per_row] [max concurrent threads] [initial no of threads per type] \n";
const int NO_OF_RECORDS = 1000;
const int MAX_CONCURRENT_THREADS = 10;
const int NO_THREADS = 10;

int main(int argc, const char* argv[])
{

	if (argc < 4) {
		cout << USAGE << "Exiting....";
		exit(1);
	}

	// 1st - input file
	string INPUT_FILE = argv[1];
	// 2nd - signal file
	string SIGNAL_FILE = argv[2];
	// 3rd - input file
	string ERROR_FILE = argv[3];
	// 4 : no of records per read
	int numRecords = NO_OF_RECORDS;
	if (argc >= 4)
		numRecords = atoi(argv[4]);
	// 5: no of concurrent threads
	int maxConcurrentThreads = MAX_CONCURRENT_THREADS;
	if (argc >= 5)
		maxConcurrentThreads = atoi(argv[5]);
	// 6: no of threads (per type)
	int maxThreads = NO_THREADS;
	if (argc >= 6)
		maxThreads = atoi(argv[6]);

	int temp = 4;
	cout << "Hi" << endl;   // more output

	if (maxConcurrentThreads > 1) {
		BigDataMultiThreadMgr mgr(INPUT_FILE, SIGNAL_FILE, ERROR_FILE, numRecords, maxConcurrentThreads, maxThreads);
		mgr.run();
	}
	else {
		BigDataManager mgr(INPUT_FILE, SIGNAL_FILE, ERROR_FILE, numRecords);
		mgr.run();
	}

	//cin.get();  // for microsoft testing only
	return 0;                                 // terminate main()
}

