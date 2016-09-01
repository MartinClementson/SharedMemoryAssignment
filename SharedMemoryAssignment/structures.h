#pragma once

#define CLOSE_EVENT_NAME (LPCWSTR)TEXT("Local\CloseEvent")

struct CommandArgs
{
	unsigned int numMessages		= 0;
	unsigned int memorySize = 0;

	bool producer = false;
	bool random	  = false;
	unsigned int msgSize	= 0;

	double msDelay;


};

struct SharedInformation
{
	unsigned int numProcesses = 0;


};