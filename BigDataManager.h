#pragma once

#include <string>
#include <time.h>   
#include "DataItem.h"
#include "ItemGroup.h"

class BigDataManager
{

	// Manages the Big data workflow

public:
	BigDataManager(const std::string& inputFile_, const std::string& signalFile_, const std::string& errorFile_, const int noRecords_)
		: inputFile(inputFile_), signalFile(signalFile_), errorFile(errorFile_), noRecords(noRecords_) {};
	virtual ~BigDataManager(void);

	bool run(void);


private:

	static double addDataItemPtr(double sum, DataItemPtr& ptr1); 
	static double addSqrDataItemPtr(double sum, DataItemPtr& ptr1);
	static void find_duplicates(ItemGroup* group, int elements);
	static DataItemPtr& parse(DataItemPtr& ptr);


	std::string inputFile;
	std::string signalFile;
	std::string errorFile;
	int noRecords;

	clock_t read_duration  = (clock_t)(0);
	clock_t parse_duration = (clock_t)(0);
	clock_t scrub_duration = (clock_t)(0);
	clock_t signal_write_duration =  (clock_t)(0);
	clock_t error_write_duration =  (clock_t)(0);

};