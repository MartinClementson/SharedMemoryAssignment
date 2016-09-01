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

	DWORD waitResult = WaitForSingleObject( //peek if the console has closed
		hCloseEvent,
		1);
	if (waitResult == WAIT_OBJECT_0)
	{
		this->running = false;
	}


}


Consumer::Consumer(CommandArgs commands)
{
	bool error = false;

	//First up, set up the local close event! we need to make sure that the handling of this event will be taken care of 
	//no matter if the rest of the program fails.

	hCloseEvent = OpenEvent(
		SYNCHRONIZE,
		FALSE,
		CLOSE_EVENT_NAME
		);
	if (hCloseEvent == NULL)
	{
		MessageBox(GetConsoleWindow(), TEXT("Could not open closeEvent event"), TEXT("Abandon hope"), MB_OK);
		error = true;
	}


 #pragma region Create file view and open mapping

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		(LPWSTR)GetFileName());
	if (hMapFile == NULL) //if there is no file created
	{
		int answer = 0;
		do {
			answer = MessageBox(GetConsoleWindow(), TEXT("Could not find the file. Make sure a producer is running"), TEXT("No File found"), MB_RETRYCANCEL);
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
			MessageBox(GetConsoleWindow(), TEXT("Error when mapping file view"), TEXT("error"), MB_OK);
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
			MessageBox(GetConsoleWindow(), TEXT("Error openingMutex"), TEXT("HELP"), MB_OK);
			error = true;
		}
#pragma endregion


		error = SetUpEventHandling(error);

		if (error == false)
		{
			if (!SetEvent(this->hConnectEvent))
			{
				DWORD hej = GetLastError();
				std::cout << GetLastError() << std::endl;
				MessageBox(GetConsoleWindow(), TEXT("Could not trigger Connect event"), TEXT("Connection problem"), MB_OK);

			}
		}
		else
			running = false; //if there was any error in the process

#pragma endregion
	}
	else
	{
		running = false;
			//if the filemap is NULL
	}
}

Consumer::Consumer()
{
}


Consumer::~Consumer()
{
	SetEvent(this->hDisconnectEvent);
	

	//UnmapViewOfFile(pbuf);
	//CloseHandle(hMapFile);
}

bool Consumer::Exec()
{
	
	if (running)
	{
		HandleEvents();
		if (!this->ReadFromMemory() == TRUE)
			MessageBox(GetConsoleWindow(), TEXT("Could not read from memory!"), TEXT("Error"), MB_OK);
		else
			return true;
	}
	else
		return false;

}
