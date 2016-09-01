#pragma once


struct CommandArgs
{
	unsigned int numMessages		= 0;
	unsigned int memorySize = 0;

	bool producer = false;
	bool random	  = false;
	unsigned int msgSize	= 0;

	double msDelay;


};