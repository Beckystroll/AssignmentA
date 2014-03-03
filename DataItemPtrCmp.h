#pragma once

#include "DataItem.h"

class DataItemPtrCmp
{
public:
	int operator() (DataItemPtr& lhs, DataItemPtr& rhs) {
		return *lhs < *rhs;
	}
};

