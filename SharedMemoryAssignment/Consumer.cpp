#include "Consumer.h"



DWORD Consumer::ReadFromMemory()
{
	DWORD waitResult;
	DWORD waitResultMutex = NULL;
	waitResult = WaitForSingleObject( //wait for event to trigger that the producer has finished writing
		hWriteEvent, // event handle
		INFINITE);    // indefinite wait


	switch (waitResult)
	{
		// Event object was signaled
	case WAIT_OBJECT_0:
		//
		// TODO: Read from the shared buffer
	#pragma region Wait for mutex control then attempt to read
	

		waitResultMutex = WaitForSingleObject(		//Get the control of the mutex
			hMutex,					// handle to mutex
			INFINITE);				// no time out interval!

		switch (waitResult)
		{
		case WAIT_OBJECT_0: //Got ownership of the mutex
			__try {
				//write to database
				//std::string data;
				//memcpy((PVOID)&data, pbuf, sizeof(char));
				std::cout << (int)*pbuf << std::endl;
			
			}
			__finally
			{
				//release ownership of the mutex

				if (!ReleaseMutex(hMutex))
					MessageBox(NULL, TEXT("COULD NOT RELEASE MUTEX!"), TEXT("DANGER"), MB_OK);
			}
			break;

		case WAIT_ABANDONED: //got ownership of an abandoned mutex object
			return FALSE;

		}
	#pragma endregion
		break;

		// An error occurred
	default:
		printf("Wait error (%d)\n", GetLastError());
		return 0;
	}



	return TRUE;

}

bool Consumer::SetUpEventHandling(bool errorflag)
{

#pragma region Create Events

	hWriteEvent = OpenEvent(
		SYNCHRONIZE,
		FALSE,
		GetWriteEventName()
		);
	if (hWriteEvent == NULL)
	{
		MessageBox(NULL, TEXT("Could not init writeEvent"), TEXT("Abandon hope"), MB_OK);
		errorflag = true;
	}

	hConnectEvent = OpenEvent(
		EVENT_MODIFY_STATE,
		FALSE,
		GetConnectEventName()
		);
	if (hConnectEvent == NULL)
	{
		MessageBox(NULL, TEXT("Could not init connectEvent"), TEXT("Abandon hope"), MB_OK);
		errorflag = true;
	}

	hDisconnectEvent = OpenEvent(
		EVENT_MODIFY_STATE,
		FALSE,
		GetDisconnectEventName()
		);
	if (hDisconnectEvent == NULL)
	{
		MessageBox(NULL, TEXT("Could not init disconnect event"), TEXT("Abandon hope"), MB_OK);
		errorflag = true;
	}




	return errorflag;
}

void Consumer::HandleEvents()
{
	


}


Consumer::Consumer(CommandArgs commands)
{
	bool error = false;
 #pragma region Create file view and open mapping

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		(LPWSTR)GetFileName());
	if (hMapFile == NULL) //if there is no file created
	{
		int answer = 0;
		do {
			answer = MessageBox(NULL, TEXT("Could not find the file. Make sure a producer is running"), TEXT("No File found"), MB_RETRYCANCEL);
			if (answer == IDRETRY)
			{
				hMapFile = OpenFileMapping(
					FILE_MAP_ALL_ACCESS,
					FALSE,
					GetFileName());
				if (hMapFile != NULL)
					break;
			}

		} while (answer != IDCANCEL);
	}
	if (hMapFile != NULL) // if the file was found when clicking retry or on the first try
	{


		pbuf = (LPTSTR)MapViewOfFile(hMapFile,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			commands.memorySize * 1000000);
		if (pbuf == NULL)
		{
			MessageBox(NULL, TEXT("Error when mapping file view"), TEXT("error"), MB_OK);
			error = true;
		}
		

		
#pragma endregion

#pragma region Open mutex

	hMutex = OpenMutex(
		MUTEX_ALL_ACCESS,
		FALSE,
		this->GetMutexName());

	if (hMutex == NULL)
	{
		MessageBox(NULL, TEXT("Error openingMutex"), TEXT("HELP"), MB_OK);
		error = true;
	}
#pragma endregion


	error = SetUpEventHandling(error);

	if (error == false)
		if (!SetEvent(this->hConnectEvent))
		{
			DWORD hej = GetLastError();
			std::cout << GetLastError() << std::endl;
			MessageBox(NULL, TEXT("Could not trigger Connect event"), TEXT("Connection problem"), MB_OK);

		}

#pragma endregion
	}
}

Consumer::Consumer()
{
}


Consumer::~Consumer()
{
	SetEvent(this->hDisconnectEvent);
	SharedMemoryHandler::~SharedMemoryHandler();

	//UnmapViewOfFile(pbuf);
	//CloseHandle(hMapFile);
}

void Consumer::Exec()
{
	while (this->ReadFromMemory() == TRUE);

}
