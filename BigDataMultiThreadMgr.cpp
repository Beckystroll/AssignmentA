#include <iostream>
//#include <boost/thread/thread.hpp>
//#include <boost/thread/condition.hpp>
//#include <boost/thread/mutex.hpp>
#include <queue>
#include <string>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <stdlib.h>
#include <numeric>
#include <cmath>
#include <vector>

#include "BigDataMultiThreadMgr.h"
#include "DateRangeScrubber.h"
#include "TimeRangeScrubber.h"
#include "PriceRangeScrubber.h"
#include "DataItemPtrCmp.h"


// initialise our statics

std::string BigDataMultiThreadMgr::inputFile = "";
std::string BigDataMultiThreadMgr::signalFile = "";
std::string BigDataMultiThreadMgr::errorFile = "";
int BigDataMultiThreadMgr::noRecords = 0;
int BigDataMultiThreadMgr::noThreads = 0;
int BigDataMultiThreadMgr::maxConcurrentThreads = 0;
WorkQueue2* BigDataMultiThreadMgr::myWorkQueue = nullptr;
BigDataReader* BigDataMultiThreadMgr::dataReaderPtr = nullptr;
BigDataWriter* BigDataMultiThreadMgr::dataErrWriterPtr = nullptr;
BigDataWriter* BigDataMultiThreadMgr::dataSigWriterPtr = nullptr;
ItemGroup** BigDataMultiThreadMgr::itemGroups = nullptr;


DataItemPtr& BigDataMultiThreadMgr::parse(DataItemPtr& ptr) {

	ptr->parse();
	return ptr;
}

double BigDataMultiThreadMgr::addDataItemPtr(double sum, DataItemPtr& ptr1) {

	return sum + ptr1->getPrice();
}

double BigDataMultiThreadMgr::addSqrDataItemPtr(double sum, DataItemPtr& ptr1) {

	return sum + ptr1->getPrice() * ptr1->getPrice();
}

void BigDataMultiThreadMgr::find_duplicates(ItemGroup* group, int elements) {

	// any to find?
	if (elements == 0)
		return;


	for (int i = 1; i < elements; i++) {
		if ((*group)[i].getLine().compare((*group)[i - 1].getLine()) == 0)
			(*group)[i].setErrorType(DataItem::DUPLICATE);
	}
}

struct JoinThreads {
	void operator() (std::thread& th) { th.join(); }
};


void BigDataMultiThreadMgr::initialiseItemGroups() {

	itemGroups = new ItemGroup*[maxConcurrentThreads];
	for (int i = 0; i < maxConcurrentThreads; i++) {
		itemGroups[i] = new ItemGroup(i, noRecords);
	}

}

bool BigDataMultiThreadMgr::initialiseReader() {
	dataReaderPtr = new BigDataReader(inputFile, noRecords);
	return (dataReaderPtr->open());
}

void BigDataMultiThreadMgr::closeReader() {
	// close reader
	dataReaderPtr->close();
}

bool BigDataMultiThreadMgr::initialiseWriters() {
	dataSigWriterPtr = new BigDataWriter(signalFile, false);
	dataErrWriterPtr = new BigDataWriter(errorFile, true);

	// remove the output files if exist
	dataSigWriterPtr->deleteFile();
	dataErrWriterPtr->deleteFile();

	return (dataSigWriterPtr->open() && dataErrWriterPtr->open());
}

void BigDataMultiThreadMgr::closeWriters() {
	// close writers
	dataSigWriterPtr->close();
	dataErrWriterPtr->close();

}

void BigDataMultiThreadMgr::reader() {
	static int id = 0;
	static int total = 0;
	static bool completedInd = false;

	// start reading

	while (!completedInd) {

		id++;
		IntPair nextFree = myWorkQueue->nextRead();
		nextFree.second = id;

		clock_t current_time = clock();
		int count = dataReaderPtr->getDataItems(itemGroups[nextFree.first]);
		myWorkQueue->updateReadDuration(clock() - current_time);
		total += count;
		std::cout << total << std::endl;

		// no data?
		if (count == 0) {
			completedInd = true;
			break;
		}
		else if (count < noRecords) {
			// we need to resize the vector so the transforms later do not cause problems
			itemGroups[nextFree.first]->getItemVector().resize(count);
		}

		myWorkQueue->completedRead(nextFree);
	}

	myWorkQueue->completedLastRead();


}

void BigDataMultiThreadMgr::parser() {

	while (true) {
		IntPair nextFree = myWorkQueue->nextParse();

		if (nextFree == myWorkQueue->COMPLETED_PAIR)
			break;

		// parse it
		clock_t current_time = clock();
		std::transform(itemGroups[nextFree.first]->getItemVector().begin(), itemGroups[nextFree.first]->getItemVector().end(), itemGroups[nextFree.first]->getItemVector().begin(), &BigDataMultiThreadMgr::parse);
		myWorkQueue->updateParseDuration(clock() - current_time);

		myWorkQueue->completedParse(nextFree);
	}
}

void BigDataMultiThreadMgr::scrubber1() {

	while (true) {
		IntPair nextFree = myWorkQueue->nextScrub1();

		if (nextFree == myWorkQueue->COMPLETED_PAIR)
			break;

		int index = nextFree.first;

		// date range scrubbing
		clock_t current_time = clock();
		std::transform(itemGroups[index]->getItemVector().begin(), itemGroups[index]->getItemVector().end(), itemGroups[index]->getItemVector().begin(), DateRangeScrubber("20080803"));
		myWorkQueue->updateScrubDuration(clock() - current_time);

		myWorkQueue->completedScrub1(nextFree);
	}
}

void BigDataMultiThreadMgr::scrubber2() {

	while (true) {
		IntPair nextFree = myWorkQueue->nextScrub2();

		if (nextFree == myWorkQueue->COMPLETED_PAIR)
			break;

		int index = nextFree.first;

		// Time range scrubbing
		clock_t current_time = clock();
		std::transform(itemGroups[index]->getItemVector().begin(), itemGroups[index]->getItemVector().end(), itemGroups[index]->getItemVector().begin(), TimeRangeScrubber(10, 15));
		myWorkQueue->updateScrubDuration(clock() - current_time);

		myWorkQueue->completedScrub2(nextFree);
	}
}

void BigDataMultiThreadMgr::scrubber3() {

	while (true) {
		IntPair nextFree = myWorkQueue->nextScrub3();

		if (nextFree == myWorkQueue->COMPLETED_PAIR)
			break;

		int index = nextFree.first;

		// Price check
		clock_t current_time = clock();
		double sum = std::accumulate(itemGroups[index]->getItemVector().begin(), itemGroups[index]->getItemVector().end(), 0.0, &BigDataMultiThreadMgr::addDataItemPtr);
		double sq_sum = std::accumulate(itemGroups[index]->getItemVector().begin(), itemGroups[index]->getItemVector().end(), 0.0, &BigDataMultiThreadMgr::addSqrDataItemPtr);
		double mean = sum / itemGroups[index]->getItemVector().size();
		double std_dev = sqrt(sq_sum / itemGroups[index]->getItemVector().size() - mean * mean);
		// now check that prices are within 3 std dev of the mean

		std::transform(itemGroups[index]->getItemVector().begin(), itemGroups[index]->getItemVector().end(), itemGroups[index]->getItemVector().begin(), PriceRangeScrubber(mean - 3 * std_dev, mean + 3 * std_dev));
		myWorkQueue->updateScrubDuration(clock() - current_time);

		myWorkQueue->completedScrub3(nextFree);
	}
}

void BigDataMultiThreadMgr::sorter() {

	while (true) {
		IntPair nextFree = myWorkQueue->nextSort();

		if (nextFree == myWorkQueue->COMPLETED_PAIR)
			break;

		int index = nextFree.first;

		// Sort it - for duplicate check later
		clock_t current_time = clock();
		std::sort(itemGroups[index]->getItemVector().begin(), itemGroups[index]->getItemVector().end(), DataItemPtrCmp());
		myWorkQueue->updateScrubDuration(clock() - current_time);

		myWorkQueue->completedSort(nextFree);
	}
}

void BigDataMultiThreadMgr::duplicate() {

	while (true) {
		IntPair nextFree = myWorkQueue->nextDuplicate();

		if (nextFree == myWorkQueue->COMPLETED_PAIR)
			break;

		int index = nextFree.first;

		// find duplicates - only the 2nd or more of something is considered a dup.

		clock_t current_time = clock();
		find_duplicates(itemGroups[index], itemGroups[index]->getItemVector().size());
		myWorkQueue->updateScrubDuration(clock() - current_time);

		myWorkQueue->completedDuplicate(nextFree);
	}
}

void BigDataMultiThreadMgr::sigWriter() {

	while (true) {

		IntPair nextFree = myWorkQueue->nextSigWrite();

		if (nextFree == myWorkQueue->COMPLETED_PAIR)
			break;

		int index = nextFree.first;

		// write this out
		clock_t current_time = clock();
		dataSigWriterPtr->writeDataItems(itemGroups[index]);
		myWorkQueue->updateSigWriteDuration(clock() - current_time);

		myWorkQueue->completedSigWrite(nextFree);
	}
}


void BigDataMultiThreadMgr::errWriter() {

	bool completedInd = false;

	while (!completedInd) {

		IntPair nextFree = myWorkQueue->nextErrWrite();

		int index = nextFree.first;

		// write this out
		clock_t current_time = clock();
		dataErrWriterPtr->writeDataItems(itemGroups[index]);
		myWorkQueue->updateErrWriteDuration(clock() - current_time);

		completedInd = myWorkQueue->completedErrWrite(nextFree);

	}

}



bool BigDataMultiThreadMgr::run() {

	// create our data structures
	initialiseItemGroups();

	bool completedInd = false;
	bool errorInd = false;
	long total = 0;

	std::cout.sync_with_stdio(true);

	if (!initialiseReader() || !initialiseWriters())
		errorInd = true;

	clock_t start_time = clock();

	if (!errorInd) {

		std::vector<std::thread> myThreadVector = std::vector<std::thread>();
		myThreadVector.push_back(std::thread(reader));

		for (int i = 0; i < noThreads; i++) {
			myThreadVector.push_back(std::thread(parser));
			myThreadVector.push_back(std::thread(scrubber1));
			myThreadVector.push_back(std::thread(scrubber2));
			myThreadVector.push_back(std::thread(scrubber3));
			myThreadVector.push_back(std::thread(sorter));
			myThreadVector.push_back(std::thread(duplicate));
		}
		
		myThreadVector.push_back(std::thread(sigWriter));
		myThreadVector.push_back(std::thread(errWriter));

		std::for_each(myThreadVector.begin(), myThreadVector.end(), JoinThreads());

	}

	// close writers
	closeWriters();
	// close reader
	closeReader();

	clock_t stop_time = clock();

	// close
	std::cout << "Read duration (sec): " << (float)myWorkQueue->getReadDuration() / CLOCKS_PER_SEC << std::endl;
	std::cout << "Parser duration (sec): " << (float)myWorkQueue->getParseDuration() / CLOCKS_PER_SEC << std::endl;
	std::cout << "Scrub duration (sec): " << (float)myWorkQueue->getScrubDuration() / CLOCKS_PER_SEC << std::endl;
	std::cout << "Signal Write duration (sec): " << (float)myWorkQueue->getSignalWriteDuration() / CLOCKS_PER_SEC << std::endl;
	std::cout << "Error Write duration (sec): " << (float)myWorkQueue->getErrorWriteDuration() / CLOCKS_PER_SEC << std::endl;
	std::cout << "Total Elapsed (sec): " << (float)(stop_time - start_time) / CLOCKS_PER_SEC << std::endl;

	return true;
}