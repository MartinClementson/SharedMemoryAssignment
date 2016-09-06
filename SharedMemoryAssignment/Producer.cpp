#include "Producer.h"
#include <atlstr.h>


DWORD Producer::WriteToMemory(int number)
{
	
	if(msgMutex->Lock(INFINITE))
	{
		//Mutex is ours
		PVOID pMsgbuf = (PVOID) this->messageBuffer->GetMessageBuffer()->vFileView;
		ZeroMemory(pMsgbuf, 10);
		memcpy(pMsgbuf, &number, sizeof(int));

		msgMutex->Unlock(); //release the mutex

		if (!SetEvent(this->hWriteEvent))	//Signal that the writing is done!
			MessageBox(NULL, TEXT("COULD NOT Trigger Write event!"), TEXT("DANGER"), MB_OK);
		else
			ResetEvent(this->hWriteEvent);		//Reset the signal directly after it has been sent out!
	}

				std::cout<< "Amount of consumers " << this->numProcesses << std::endl;
			
	return TRUE;
 }

bool Producer::Exec()
{
	static int x = 0; //just some data to transfer
	if (running)
	{

			HandleEvents(); //check for events
			Sleep(500);
			if (ReadSharedInformation()) //if the information was read successfully
			{

				if (this->numProcesses > 0) //If there are any consumers
				{

					if (WriteToMemory(x) == FALSE)
					{
						MessageBox(GetConsoleWindow(), TEXT("Could not write to memory"), TEXT("HELP"), MB_OK);
						running = false;
					}
					x++;
				}
			}
			else
				running = false;
		return true;

	}
	else
		return false;

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
		MessageBox(GetConsoleWindow(), TEXT("Could not init writeEvent"), TEXT("Abandon hope"), MB_OK);
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


}

bool Producer::ReadSharedInformation()
{


#pragma region Access Info mutex 

	
	SharedData::SharedInformation* temp;
	temp = (SharedData::SharedInformation*)this->messageBuffer->GetInfoBuffer()->vFileView;

	if (temp->numProcesses != this->numProcesses) //if the information has changed
	{
		this->numProcesses = temp->numProcesses;  //update the producers information
		if (numProcesses == 0)					  //If the consumers disconnected
			std::cout << "No Consumers connected" << std::endl;
	}
	return true;
}

Producer::Producer()
{
}

Producer::Producer(CommandArgs arguments)
{


	/*
		We have two shared memory files.
		One is for the messages.

		one is for information, such as, how many consumers are there, and how many have read the latest message
			
	*/


	bool errorflag = false;





try{
	messageBuffer = std::unique_ptr<SharedMemory::CircleBuffer>(new SharedMemory::CircleBuffer(arguments,GetFileName(Files::MessageFile), GetFileName(Files::InformationFile)));
}
catch(...)
{
	errorflag = true;
}








#pragma region Create file and mapping
//this->hMsgMapFile = CreateFileMapping(
//	INVALID_HANDLE_VALUE,	//Instead of a file in the system, we use a system paging file
//	NULL,					//No extra attributes (default)
//	PAGE_READWRITE,			//specifies the protection, all the views to the file need to p� compatible with this!
//	0,
//	(arguments.memorySize * 1 << 20), //convert to megabyte
//	GetFileName(Files::MessageFile)
//	);
//
//if (hMsgMapFile == NULL)
//{
//	_tprintf(TEXT("FAILED!"));
//	errorflag = true;
//	DebugBreak();
//}
//
//pMsgbuf = (LPTSTR)MapViewOfFile(hMsgMapFile,
//	FILE_MAP_ALL_ACCESS,
//	0,
//	0,
//	arguments.memorySize* 1 << 20);
//
//if (pMsgbuf == NULL)
//{
//	_tprintf(TEXT("FAILED!"));
//	errorflag = true;
//	DebugBreak();
//}
//
//
//
//
//this->hInfoMapFile = CreateFileMapping(
//	INVALID_HANDLE_VALUE,	//Instead of a file in the system, we use a system paging file
//	NULL,					//No extra attributes (default)
//	PAGE_READWRITE,			//specifies the protection, all the views to the file need to p� compatible with this!
//	0,
//	sizeof(SharedData::SharedInformation),
//	GetFileName(Files::InformationFile)
//	);
//
//if (hInfoMapFile == NULL)
//{
//	MessageBox(GetConsoleWindow(), TEXT("Error when mapping file view"), TEXT("error"), MB_OK);
//	_tprintf(TEXT("FAILED!"));
//	errorflag = true;
//	DebugBreak();
//}
//
//
//pInfobuf = (LPTSTR)MapViewOfFile(hInfoMapFile,
//	FILE_MAP_ALL_ACCESS,
//	0,
//	0,
//	sizeof(SharedData::SharedInformation));
//
//if (pInfobuf == NULL)
//{
//	_tprintf(TEXT("FAILED!"));
//	errorflag = true;
//	DebugBreak();
//}





	//TCHAR szMsg[] = TEXT("This text is sent to the file");
	//
	//CopyMemory((PVOID)pbuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

#pragma endregion


#pragma region Create Mutexes
	
	try {

		msgMutex = std::unique_ptr<SharedMemory::SharedMutex> (new SharedMemory::SharedMutex(this->GetMutexName(Files::MessageFile)));
		infoMutex = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(this->GetMutexName(Files::InformationFile)));
	}
	catch (...)
	{
		MessageBox(GetConsoleWindow(), TEXT("error creating the mutexes"), TEXT("ERROR"), MB_OK);
		errorflag = true;
	}
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
