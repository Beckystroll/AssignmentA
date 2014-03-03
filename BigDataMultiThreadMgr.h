#pragma once

#include <string>
#include <time.h>      
#include "DataItem.h"
#include "ItemGroup.h"
#include "WorkQueue2.h"
#include "BigDataReader.h"
#include "BigDataWriter.h"

class BigDataMultiThreadMgr
{

	// Manages the Big data workflow

public:
	BigDataMultiThreadMgr(const std::string& inputFile_, const std::string& signalFile_, const std::string& errorFile_, const int noRecords_, const int maxConcurrentThreads_ = 1, const int noThreads_ = 1 )
	{
		// update our statics - this should be a singleton - will change it later.
		inputFile = inputFile_;
		signalFile = signalFile_;
		errorFile = errorFile_; 
		noRecords = noRecords_;
		maxConcurrentThreads = maxConcurrentThreads_; 

		if (noThreads_ <= maxConcurrentThreads)
			noThreads = noThreads_; // this should be less than  or equal to maxConcurrentThreads
		else
			noThreads = maxConcurrentThreads;

		// now initialise our work queue
		myWorkQueue = new WorkQueue2(maxConcurrentThreads);
	};
	virtual ~BigDataMultiThreadMgr(void) {}

	bool run(void);

	static void initialiseItemGroups();
	static bool initialiseWriters();
	static void closeWriters();
	static bool initialiseReader();
	static void closeReader();
	static void reader();
	static void parser();
	static void scrubber1();
	static void scrubber2();
	static void scrubber3();
	static void sorter();
	static void duplicate();
	static void sigWriter();
	static void errWriter();


private:

	static double addDataItemPtr(double sum, DataItemPtr& ptr1);
	static double addSqrDataItemPtr(double sum, DataItemPtr& ptr1);
	static void find_duplicates(ItemGroup* group, int elements);
	static DataItemPtr& parse(DataItemPtr& ptr);

	static std::string inputFile;
	static std::string signalFile;
	static std::string errorFile;
	static int noRecords;
	static int maxConcurrentThreads;
	static int noThreads;

	//static unsigned short* inFlight;
	//static int lastId;
	//static bool lastIdSet;
	//static bool closeDown;

	// can't get boost to work at work!! so long for now

	//static clock_t read_duration;
	//static clock_t parse_duration;
	//static clock_t scrub_duration;
	//static clock_t signal_write_duration;
	//static clock_t error_write_duration;

	static WorkQueue2* myWorkQueue;
	static BigDataReader* dataReaderPtr; 
	static BigDataWriter* dataErrWriterPtr;
	static BigDataWriter* dataSigWriterPtr;

	static ItemGroup** itemGroups;

};

