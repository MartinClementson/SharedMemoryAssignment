#pragma once
#include "SharedMemoryHandler.h"
#include "structures.h"
#include <iostream>
#include <string>
class Consumer : public SharedMemoryHandler
{

private:
	
	DWORD ReadFromMemory();


protected:
	bool SetUpEventHandling(bool errorflag);
	 void HandleEvents();
public:
	Consumer(CommandArgs& commands);
	Consumer();
	~Consumer();


	bool Exec();
};

