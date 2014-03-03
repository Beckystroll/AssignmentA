#include "DataItem.h"

#include <iostream>
#include <boost/algorithm/string.hpp>


#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>

using namespace std;


std::string DataItem::errorTypesArray[] = { "NO_ERROR", "DUPLICATE", "BAD_DATE", "BAD_TIME", "BAD_PRICE", "BAD_FORMAT", "OUT_OF_RANGE", "TOO_MUCH_DATA" };

std::string DataItem::toString() {
	return "DataItem: " + line + ",errorType: " + std::to_string(errorType);
}

bool DataItem::operator< (const DataItem& rhs) const {

	return (timeStamp.compare(rhs.timeStamp) < 0);

}

bool DataItem::operator== (const DataItem& rhs) const {

	return(line == rhs.line);

}


ostream& operator<<(std::ostream& out, const DataItem& item) {

	out << item.line;
	if (item.isError())
		out << ",ERROR:" << DataItem::errorTypesArray[item.errorType];
	out << endl;

	return(out);
}

ostream& operator<<(std::ostream& out, const DataItem* item) {

	out << *item;

	return(out);
}


void DataItem::parse() {

	/* we are expecting exactly 3 tokens:
	1) timestamp - string
	2) price - double
	3) quantity - long

	its an error if we don't get the above

	*/

	//boost::trim(line);
	errorType = DataItem::NO_ERROR;

	// find the commas
	int first = line.find_first_of(DELIMITER, 0);
	if (first == string::npos)
		errorType = DataItem::BAD_FORMAT;
	else {
		int second = line.find_first_of(DELIMITER, first + 1);
		if (second == string::npos)
			errorType = DataItem::BAD_FORMAT;
		else {

			try {

				timeStamp = line.substr(0, first);
				price = stof(line.substr(first + 1, second));
				quantity = stol(line.substr(second + 1));
			}
			catch (const std::invalid_argument& ia) {
				errorType = BAD_FORMAT;
				cerr << "Line: " << line << " - Invalid argument: " << ia.what() << endl;
			}
			catch (const std::out_of_range& oor) {
				errorType = OUT_OF_RANGE;
				cerr << "Line: " << line << " - Out of Range error: " << oor.what() << endl;
			}

		}

	}

	// don't care if the line contains additional data - only care about 1st 3 fields.


}