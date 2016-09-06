#pragma once
#include "SharedMemoryHandler.h"
#include <string>
#include <stdlib.h>
#include <iostream>
class Producer : public SharedMemoryHandler
{
private:

protected:
	bool SetUpEventHandling(bool errorflag);

	 void HandleEvents();
	 bool ReadSharedInformation(); // use to access the shared info memory and update info
public:


	Producer();
	Producer(CommandArgs& arguments);
	~Producer();
	DWORD WriteToMemory(int number);
	bool Exec();
};

