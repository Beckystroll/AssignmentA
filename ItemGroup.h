#pragma once

#include <vector>
#include "DataItem.h"

/* this class stores a group of data items */

class ItemGroup
{
public:

	ItemGroup(int groupNo_, long noElements_);
	virtual ~ItemGroup();
	void reset();
	DataItem& operator[](const int index);
	std::vector<DataItemPtr>& getItemVector() { return *itemVector; }

private:

	int groupNo;
	long noElements;
	std::vector<DataItemPtr>* itemVector;  // a vector of noElements pointers to T

};

