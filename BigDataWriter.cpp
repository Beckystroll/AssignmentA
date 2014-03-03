#include "BigDataWriter.h"

//#include <algorithm>
#include <cstdio>
#include <iostream>


BigDataWriter::~BigDataWriter()
{
	close();
}

bool BigDataWriter::deleteFile() {

	// removes the file
	int ret_code = std::remove(fileName.c_str());
	return (ret_code == 0);

}

bool BigDataWriter::open() {

	// opens the file

	pFile = fopen(fileName.c_str(), "w");  // try b later

	// and returns if successful
	return (pFile != NULL);

}

void BigDataWriter::close() {

	if (pFile != NULL) {
		fclose(pFile);
	}

}

bool BigDataWriter::writeDataItems(ItemGroup* itemGroup) {

	if (pFile != NULL) {

		DataItem& item = DataItem();
		int size = itemGroup->getItemVector().size();
				
		for (int i = 0; i < size; i++) {
			item = (*itemGroup)[i];
			if (errorInd == item.isError()) {
				fputs(item.getLine().c_str(), pFile);
				if (item.isError()) {
					fputs(",ERROR:", pFile);
					fputs(DataItem::errorTypesArray[item.getErrorType()].c_str(), pFile);
				}
				fputc('\n', pFile);
			}
		}

	}
	return true;
}


