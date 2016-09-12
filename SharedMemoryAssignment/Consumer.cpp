#include "Consumer.h"



DWORD Consumer::ReadFromMemory()
{


	
	if (this->messageBuffer->Pop(this->localMsg->message, localMsg->header.length))
	{
	//	std::cout << localMsg->message << "\n\n\n"<<std::endl;
		sessionInfo.messagesRecieved += 1;
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

	this->sessionInfo = SessionInfo(arguments); //store the arguments into a struct

	localMsg				 = std::shared_ptr<SharedData::SharedMessage>(new SharedData::SharedMessage());
	this->maxMessageLen		 = (arguments.memorySize *(1 << 20)); 
	localMsg->message		 = new char[maxMessageLen];			//make it maximum size


 #pragma region Open mapping and create file view 

	messageBuffer = std::unique_ptr<SharedMemory::CircleBuffer>(new SharedMemory::CircleBuffer());
	if (!messageBuffer->Init(arguments, 
		GetFileName(Files::MessageFile),
		GetMutexName(Files::MessageFile),
		GetFileName(Files::InformationFile),
		GetMutexName(Files::InformationFile)
		))
		errorflag = true;

#pragma endregion

#pragma region Open mutex

	//if (!errorflag)
	//{
	//
	//try {	//Create the  mutexes
	//		msgMutex  = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(this->GetMutexName(Files::MessageFile)));
	//		infoMutex = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(this->GetMutexName(Files::InformationFile)));
	//	}
	//	catch (...)
	//	{
	//		MessageBox(GetConsoleWindow(), TEXT("error creating the mutexes"), TEXT("ERROR"), MB_OK);
	//		errorflag = true;
	//	}
	//}

#pragma endregion

	if(!errorflag)
		errorflag = SetUpEventHandling(errorflag);

		//if (errorflag == false)
		//{
		//	//Get the info file, wait for the mutex. then add a counter to the "numProcesses" variable
		//	if (infoMutex->Lock(INFINITE))
		//	{
		//		SharedData::SharedInformation *temp;
		//		temp = (SharedData::SharedInformation*)this->messageBuffer->GetInfoBuffer()->vFileView;
		//		temp->clients += 1; // increment numProcesses in the shared memory

		//		infoMutex->Unlock();
		//	}

		//}
		//else
		//	running = false; //if there was any error in the process

#pragma endregion
}

Consumer::Consumer()
{
}


Consumer::~Consumer()
{
	
	
	
}

bool Consumer::Exec()
{
	
	if (running)
	{
		if (this->sessionInfo.messagesRecieved == sessionInfo.numMessages)
			running = false;
		HandleEvents();
		Sleep(this->sessionInfo.msDelay);
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
