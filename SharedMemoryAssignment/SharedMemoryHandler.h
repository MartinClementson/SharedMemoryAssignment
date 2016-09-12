#pragma once
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "structures.h"
#include "SharedMutex.h"
#include <memory>
#include "CircleBuffer.h"
	enum Files
	{
		MessageFile,
		InformationFile

	};

class SharedMemoryHandler
{
protected:
	struct SessionInfo
	{
		unsigned int numMessages = 0;
		unsigned int messagesGenerated = 0; //used to keep track of message ids,
		unsigned int messagesSent = 0;
		unsigned int messagesRecieved = 0;
		bool random = false;
		unsigned int msgSize = 0;
		double msDelay = 0;
		SessionInfo() {};

		SessionInfo(CommandArgs commands)
		{
			this->messagesSent = 0;
			this->messagesRecieved = 0;
			this->numMessages = commands.numMessages;
			this->random = commands.random;
			this->msgSize = commands.msgSize;
			this->msDelay = commands.msDelay;
		}
		bool MessagesToSend()
		{
			if (this->messagesSent < this->numMessages)
				return true;  //there are messages to send
			else
				return false; //all messages have been sent
		}
	};
	private:
		LPCWSTR msgFileName			  = (LPCWSTR)TEXT("sharedMsgFile");		  // the file to be manipulated
		LPCWSTR msgMutexName		  = (LPCWSTR)TEXT("sharedMsgMutex");	  // the the mutex to be used when synchronizing data							
		LPCWSTR infoFileName		  = (LPCWSTR)TEXT("sharedInfoFile");	  // the file to be manipulated
		LPCWSTR infomutexName		  = (LPCWSTR)TEXT("sharedInfoMutex");	  // the the mutex to be used when synchronizing data											
		LPCWSTR writeEventName		  = (LPCWSTR)TEXT("writeEvent");		  // the event handle to the writing event

	protected:
		SessionInfo sessionInfo;

		std::shared_ptr<SharedData::SharedMessage> localMsg = nullptr;		  //this could be evaluated to a vector, to be able to store many reads or writes locally
		std::unique_ptr<SharedMemory::CircleBuffer> messageBuffer;
		// Variables for the message file
		std::unique_ptr<SharedMemory::SharedMutex> msgMutex;  // mutex
		// Variables for the Info file
		std::unique_ptr<SharedMemory::SharedMutex> infoMutex; // mutex


		
		
		HANDLE hEventThread;		 // thread handle for events (not in use yet)

		HANDLE hWriteEvent;			 // writeEvent Handle

		size_t maxMessageLen		 = 0;
		unsigned int numProcesses	 = 0; //the amount of connected processes
		bool running				 = true;
	
		virtual bool SetUpEventHandling(bool errorflag) = 0;
		virtual void HandleEvents()						= 0;
	
	
		LPCWSTR GetFileName (Files fileName);
		LPCWSTR GetMutexName(Files fileName);
		LPCWSTR GetWriteEventName()	{ return this->writeEventName; };

	
	public:
	
		SharedMemoryHandler();
		SharedMemoryHandler(CommandArgs* arguments);
		virtual ~SharedMemoryHandler();
		virtual bool Exec() = 0;
	
};

