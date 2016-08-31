#pragma once
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "structures.h"
class SharedMemoryHandler
{
	private:
		LPCWSTR fileName		   = (LPCWSTR)TEXT("Global\sharedFile");	// the file to be manipulated
		LPCWSTR mutexName		   = (LPCWSTR)TEXT("Global\sharedMutex");	// the the mutex to be used when synchronizing

		LPCWSTR writeEventName     = (LPCWSTR)TEXT("Global\writeEvent");	// the event handle to the writing event
		LPCWSTR processConnectName = (LPCWSTR)TEXT("Global\connectEvent");  // the event handle to the connect event
protected:
	HANDLE hMutex;		  // mutex Handle
	HANDLE hWriteEvent;   // writeEvent Handle
	HANDLE hConnectEvent; // connectEvent Handle
	HANDLE hMapFile;	  // file  Handle
	LPCTSTR pbuf;		  //File View





	LPCWSTR GetFileName()		    { return this->fileName;  };
	LPCWSTR GetMutexName()		    { return this->mutexName; };
	LPCWSTR GetWriteEventName()		{ return this->writeEventName; };
	LPCWSTR GetConnectEventName()   { return this->processConnectName; };
public:
	SharedMemoryHandler();
	SharedMemoryHandler(CommandArgs& commands);
	virtual ~SharedMemoryHandler();
	virtual void Exec() = 0;
};

