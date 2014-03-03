#pragma once
#include "DataScrubberInterface.h"
#include "DataItem.h"

// must have a min date
// max date is optional

class DateRangeScrubber:public DataScrubberInterface
{
public:
	DateRangeScrubber(std::string minDate_, std::string maxDate_ = std::string()) : minDate(minDate_), maxDate(maxDate_) {}
	virtual ~DateRangeScrubber() {}

	DataItemPtr& operator() (DataItemPtr& data);

private:
	std::string minDate;
	std::string maxDate;
};

