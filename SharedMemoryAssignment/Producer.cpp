#include "Producer.h"
#include <atlstr.h>


DWORD Producer::WriteToMemory(int number)
{
	DWORD waitResult;
	
	waitResult = WaitForSingleObject(		//Get the control of the mutex
		hMutex,					// handle to mutex
		INFINITE);				// no time out interval!

	switch (waitResult)
	{
	case WAIT_OBJECT_0: //Got ownership of the mutex
		__try {
			//write to database
			//TCHAR szMsg[] = (LPCWSTR)text.c_str(); // the file to be manipulated
			
			std::cout << number << std::endl;
			//CopyMemory((PVOID)pbuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
			ZeroMemory((PVOID)pbuf,10);
			memcpy((PVOID)pbuf, &number,sizeof(int));
		}
		__finally
		{
			//release ownership of the mutex
			
			if (!ReleaseMutex(hMutex))
				MessageBox(NULL, TEXT("COULD NOT RELEASE MUTEX!"), TEXT("Obsessed handle"), MB_OK);
			if(!SetEvent(this->hWriteEvent))
				MessageBox(NULL, TEXT("COULD NOT Trigger Write event!"), TEXT("DANGER"), MB_OK);
			ResetEvent(this->hWriteEvent); //Reset the signal directly after it has been sent out!
		}
		break;

	case WAIT_ABANDONED: //got ownership of an abandoned mutex object
		return FALSE;
	

	}
	//CopyMemory((PVOID)pbuf, text, sizeof(text));
	//FlushViewOfFile() // flush it to the file

	return TRUE;
 }

void Producer::Exec()
{

	for (int i = 0; i < 500; i++)
	{
		Sleep(500);
		if (WriteToMemory(i)== FALSE)
			MessageBox(NULL, TEXT("Could not write to memory"), TEXT("HELP"), MB_OK);
	}
}

Producer::Producer()
{
}

Producer::Producer(CommandArgs arguments)
{
#pragma region Create file and mapping
	this->hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		(arguments.memorySize * 1000000), //convert to megabyte
		GetFileName()
		);

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("FAILED!"));
		DebugBreak();
	}


	pbuf = (LPTSTR)MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		arguments.memorySize* 1000000);

	if (pbuf == NULL)
	{
		_tprintf(TEXT("FAILED!"));
		CloseHandle(hMapFile);
		DebugBreak();
	}
	TCHAR szMsg[] = TEXT("This text is sent to the file");

	CopyMemory((PVOID)pbuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

	//getchar();
	//
	//TCHAR szMsgg[] = TEXT("This is the updated text!");
	//
	//CopyMemory((PVOID)pbuf, szMsgg, (_tcslen(szMsgg) * sizeof(TCHAR)));
#pragma endregion


#pragma region Create Mutex
	

		hMutex = CreateMutex(
			NULL,
			FALSE,
			this->GetMutexName());
		if (hMutex == NULL)
			MessageBox(NULL, TEXT("Error creating mutex")+ GetLastError(), TEXT("CRY"), MB_OK);
		// Keep this process around until the second process is run
		//_getch();
#pragma endregion


#pragma region Create Events

		hWriteEvent = CreateEvent(
			NULL,				 //default security attr
			TRUE,				 //manual reset event
			FALSE,			     // non signaled when initializing
			GetWriteEventName()  // name
			);
		if (hWriteEvent == NULL)
			MessageBox(NULL, TEXT("Could not init writeEvent"), TEXT("Abandon hope"), MB_OK);
#pragma endregion
}


Producer::~Producer()
{
//	UnmapViewOfFile(pbuf);
	//CloseHandle(hMapFile);

}
