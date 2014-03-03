#include "BigDataReader.h"


#include <iostream>


BigDataReader::~BigDataReader(void)
{
	close();
}



bool BigDataReader::open() {

	// opens the file

	fileStream = std::ifstream(fileName);

	// and returns if successful
	return (fileStream.is_open());

}

void BigDataReader::close() {

	if (!fileStream == false && fileStream.is_open()) {
		fileStream.close();
	}

}

bool BigDataReader::isEOF() {
	return (fileStream.eof());

}




int BigDataReader::getDataItems(ItemGroup* dataGroup) {

	std::string tmp;
	int count = 0;
	DataItemPtr itemPtr = nullptr;

	while (!fileStream.eof() && count < noRows) {

		itemPtr = &(*dataGroup)[count++];
		getline(fileStream, itemPtr->line, '\n');                 // Grab the next line

	}

	return count;

}