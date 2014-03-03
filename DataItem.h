#pragma once

#include <string>
#include <fstream>

class DataItem
{

public:

	friend class BigDataReader;

	enum ErrorTypes { NO_ERROR, DUPLICATE, BAD_DATE, BAD_TIME, BAD_PRICE, BAD_FORMAT, OUT_OF_RANGE, TOO_MUCH_DATA };

	static std::string errorTypesArray[]; 

	static const char DELIMITER = ',';

	DataItem(std::string line_) : line(line_), errorType(NO_ERROR) {}
	DataItem() : line(""), timeStamp(""), price(0.0), quantity(0), errorType(NO_ERROR) {}

	virtual ~DataItem(void) {}

	std::string getLine() { return line; }
	void setLine(std::string line_) { line = line_; }

	std::string getTimeStamp() { return timeStamp; }
	void setTimeStamp(std::string timestamp_) { timeStamp = timestamp_; }

	float getPrice() { return price; }
	void setPrice(float price_) { price = price_; }

	long getQuantity() { return quantity; }
	void setQuantity(long quantity_) { quantity = quantity_; }

	ErrorTypes getErrorType() { return errorType; }
	void setErrorType(ErrorTypes errorType_) { errorType = errorType_; }
	bool isError() const { return (errorType != NO_ERROR); }

	std::string toString();
	void reset() { line = ""; timeStamp = ""; price = 0.0;  quantity = 0; errorType = NO_ERROR; }
	void parse();

	bool operator< (const DataItem &rhs) const;
	bool operator== (const DataItem &rhs) const;
	friend std::ostream& operator<<(std::ostream& out, const DataItem& item);
	friend std::ostream& operator<<(std::ostream& out, const DataItem* item);


private:

	std::string line; // the original data - will pass this back to output file.
	std::string timeStamp;  // stores timeStamp as string in format YYYYMMDD:HH:MM:SS.SSSSSS (1 microsecond resolution)
	float price;  // stores price of asset - may be negative or 0.  to 6dp resolution
	long quantity; // quantity traded of assets
	ErrorTypes errorType; // the type of error

};

typedef DataItem* DataItemPtr;