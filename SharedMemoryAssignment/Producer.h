#pragma once
#include "SharedMemoryHandler.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <time.h>


class Producer : public SharedMemoryHandler
{

	struct SessionInfo
	{
		unsigned int numMessages  = 0;
		unsigned int messagesGenerated = 0; //used to keep track of message ids,
		unsigned int messagesSent = 0;
		bool random				  = false;
		unsigned int msgSize	  = 0;
		double msDelay			  = 0;
		SessionInfo() {};

		SessionInfo(CommandArgs commands)
		{
			this->messagesSent  = 0;
			this->numMessages   = commands.numMessages;
			this->random		= commands.random;
			this->msgSize		= commands.msgSize;
			this->msDelay		= commands.msDelay;
		}
		bool MessagesToSend()
		{
			if (this->messagesSent < this->numMessages)
				return true; //there are messages to send
			else
				return false; //all messages have been sent
		}
	};

private:
	SessionInfo sessionInfo;
	
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

private:
	SharedData::SharedMessage GenerateRndMessage();
};

