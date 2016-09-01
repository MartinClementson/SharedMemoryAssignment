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
			//CopyMemory((PVOID)pbuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
			std::cout << number << std::endl;
			
			ZeroMemory((PVOID)pbuf,10);
			memcpy((PVOID)pbuf, &number,sizeof(int));
		}
		__finally
		{
			if (!ReleaseMutex(hMutex))			// Release the ownership of the mutex so that other processes can access it
				MessageBox(NULL, TEXT("COULD NOT RELEASE MUTEX!"), TEXT("Obsessed handle"), MB_OK);

			if(!SetEvent(this->hWriteEvent))	//Signal that the writing is done!
				MessageBox(NULL, TEXT("COULD NOT Trigger Write event!"), TEXT("DANGER"), MB_OK);

			ResetEvent(this->hWriteEvent);		//Reset the signal directly after it has been sent out!
		}
		break;

	case WAIT_ABANDONED: //got ownership of an abandoned mutex object
		return FALSE;
	}
	return TRUE;
 }

void Producer::Exec()
{
	static int x = 0;
	
	if (this->numProcesses < 1)
	{

		DWORD waitResult = WaitForSingleObject( //Wait for a consumer to connect!
			hConnectEvent,
			INFINITE);
	
		switch (waitResult)
		{
			// Event object was signaled
		case WAIT_OBJECT_0:
			std::cout << "Consumer has connected, Beginning Data Transfer \n";
			this->numProcesses += 1;
				
			
	
		default:
			printf("Wait error (%d)\n", GetLastError());
			
		}

	}
	else
	{

		HandleEvents();
		Sleep(500);
		if (WriteToMemory(x) == FALSE)
			MessageBox(NULL, TEXT("Could not write to memory"), TEXT("HELP"), MB_OK);
		x++;
	}
	

} 

bool Producer::SetUpEventHandling(bool errorflag)
{


#pragma region Create Events

	hWriteEvent = CreateEvent(
		NULL,				 //default security attr
		TRUE,				 //manual reset event
		FALSE,			     // non signaled when initializing
		GetWriteEventName()  // name
		);
	if (hWriteEvent == NULL)
	{
		errorflag = true;
		MessageBox(NULL, TEXT("Could not init writeEvent"), TEXT("Abandon hope"), MB_OK);
	}



	hConnectEvent = CreateEvent(
		NULL,				 //default security attr
		FALSE,				 //automatic reset event
		FALSE,			     // non signaled when initializing
		GetConnectEventName()  // name
		);
	if (hConnectEvent == NULL)
	{
		errorflag = true;
		MessageBox(NULL, TEXT("Could not init connectEvent"), TEXT("Abandon hope"), MB_OK);
	}


	hDisconnectEvent = CreateEvent(
		NULL,				 //default security attr
		FALSE,				 //automatic reset event
		FALSE,			     // non signaled when initializing
		GetDisconnectEventName()  // name
		);
	if (hDisconnectEvent == NULL)
	{
		errorflag = true;
		MessageBox(NULL, TEXT("Could not init disconnect event"), TEXT("Abandon hope"), MB_OK);
	}


#pragma endregion




#pragma region Create the thread

	/*this->hEventThread = CreateThread(
		NULL,
		0,
		ExecEventHandler,
		NULL,
		)*/
#pragma endregion







	return errorflag;
}

void Producer::HandleEvents()
{


	DWORD waitResult = WaitForSingleObject( //peek if a consumer has connected
		hConnectEvent,
		1);
	if (waitResult == WAIT_OBJECT_0)
	{

		this->numProcesses += 1;
		std::cout << "Consumer has connected \n Total Amount of consumers :" << this->numProcesses << std::endl;
	}


	waitResult = WaitForSingleObject( //peek if a consumer has connected
		hDisconnectEvent,
		1);
	if (waitResult == WAIT_OBJECT_0)
	{

		this->numProcesses -= 1;
		std::cout << "Consumer has disconnected \n Total Amount of consumers :" << this->numProcesses << std::endl;
	}




}

Producer::Producer()
{
}

Producer::Producer(CommandArgs arguments)
{
	bool errorflag = false;
#pragma region Create file and mapping
	this->hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,	//Instead of a file in the system, we use a system paging file
		NULL,					//No extra attributes (default)
		PAGE_READWRITE,			//specifies the protection, all the views to the file need to på compatible with this!
		0,
		(arguments.memorySize * 1000000), //convert to megabyte
		GetFileName()
		);

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("FAILED!"));
		errorflag = true;
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
		//CloseHandle(hMapFile);
		errorflag = true;
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
		{
			errorflag = true;
			MessageBox(NULL, TEXT("Error creating mutex")+ GetLastError(), TEXT("CRY"), MB_OK);
		}
		// Keep this process around until the second process is run
		//_getch();
#pragma endregion

		errorflag = this->SetUpEventHandling(errorflag);

		if (errorflag == true)
			MessageBox(NULL, TEXT("ERROR CREATING THE PRODUCER"), TEXT("You have no power here"), MB_OK);

}


Producer::~Producer()
{
//	UnmapViewOfFile(pbuf);
	//CloseHandle(hMapFile);

}
