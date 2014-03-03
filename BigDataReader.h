#pragma once

#include <string>
#include <list>
#include <fstream>
#include <memory>
#include <vector>

#include "DataScrubberInterface.h"
#include "DataItem.h"
#include "TypeDefs.h"
#include "ItemGroup.h"

class BigDataReader
{



public:
	BigDataReader(std::string fileName_, long noRows_) : fileName(fileName_), noRows(noRows_) {}
	BigDataReader() {}

	int  getDataItems(ItemGroup* dataGroup);

	bool open();
	void close();
	bool isEOF();
	~BigDataReader(void);

private:

	std::string fileName;
	std::ifstream fileStream;
	int noRows; // at a time
};