#pragma once
#include "SharedMemoryHandler.h"
#include <string>
#include <stdlib.h>
#include <iostream>
class Producer : public SharedMemoryHandler
{
private:
public:
	Producer();
	Producer(CommandArgs arguments);
	~Producer();
	DWORD WriteToMemory(int number);
	void Exec();
};

