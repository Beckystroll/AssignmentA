#include "TimeRangeScrubber.h"


DataItemPtr& TimeRangeScrubber::operator() (DataItemPtr& data)
{
	if (data->getErrorType() == DataItem::NO_ERROR) {
		// checks that the data item is whithin the date range.

		// extract the date part
		int hour = std::stoi(data->getTimeStamp().substr(9, 2));  // date part

		// is our outside the range?
		if (hour < minTime || hour > maxTime) {
			data->setErrorType(DataItem::BAD_TIME);
		}

	}

	return data;

}