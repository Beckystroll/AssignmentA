#pragma once


#include <stdio.h>
#include <string>

#include "DataItem.h"
#include "ItemGroup.h"


class BigDataWriter
{

private:

	std::string fileName;
	bool errorInd;
	FILE* pFile;

public:
	BigDataWriter(std::string fileName_, bool errorInd_) : fileName(fileName_), errorInd(errorInd_) {}

	bool writeDataItem( DataItem& data);
	bool writeDataItems(ItemGroup* itemGroup);
	bool deleteFile();
	bool open();
	void close();

	virtual ~BigDataWriter(void);
};



