#include "BigDataManager.h"
#include "DateRangeScrubber.h"
#include "TimeRangeScrubber.h"
#include "PriceRangeScrubber.h"
#include "BigDataReader.h"
#include "BigDataWriter.h"
#include "DataItemPtrCmp.h"

#include <ctime>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <numeric>
#include <cmath>

DataItemPtr& BigDataManager::parse(DataItemPtr& ptr) {

	ptr->parse();
	return ptr;
}

double BigDataManager::addDataItemPtr(double sum, DataItemPtr& ptr1) {

	return sum + ptr1->getPrice();
}

double BigDataManager::addSqrDataItemPtr(double sum, DataItemPtr& ptr1) {

	return sum + ptr1->getPrice() * ptr1->getPrice();
}

void BigDataManager::find_duplicates(ItemGroup* group, int elements) {

	// any to find?
	if (elements == 0)
		return;


	for (int i = 1; i < elements; i++) {
		if ((*group)[i].getLine().compare((*group)[i-1].getLine()) == 0)
			(*group)[i].setErrorType(DataItem::DUPLICATE);
	}
}



BigDataManager::~BigDataManager(void)
{
}



bool BigDataManager::run(void)
{
	// create vector of data scrubbers
	//ScrubberVector scrubbers;
	//ScrubberVector::iterator iter;

	//scrubbers.push_back(new DateRangeScrubber("20080803"));  // not bothering with smart pointer for now.

	// create our data structures
	ItemGroup* itemGroup = new ItemGroup(1, noRecords);

	bool completedInd = false;
	bool errorInd = false;
	long total = 0;

	// initialise files
	BigDataReader reader(inputFile, noRecords);
	BigDataWriter sigWriter(signalFile, false);
	BigDataWriter errWriter(errorFile, true);

	// remove the output files if exist
	sigWriter.deleteFile();
	errWriter.deleteFile();

	if (!reader.open() || !sigWriter.open() || !errWriter.open())
		errorInd = true;

	// process items - a block at a time

	clock_t start_time = clock();

	while (!completedInd && !errorInd) {

		// start reading
		clock_t current_time = clock();
		
		// read items in
		int count = reader.getDataItems(itemGroup);
		total += count;
		std::cout << total << std::endl;

		// no data?
		if (count == 0) {
			completedInd = true;
			break;
		}
		else if (count < noRecords) {
			// we need to resize the vector so the transforms later do not cause problems
			itemGroup->getItemVector().resize(count);
		}
		read_duration += clock() - current_time;

		current_time = clock();
		// parse the data
		std::transform(itemGroup->getItemVector().begin(), itemGroup->getItemVector().end(), itemGroup->getItemVector().begin(), &BigDataManager::parse);
		parse_duration += clock() - current_time;


		// srub them
		current_time = clock();

		std::transform(itemGroup->getItemVector().begin(), itemGroup->getItemVector().end(), itemGroup->getItemVector().begin(), DateRangeScrubber("20080803"));  // doing this as a pipeline - don't know if this can be threaded though.
		std::transform(itemGroup->getItemVector().begin(), itemGroup->getItemVector().end(), itemGroup->getItemVector().begin(), TimeRangeScrubber(10,15));  // doing this as a pipeline - don't know if this can be threaded though.

		// work out mean price and variance - not the very best way to do this but hopefully GPU compatible
		double sum = std::accumulate(itemGroup->getItemVector().begin(), itemGroup->getItemVector().end(), 0.0, &BigDataManager::addDataItemPtr);
		double sq_sum = std::accumulate(itemGroup->getItemVector().begin(), itemGroup->getItemVector().end(), 0.0, &BigDataManager::addSqrDataItemPtr);
		double mean = sum / itemGroup->getItemVector().size();
		double std_dev = sqrt(sq_sum / itemGroup->getItemVector().size() - mean * mean);
		// now check that prices are within 3 std dev of the mean
		std::transform(itemGroup->getItemVector().begin(), itemGroup->getItemVector().end(), itemGroup->getItemVector().begin(), PriceRangeScrubber(mean - 3 * std_dev, mean + 3 * std_dev));  

		// sort it
		std::sort(itemGroup->getItemVector().begin(), itemGroup->getItemVector().end(), DataItemPtrCmp());

		// find duplicates - only the 2nd or more of something is considered a dup.

		find_duplicates(itemGroup, count);

		scrub_duration += clock() - current_time;

		// write this out
		current_time = clock();
		
		sigWriter.writeDataItems(itemGroup);

		signal_write_duration += clock() - current_time;
		current_time = clock();

		errWriter.writeDataItems(itemGroup);

		error_write_duration += clock() - current_time;

		// are we finished?
		if (reader.isEOF())
			completedInd = true;
		//else
			//itemGroup->reset();

	
	}

	// close writers   

	sigWriter.close();
	errWriter.close();

	// close reader
	reader.close();

	clock_t stop_time = clock();

	// close
	std::cout << "Read duration (sec): " << (float)read_duration / CLOCKS_PER_SEC << std::endl;
	std::cout << "Parse duration (sec): " << (float)parse_duration / CLOCKS_PER_SEC << std::endl;
	std::cout << "Scrub duration (sec)" << (float)scrub_duration / CLOCKS_PER_SEC << std::endl;
	std::cout << "Signal Write duration (sec): " << (float)signal_write_duration / CLOCKS_PER_SEC << std::endl;
	std::cout << "Error Write duration (sec): " << (float)error_write_duration / CLOCKS_PER_SEC << std::endl;
	std::cout << "Total Elapsed (sec): " << (float)(stop_time - start_time) / CLOCKS_PER_SEC << std::endl;

	return true;
}


