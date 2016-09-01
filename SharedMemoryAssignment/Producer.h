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
public:


	Producer();
	Producer(CommandArgs arguments);
	~Producer();
	DWORD WriteToMemory(int number);
	void Exec();
};

