#pragma once
#include "DataScrubberInterface.h"
#include "DataItem.h"


class PriceRangeScrubber :
	public DataScrubberInterface
{
public:
	PriceRangeScrubber(double low_, double high_) : low(low_), high(high_) {}
	virtual ~PriceRangeScrubber() {}

	DataItemPtr& operator() (DataItemPtr& data);

private:
	double low;
	double high;
};
