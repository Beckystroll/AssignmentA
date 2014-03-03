#include "ItemGroup.h"

ItemGroup::ItemGroup(int groupNo_, long noElements_) : groupNo(groupNo_), noElements(noElements_)
{
	// our constructor

	// creates our vector - it will have a long life - until the program ends

	itemVector = new std::vector<DataItemPtr>(noElements_);

	// initialise the elements
	for (int i = 0; i < noElements_; i++) {
		(*itemVector)[i] = new DataItem();
	}


}

ItemGroup::~ItemGroup()
{
	// our destructor

	for (int i = 0; i < noElements; i++) {
		delete (*itemVector)[i] ;
	}

	delete itemVector;
}

void ItemGroup::reset()
{
	// resets the data structures 

	for (int i = 0; i < noElements; i++) {
		(*itemVector)[i]->reset();
	}

}

DataItem& ItemGroup::operator[](const int index) {

	if (index < noElements)
		return (*(*itemVector)[index]);

}