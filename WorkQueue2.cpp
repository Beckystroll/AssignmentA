#include "WorkQueue2.h"

void WorkQueue2::completedRead(IntPair ids) {
	std::unique_lock<std::mutex> lock(ParseMutex_);
	parse_.push_back(ids);
	parseToBeDone_.notify_one();
}

void WorkQueue2::completedLastRead() {
	// doesn't really need a mutex here as we only one read thread but added incase this ever changes
	std::unique_lock<std::mutex> lock(CheckCompleteMutex_);
	isComplete = true;
	lock.unlock();
	// need to an an entry back on read queue so that the check in then writer works
	std::unique_lock<std::mutex> lock2(ReadMutex_);
	IntPair myPair(0, 0);
	read_.push_back(myPair);

}

void WorkQueue2::completedParse(IntPair ids) {
	std::unique_lock<std::mutex> lock(Scrub1Mutex_);
	scrub1_.push_back(ids);
	scrub1ToBeDone_.notify_one();
}

void WorkQueue2::completedScrub1(IntPair ids) {
	std::unique_lock<std::mutex> lock(Scrub2Mutex_);
	scrub2_.push_back(ids);
	scrub2ToBeDone_.notify_one();
}

void WorkQueue2::completedScrub2(IntPair ids) {
	std::unique_lock<std::mutex> lock(Scrub3Mutex_);
	scrub3_.push_back(ids);
	scrub3ToBeDone_.notify_one();
}

void WorkQueue2::completedScrub3(IntPair ids) {
	std::unique_lock<std::mutex> lock(SortMutex_);
	sort_.push_back(ids);
	sortToBeDone_.notify_one();
}

void WorkQueue2::completedSort(IntPair ids) {
	std::unique_lock<std::mutex> lock(DuplicateMutex_);
	duplicate_.push_back(ids);
	duplicateToBeDone_.notify_one();
}


void WorkQueue2::completedDuplicate(IntPair ids) {
	std::unique_lock<std::mutex> lock(SigWriteMutex_);
	sigWrite_.push_back(ids);
	sigWriteToBeDone_.notify_one();
}

void WorkQueue2::completedSigWrite(IntPair ids) {
	std::unique_lock<std::mutex> lock(ErrWriteMutex_);
	errWrite_.push_back(ids);
	errWriteToBeDone_.notify_one();
}


bool WorkQueue2::completedErrWrite(IntPair ids) {
	// return boolean stating whether we are finished or not.

	std::unique_lock<std::mutex> lock2(ReadMutex_);
	read_.push_back(ids);
	lock2.unlock();

	// upadate err writer id - so that we can check for completion
	std::unique_lock<std::mutex> lock(CheckCompleteMutex_);
	bool weAreDone = isComplete && read_.size() == maxConcurrentThreads;
	if (weAreDone) {
		isErrWriterComplete = true;
		std::cout << "ALL COMPLETE" << std::endl;
		return (true);
	}
	lock.unlock();

	// add our block number to reader deque for reuse and notify reader
	lock2.lock();
	readToBeDone_.notify_one();

	return false;
}


IntPair WorkQueue2::nextRead() {
	std::unique_lock<std::mutex> lock(ReadMutex_);
	while (read_.empty())
		readToBeDone_.wait(lock);

	IntPair tmp = read_.front();
	read_.pop_front();
	return(tmp);

}



IntPair WorkQueue2::nextParse() {
	std::unique_lock<std::mutex> lock(ParseMutex_);

	while (parse_.empty() && !isErrWriterComplete)
		parseToBeDone_.wait_for(lock, WAIT_DURATION_1MS);

	if (isErrWriterComplete)
		return(COMPLETED_PAIR);

	IntPair tmp = parse_.front();
	parse_.pop_front();
	return(tmp);
}


IntPair WorkQueue2::nextScrub1() {
	std::unique_lock<std::mutex> lock(Scrub1Mutex_);

	while (scrub1_.empty() && !isErrWriterComplete)
		scrub1ToBeDone_.wait_for(lock, WAIT_DURATION_1MS);

	if (isErrWriterComplete)
		return(COMPLETED_PAIR);

	IntPair tmp = scrub1_.front();
	scrub1_.pop_front();
	return(tmp);
}

IntPair WorkQueue2::nextScrub2() {
	std::unique_lock<std::mutex> lock(Scrub2Mutex_);

	while (scrub2_.empty() && !isErrWriterComplete)
		scrub2ToBeDone_.wait_for(lock, WAIT_DURATION_1MS);

	if (isErrWriterComplete)
		return(COMPLETED_PAIR);

	IntPair tmp = scrub2_.front();
	scrub2_.pop_front();
	return(tmp);
}

IntPair WorkQueue2::nextScrub3() {
	std::unique_lock<std::mutex> lock(Scrub3Mutex_);

	while (scrub3_.empty() && !isErrWriterComplete)
		scrub3ToBeDone_.wait_for(lock, WAIT_DURATION_1MS);

	if (isErrWriterComplete)
		return(COMPLETED_PAIR);

	IntPair tmp = scrub3_.front();
	scrub3_.pop_front();
	return(tmp);
}



IntPair WorkQueue2::nextSort() {
	std::unique_lock<std::mutex> lock(SortMutex_);

	while (sort_.empty() && !isErrWriterComplete)
		sortToBeDone_.wait_for(lock, WAIT_DURATION_1MS);

	if (isErrWriterComplete)
		return(COMPLETED_PAIR);

	IntPair tmp = sort_.front();
	sort_.pop_front();
	return(tmp);

}

IntPair WorkQueue2::nextDuplicate() {
	std::unique_lock<std::mutex> lock(DuplicateMutex_);

	while (duplicate_.empty() && !isErrWriterComplete)
		duplicateToBeDone_.wait_for(lock, WAIT_DURATION_1MS);

	if (isErrWriterComplete)
		return(COMPLETED_PAIR);

	IntPair tmp = duplicate_.front();
	duplicate_.pop_front();
	return(tmp);

}

IntPair WorkQueue2::nextSigWrite() {
	std::unique_lock<std::mutex> lock(SigWriteMutex_);

	while (sigWrite_.empty() && !isErrWriterComplete)
		sigWriteToBeDone_.wait_for(lock, WAIT_DURATION_1MS);

	if (isErrWriterComplete)
		return(COMPLETED_PAIR);

	IntPair tmp = sigWrite_.front();
	sigWrite_.pop_front();
	return(tmp);

}

IntPair WorkQueue2::nextErrWrite() {
	std::unique_lock<std::mutex> lock(ErrWriteMutex_);

	while (errWrite_.empty())
		errWriteToBeDone_.wait_for(lock, WAIT_DURATION_1MS);

	IntPair tmp = errWrite_.front();
	errWrite_.pop_front();
	return(tmp);

}

void WorkQueue2::updateReadDuration(clock_t duration) {
	std::unique_lock<std::mutex> lock(TimeUpdate_);
	read_duration += duration;
}

void WorkQueue2::updateParseDuration(clock_t duration) {
	std::unique_lock<std::mutex> lock(TimeUpdate_);
	parse_duration += duration;
}
void WorkQueue2::updateScrubDuration(clock_t duration) {
	std::unique_lock<std::mutex> lock(TimeUpdate_);
	scrub_duration += duration;
}
void WorkQueue2::updateErrWriteDuration(clock_t duration) {
	std::unique_lock<std::mutex> lock(TimeUpdate_);
	error_write_duration += duration;
}
void WorkQueue2::updateSigWriteDuration(clock_t duration) {
	std::unique_lock<std::mutex> lock(TimeUpdate_);
	signal_write_duration += duration;
}