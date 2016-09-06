#include "Consumer.h"



DWORD Consumer::ReadFromMemory()
{
	DWORD waitResult;
	DWORD waitResultMutex = NULL;
	waitResult = WaitForSingleObject( //wait for event to trigger that the producer has finished writing
		hWriteEvent, // event handle
		INFINITE);    // infinite wait

	switch (waitResult)
	{
		// Event object was signaled
		int* pMsgbuf;
	case WAIT_OBJECT_0:
		//
		// TODO: Read from the shared buffer
			pMsgbuf = (int*)this->messageBuffer->GetMessageBuffer()->vFileView;
			std::cout << *pMsgbuf << std::endl;
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


Consumer::Consumer(CommandArgs &arguments)
{
	bool errorflag = false;


 #pragma region Open mapping and create file view 

	messageBuffer = std::unique_ptr<SharedMemory::CircleBuffer>(new SharedMemory::CircleBuffer());
	if (!messageBuffer->Init(arguments, GetFileName(Files::MessageFile), GetFileName(Files::InformationFile)))
		errorflag = true;

#pragma endregion

#pragma region Open mutex

	if (!errorflag)
	{

	try {	//Create the  mutexes
			msgMutex  = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(this->GetMutexName(Files::MessageFile)));
			infoMutex = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(this->GetMutexName(Files::InformationFile)));
		}
		catch (...)
		{
			MessageBox(GetConsoleWindow(), TEXT("error creating the mutexes"), TEXT("ERROR"), MB_OK);
			errorflag = true;
		}
	}

#pragma endregion

	if(!errorflag)
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
