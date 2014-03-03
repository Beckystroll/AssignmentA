#pragma once
#include "DataScrubberInterface.h"
#include "DataItem.h"


class TimeRangeScrubber :
	public DataScrubberInterface
{
public:
	TimeRangeScrubber(int minTime_, int maxTime_ ) : minTime(minTime_), maxTime(maxTime_) {}
	virtual ~TimeRangeScrubber() {}

	DataItemPtr& operator() (DataItemPtr& data);

private:
	int minTime;
	int maxTime;
};


