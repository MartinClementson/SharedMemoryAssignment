#pragma once
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "structures.h"
class SharedMemoryHandler
{
	private:
		LPCWSTR fileName			  = (LPCWSTR)TEXT("Global\sharedFile");		  // the file to be manipulated
		LPCWSTR mutexName			  = (LPCWSTR)TEXT("Global\sharedMutex");	  // the the mutex to be used when synchronizing data
		LPCWSTR writeEventName		  = (LPCWSTR)TEXT("Global\writeEvent");		  // the event handle to the writing event
		LPCWSTR processConnectName	  = (LPCWSTR)TEXT("Global\connectEvent");     // the event handle to the connect event
		LPCWSTR processDisconnectName = (LPCWSTR)TEXT("Global\disconnectEvent");  // the event handle to the disconnect event

	
	protected:
		HANDLE hMutex;				// mutex Handle
		HANDLE hEventThread;		// thred handle for events

		HANDLE hCloseEvent;			// console close event
		HANDLE hWriteEvent;			// writeEvent Handle
		HANDLE hConnectEvent;		// connectEvent Handle
		HANDLE hDisconnectEvent;	// DisconnectEvent Handle
		HANDLE hMapFile;		    // file  Handle
		LPCTSTR pbuf;				// File View
	
		unsigned int numProcesses = 0; //the amount of connected processes
		bool running = true;
	
		virtual bool SetUpEventHandling(bool errorflag) = 0;
		virtual void HandleEvents() = 0;
	
	
		LPCWSTR GetFileName()		     { return this->fileName;  };
		LPCWSTR GetMutexName()		     { return this->mutexName; };
		LPCWSTR GetWriteEventName()		 { return this->writeEventName; };
		LPCWSTR GetConnectEventName()    { return this->processConnectName; };
		LPCWSTR GetDisconnectEventName() { return this->processDisconnectName; }

	
	public:
	
		SharedMemoryHandler();
		SharedMemoryHandler(CommandArgs& commands);
		virtual ~SharedMemoryHandler();
		virtual bool Exec() = 0;
	
};

