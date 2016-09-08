#include "Producer.h"
#include <atlstr.h>
#include <assert.h>


DWORD Producer::WriteToMemory(SharedData::SharedMessage* msg)
{
	
	//calculate padding
	size_t offset   = msg->header.length % 256;
	size_t padding  = 256 - offset;

	assert((msg->header.length + padding) % 256 == 0);

	
	if(msgMutex->Lock(INFINITE))
	{
		//Mutex is ours
		PVOID pMsgbuf = (PVOID) this->messageBuffer->GetMessageBuffer()->vFileView;
		ZeroMemory(pMsgbuf, 10);

		SharedData::SharedMessage* ptr = localMsg.get();
		//messageBuffer->Push(&localMsg->header, sizeof(SharedData::MesssageHeader)); //write header
		//messageBuffer->Push(msg->message, localMsg->header.length + padding);
		messageBuffer->Push(localMsg.get());

		//messageBuffer->Push((char*)msg,sizeof(SharedData::MesssageHeader) + localMsg->header.length);
		//memcpy(pMsgbuf, &number, sizeof(int));

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
			HandleEvents();						//check for events
			Sleep((DWORD)sessionInfo.msDelay);  //delay specified by the user
			if (ReadSharedInformation())		//if the information was read successfully
			{
				if (this->numProcesses > 0) //If there are any consumers
				{
					if (sessionInfo.MessagesToSend()) //if there are messages to send
					{

						cout << "\n\n\n \t\tMESSAGE NUMBER :" << sessionInfo.messagesSent + 1 << "\n\n";
						this->GenerateRndMessage();
						cout << localMsg->message << endl;
						cout << localMsg->message << endl;
						cout << localMsg->message << endl;
						cout << localMsg->message << endl;
						if (WriteToMemory(localMsg.get()) == FALSE)
						{
							MessageBox(GetConsoleWindow(), TEXT("Could not write to memory"), TEXT("HELP"), MB_OK);
							running = false;
						}
						else {
							localMsg->Flush();
							sessionInfo.messagesSent += 1;
							x++;
						}
					}
					else {
						//std::cout << "All messages have been sent.. Exiting application" << std::endl;
						#ifdef DEBUG
						Sleep(2000);
						#endif // DEBUG
						//running = false;
					}
				}
			}
			else
				running = false;
		return true;

	}
	else
		return false;

}

void Producer::GenerateRndMessage()
{
	static const char charArray[] =
		"abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ123456789";

	size_t msgSize = 1;
	if (sessionInfo.random) //create a random sized message (NOT bigger than a quarter of fileSize)
	{
		size_t fileSize = messageBuffer->GetMessageBuffer()->fileSize;
		//msgSize = rand() % ((fileSize / 4) - sizeof(SharedData::MesssageHeader)) + 1; // random between (1 - fileSize/4 - header)
	
		msgSize = rand() % 8 + 1; //temp
	}
	else
		msgSize = sessionInfo.msgSize;
	size_t hej = sizeof(charArray);
	
	if (msgSize > maxMessageLen) // i later preallocated localMsg->message to the maximum possible, this if statement should not be executed, but you never know for sure
	{
		MessageBox(GetConsoleWindow(), TEXT("msgSize is larger than maxMessageLength"), TEXT("Func: GenerateRndMessage"),MB_OK);
		msgSize = maxMessageLen;
	}
	
	for (size_t i = 0; i < msgSize; i++) // fill the message with random characters
	{
		localMsg->message[i] =  charArray[rand() % (sizeof(charArray) - 1)]; //-1 to remove the \0

	}
	
	localMsg->message[msgSize]	= '\0';
	localMsg->header.msgId			= sessionInfo.messagesGenerated;
	localMsg->header.length			= msgSize+1; //+1 for escapechar
	localMsg->header.consumerQueue  = this->numProcesses;
	sessionInfo.messagesGenerated  += 1;

	cout << localMsg->message << endl;

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

Producer::Producer(CommandArgs& arguments)
{

	
	bool errorflag		 = false;
	srand(time(NULL));
	this->sessionInfo	 = SessionInfo(arguments); //store the arguments into a struct

	localMsg			 = std::shared_ptr<SharedData::SharedMessage>(new SharedData::SharedMessage());
	this->maxMessageLen  = (arguments.memorySize *(1 << 20)) / 4;
	localMsg->message	 = new char[maxMessageLen]; //make it maximum size
	memset(localMsg->message, '0', maxMessageLen);




	messageBuffer	= std::unique_ptr<SharedMemory::CircleBuffer>(new SharedMemory::CircleBuffer());
	if (!messageBuffer->Init(arguments, GetFileName(Files::MessageFile), GetFileName(Files::InformationFile)))
		errorflag   = true;



	/*
		We have two shared memory files.
		One is for the messages.

		one is for information, such as, how many consumers are there, and how many have read the latest message
			
	*/
	
	

#pragma region Create Mutexes
if (!errorflag)
{

	try {

		msgMutex = std::unique_ptr<SharedMemory::SharedMutex> (new SharedMemory::SharedMutex(this->GetMutexName(Files::MessageFile)));
		infoMutex = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(this->GetMutexName(Files::InformationFile)));
	}
	catch (...)
	{
		MessageBox(GetConsoleWindow(), TEXT("error creating the mutexes"), TEXT("ERROR"), MB_OK);
		errorflag = true;
	}
}
#pragma endregion
	if (!errorflag)
		errorflag = this->SetUpEventHandling(errorflag);

	if (errorflag == true)
		running = false;
	
}


Producer::~Producer()
{
//	UnmapViewOfFile(pbuf);
	//CloseHandle(hMapFile);

}
