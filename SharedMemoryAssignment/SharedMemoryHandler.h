#pragma once
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "structures.h"
	enum Files
	{
		MessageFile,
		InformationFile

	};
class SharedMemoryHandler
{
	private:
		LPCWSTR msgFileName			  = (LPCWSTR)TEXT("Global\sharedMsgFile");	  // the file to be manipulated
		LPCWSTR msgMutexName		  = (LPCWSTR)TEXT("Global\sharedMsgMutex");	  // the the mutex to be used when synchronizing data

		LPCWSTR infoFileName		  = (LPCWSTR)TEXT("Global\sharedInfoFile");	  // the file to be manipulated
		LPCWSTR infomutexName		  = (LPCWSTR)TEXT("Global\sharedInfoMutex");  // the the mutex to be used when synchronizing data

		LPCWSTR writeEventName		  = (LPCWSTR)TEXT("Global\writeEvent");		  // the event handle to the writing event
	
	protected:



		//// Variables for the message file
		HANDLE hMsgMapFile;			 // file  Handle
		HANDLE hMsgMutex;			 // mutex Handle
		LPCTSTR pMsgbuf;			 // File View
		// Variables for the Info file
		HANDLE hInfoMapFile;		 // file  Handle
		HANDLE hInfoMutex;			 // mutex Handle
		LPCTSTR pInfobuf;			 // File View


		
		
		HANDLE hEventThread;		 // thread handle for events (not in use yet)

		HANDLE hWriteEvent;			 // writeEvent Handle

	
		unsigned int numProcesses = 0; //the amount of connected processes
		bool running			  = true;
	
		virtual bool SetUpEventHandling(bool errorflag) = 0;
		virtual void HandleEvents() = 0;
	
	
		LPCWSTR GetFileName(Files fileName);
		LPCWSTR GetMutexName(Files fileName);


		LPCWSTR GetWriteEventName()			 { return this->writeEventName; };

	
	public:
	
		SharedMemoryHandler();
		SharedMemoryHandler(CommandArgs& commands);
		virtual ~SharedMemoryHandler();
		virtual bool Exec() = 0;
	
};

