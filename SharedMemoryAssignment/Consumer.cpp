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
			hMsgMutex,					// handle to mutex
			INFINITE);				// no time out interval!

		switch (waitResult)
		{
		case WAIT_OBJECT_0: //Got ownership of the mutex
			__try {
				//write to database
				//std::string data;
				//memcpy((PVOID)&data, pbuf, sizeof(char));
				std::cout << (int)*pMsgbuf << std::endl;
			
			}
			__finally
			{
				//release ownership of the mutex

				if (!ReleaseMutex(hMsgMutex))
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

	


	return errorflag;
}

void Consumer::HandleEvents()
{




}


Consumer::Consumer(CommandArgs commands)
{
	bool error = false;

 #pragma region Create file view and open mapping

	hMsgMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		(LPWSTR)GetFileName(Files::MessageFile));
	if (hMsgMapFile == NULL) //if there is no file created
	{
		int answer = 0;
		do {
			answer = MessageBox(GetConsoleWindow(), TEXT("Could not find the file. Make sure a producer is running"), TEXT("No File found"), MB_RETRYCANCEL);
			if (answer == IDRETRY)
			{
				hMsgMapFile = OpenFileMapping(
					FILE_MAP_ALL_ACCESS,
					FALSE,
					GetFileName(Files::MessageFile));
				if (hMsgMapFile != NULL)
					break;
			}

		} while (answer != IDCANCEL);
	}
	if (hMsgMapFile != NULL) // if the file was found when clicking retry or on the first try
	{


		pMsgbuf = (LPTSTR)MapViewOfFile(hMsgMapFile,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			commands.memorySize * 1000000);
		if (pMsgbuf == NULL)
		{
			MessageBox(GetConsoleWindow(), TEXT("Error when mapping file view"), TEXT("error"), MB_OK);
			error = true;
		}



		hInfoMapFile = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			(LPWSTR)GetFileName(Files::InformationFile));

		if (hInfoMapFile == NULL)
			error = true;

		pInfobuf = (LPTSTR)MapViewOfFile(hInfoMapFile,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			sizeof(SharedInformation));
		if (pInfobuf == NULL)
		{
			MessageBox(GetConsoleWindow(), TEXT("Error when mapping file view"), TEXT("error"), MB_OK);
			error = true;
		}







#pragma endregion

#pragma region Open mutex

		hMsgMutex = OpenMutex(
			MUTEX_ALL_ACCESS,
			FALSE,
			this->GetMutexName(Files::MessageFile));

		if (hMsgMutex == NULL)
		{
			MessageBox(GetConsoleWindow(), TEXT("Error openingMutex"), TEXT("HELP"), MB_OK);
			error = true;
		}


		hInfoMutex = OpenMutex(
			MUTEX_ALL_ACCESS,
			FALSE,
			this->GetMutexName(Files::InformationFile));

		if (hInfoMutex == NULL)
		{
			MessageBox(GetConsoleWindow(), TEXT("Error openingMutex"), TEXT("HELP"), MB_OK);
			error = true;
		}


#pragma endregion


		error = SetUpEventHandling(error);

		if (error == false)
		{
		

			//Get the info file, wait for the mutex. then add a counter to the "numProcesses" variable

			DWORD waitResult;

			waitResult = WaitForSingleObject(		//Get the control of the mutex
				hInfoMutex,							// handle to mutex
				INFINITE);					     	// no time out interval!
			SharedInformation *temp;
			switch (waitResult)
			{
			case WAIT_OBJECT_0: //Got ownership of the mutex

				temp = (SharedInformation*)pInfobuf;
				temp->numProcesses += 1; // increment numProcesses in the shared memory
			
				if (!ReleaseMutex(hInfoMutex))			// Release the ownership of the mutex so that other processes can access it
						MessageBox(GetConsoleWindow(), TEXT("COULD NOT RELEASE MUTEX!"), TEXT("Obsessed handle"), MB_OK);
				break;

			case WAIT_ABANDONED: //got ownership of an abandoned mutex object
					MessageBox(GetConsoleWindow(), TEXT("COULD NOT get Info MUTEX!"), TEXT("in consumer Constructor"), MB_OK);
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
	
	
		 //Get the info file, wait for the mutex. then decrease a counter to the "numProcesses" variable

		DWORD waitResult;

		waitResult = WaitForSingleObject(		//Get the control of the mutex
			hInfoMutex,							// handle to mutex
			INFINITE);					     	// no time out interval!
		SharedInformation* temp;
		switch (waitResult)
		{
		case WAIT_OBJECT_0: //Got ownership of the mutex

			temp = (SharedInformation*)pInfobuf;
			temp->numProcesses -= 1;
			if (!ReleaseMutex(hInfoMutex))			// Release the ownership of the mutex so that other processes can access it
				MessageBox(GetConsoleWindow(), TEXT("COULD NOT RELEASE MUTEX!"), TEXT("Obsessed handle"), MB_OK);
			break;

		case WAIT_ABANDONED: //got ownership of an abandoned mutex object
			MessageBox(GetConsoleWindow(), TEXT("COULD NOT get Info MUTEX!"), TEXT("in consumer Constructor"), MB_OK);
		}

	


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
