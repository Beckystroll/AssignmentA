#include "PriceRangeScrubber.h"

DataItemPtr& PriceRangeScrubber::operator() (DataItemPtr& data)
{
	if (data->getErrorType() == DataItem::NO_ERROR) {

		// checks that the data item is whithin the price range.

		if (data->getPrice() < low || data->getPrice() > high) {
			data->setErrorType(DataItem::BAD_PRICE);
		}

	}

	return data;

}