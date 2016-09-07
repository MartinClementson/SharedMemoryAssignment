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
	private:
		LPCWSTR msgFileName			  = (LPCWSTR)TEXT("sharedMsgFile");		  // the file to be manipulated
		LPCWSTR msgMutexName		  = (LPCWSTR)TEXT("sharedMsgMutex");	  // the the mutex to be used when synchronizing data							
		LPCWSTR infoFileName		  = (LPCWSTR)TEXT("sharedInfoFile");	  // the file to be manipulated
		LPCWSTR infomutexName		  = (LPCWSTR)TEXT("sharedInfoMutex");	  // the the mutex to be used when synchronizing data											
		LPCWSTR writeEventName		  = (LPCWSTR)TEXT("writeEvent");		  // the event handle to the writing event


	protected:

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

