#pragma once

#include <deque>
#include <string>
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <iostream>
#include <chrono>
#include <utility>


typedef std::pair<int, int> IntPair;

class WorkQueue2 {
public:

	const std::chrono::milliseconds WAIT_DURATION_1MS = std::chrono::milliseconds(1);
	const IntPair COMPLETED_PAIR = IntPair(-1, -1);

	WorkQueue2(int maxConcurrentThreads_) : maxConcurrentThreads(maxConcurrentThreads_), read_duration(0), parse_duration(0), scrub_duration(0), error_write_duration(0), signal_write_duration(0)
	{
		// set up initial available block numbers for the reader
		for (int i = 0; i < maxConcurrentThreads_; i++) {
			IntPair myPair(i, i+1);
			read_.push_back(myPair);
		}
	}
	~WorkQueue2() {}

	void completedRead(IntPair ids);
	void completedLastRead();
	void completedParse(IntPair ids);
	void completedScrub1(IntPair ids);
	void completedScrub2(IntPair ids);
	void completedScrub3(IntPair ids);
	void completedSort(IntPair ids);
	void completedDuplicate(IntPair ids);
	void completedSigWrite(IntPair ids);
	bool completedErrWrite(IntPair ids);

	IntPair nextRead();
	IntPair nextParse();
	IntPair nextScrub1();
	IntPair nextScrub2();
	IntPair nextScrub3();
	IntPair nextSort();
	IntPair nextDuplicate();
	IntPair nextSigWrite();
	IntPair nextErrWrite();

	void updateReadDuration(clock_t duration);
	void updateParseDuration(clock_t duration);
	void updateScrubDuration(clock_t duration);
	void updateErrWriteDuration(clock_t duration);
	void updateSigWriteDuration(clock_t duration);

	clock_t getReadDuration(){ return read_duration; }
	clock_t getParseDuration(){ return parse_duration; }
	clock_t getScrubDuration(){ return scrub_duration; }
	clock_t getSignalWriteDuration(){ return signal_write_duration; }
	clock_t getErrorWriteDuration(){ return error_write_duration; }
	
private:
	int maxConcurrentThreads;
	bool isComplete = false;
	bool isErrWriterComplete = false;

	std::deque<IntPair> read_;
	std::deque<IntPair> parse_;
	std::deque<IntPair> scrub1_;
	std::deque<IntPair> scrub2_;
	std::deque<IntPair> scrub3_;
	std::deque<IntPair> sort_;
	std::deque<IntPair> duplicate_;
	std::deque<IntPair> sigWrite_;
	std::deque<IntPair> errWrite_;

	mutable std::mutex ReadMutex_;
	mutable std::mutex ParseMutex_;
	mutable std::mutex Scrub1Mutex_;
	mutable std::mutex Scrub2Mutex_;
	mutable std::mutex Scrub3Mutex_;
	mutable std::mutex SortMutex_;
	mutable std::mutex DuplicateMutex_;
	mutable std::mutex SigWriteMutex_;
	mutable std::mutex ErrWriteMutex_;
	mutable std::mutex CheckCompleteMutex_;
	mutable std::mutex TimeUpdate_;

	std::condition_variable readToBeDone_;
	std::condition_variable parseToBeDone_;
	std::condition_variable scrub1ToBeDone_;
	std::condition_variable scrub2ToBeDone_;
	std::condition_variable scrub3ToBeDone_;
	std::condition_variable sortToBeDone_;
	std::condition_variable duplicateToBeDone_;
	std::condition_variable sigWriteToBeDone_;
	std::condition_variable errWriteToBeDone_;

	clock_t read_duration;
	clock_t parse_duration;
	clock_t scrub_duration;
	clock_t signal_write_duration;
	clock_t error_write_duration;


};