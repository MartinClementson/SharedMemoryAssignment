#pragma once
#include "SharedMemoryHandler.h"
#include "structures.h"
#include <iostream>
#include <string>
class Consumer : public SharedMemoryHandler
{

private:
	DWORD ReadFromMemory();
public:
	Consumer(CommandArgs commands);
	Consumer();
	~Consumer();


	void Exec();
};

