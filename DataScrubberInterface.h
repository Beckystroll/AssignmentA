#pragma once

#include "DataItem.h"
#include <vector>

class DataScrubberInterface
{
public:
	virtual DataItemPtr& operator() (DataItemPtr& d) {
		return d;
	}
	DataItemPtr& scrub(DataItemPtr& d) {
		return d;
	}
};


typedef std::vector<DataScrubberInterface*> ScrubberVector;