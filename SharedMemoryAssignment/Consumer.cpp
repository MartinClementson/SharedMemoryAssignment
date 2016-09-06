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
		int* pMsgbuf;
	case WAIT_OBJECT_0:
		//
		// TODO: Read from the shared buffer
			pMsgbuf = (int*)this->messageBuffer->GetMessageBuffer()->vFileView;
			std::cout << *pMsgbuf << std::endl;
	#pragma region Wait for mutex control then attempt to read
	

	//waitResultMutex = WaitForSingleObject(		//Get the control of the mutex
	//	hMsgMutex,					// handle to mutex
	//	INFINITE);				// no time out interval!
	//
	//switch (waitResult)
	//{
	//case WAIT_OBJECT_0: //Got ownership of the mutex
	//	__try {
	//		//write to database
	//		//std::string data;
	//		//memcpy((PVOID)&data, pbuf, sizeof(char));
	//	
	//	}
	//	__finally
	//	{
	//		//release ownership of the mutex
	//
	//		if (!ReleaseMutex(hMsgMutex))
	//			MessageBox(NULL, TEXT("COULD NOT RELEASE MUTEX!"), TEXT("DANGER"), MB_OK);
	//	}
	//	break;
	//
	//case WAIT_ABANDONED: //got ownership of an abandoned mutex object
	//	return FALSE;
	//
	//}
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
		hWriteEvent = CreateEvent(
			NULL,				 //default security attr
			TRUE,				 //manual reset event
			FALSE,			     // non signaled when initializing
			GetWriteEventName()  // name
			);
		if (hWriteEvent == NULL)
		{
			MessageBox(NULL, TEXT("Could not init writeEvent"), TEXT("Abandon hope"), MB_OK);
			errorflag = true;
		}
	}

	


	return errorflag;
}

void Consumer::HandleEvents()
{




}


Consumer::Consumer(CommandArgs arguments)
{
	bool errorflag = false;


 #pragma region Open mapping and create file view 

	try {
		messageBuffer = std::unique_ptr<SharedMemory::CircleBuffer>(new SharedMemory::CircleBuffer(arguments, GetFileName(Files::MessageFile), GetFileName(Files::InformationFile)));
	}
	catch (...)
	{
		errorflag = true;
	}

#pragma endregion

#pragma region Open mutex


	try {	//Create the  mutexes
			msgMutex  = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(this->GetMutexName(Files::MessageFile)));
			infoMutex = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(this->GetMutexName(Files::InformationFile)));
		}
		catch (...)
		{
			MessageBox(GetConsoleWindow(), TEXT("error creating the mutexes"), TEXT("ERROR"), MB_OK);
			errorflag = true;
		}

#pragma endregion


		errorflag = SetUpEventHandling(errorflag);

		if (errorflag == false)
		{
			//Get the info file, wait for the mutex. then add a counter to the "numProcesses" variable
			if (infoMutex->Lock(INFINITE))
			{
				SharedData::SharedInformation *temp;
				temp = (SharedData::SharedInformation*)this->messageBuffer->GetInfoBuffer()->vFileView;
				temp->numProcesses += 1; // increment numProcesses in the shared memory

				infoMutex->Unlock();
			}

		}
		else
			running = false; //if there was any error in the process

#pragma endregion
	//}
	//else
	//{
	//	//if the filemap is NULL and user didnt want to retry
	//	running = false;
	//}
}

Consumer::Consumer()
{
}


Consumer::~Consumer()
{
	
	
		 //Get the info file, wait for the mutex. then decrease a counter to the "numProcesses" variable
	if (infoMutex != NULL)
	{
		if (infoMutex->Lock(INFINITE))
		{
			SharedData::SharedInformation* temp = (SharedData::SharedInformation*)this->messageBuffer->GetInfoBuffer()->vFileView;
			temp->numProcesses -= 1;

			infoMutex->Unlock();
		}
	}
}

bool Consumer::Exec()
{
	
	if (running)
	{
		HandleEvents();
		if (!this->ReadFromMemory() == TRUE)
		{
			MessageBox(GetConsoleWindow(), TEXT("Could not read from memory!"), TEXT("Error"), MB_OK);
			return false;
		}
		return true;
	}
	else
		return false;

}
