#include "DateRangeScrubber.h"

DataItemPtr& DateRangeScrubber::operator() (DataItemPtr& data)
{
	if (data->getErrorType() == DataItem::NO_ERROR) {
		// checks that the data item is whithin the date range.

		// extract the date part
		std::string date = data->getTimeStamp().substr(0, 8);  // date part

		// is our record too old?
		if (date.compare(minDate) < 0) {
			data->setErrorType(DataItem::BAD_DATE);
		}

		// or too young?
		if (!maxDate.empty() && date.compare(maxDate) > 0) {
			data->setErrorType(DataItem::BAD_DATE);
		}
	}

	return data;

}







