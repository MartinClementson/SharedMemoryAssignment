#pragma once
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "structures.h"
class SharedMemoryHandler
{
	private:
		LPCWSTR fileName		= (LPCWSTR)TEXT("Global\sharedFile"); // the file to be manipulated
		LPCWSTR mutexName		= (LPCWSTR)TEXT("Global\sharedMutex"); // the the mutex to be used when synchronizing
		LPCWSTR writeEventName  = (LPCWSTR)TEXT("Global\writeEvent"); // the the mutex to be used when synchronizing
protected:
	HANDLE hMapFile;	// file  Handle
	HANDLE hMutex;		// mutex Handle
	HANDLE hWriteEvent;  // writeEvent Handle
	LPCTSTR pbuf;    //File View





	LPCWSTR GetFileName()		{ return this->fileName;  };
	LPCWSTR GetMutexName()		{ return this->mutexName; };
	LPCWSTR GetWriteEventName() { return this->writeEventName; };
public:
	SharedMemoryHandler();
	SharedMemoryHandler(CommandArgs& commands);
	virtual ~SharedMemoryHandler();
	virtual void Exec() = 0;
};

