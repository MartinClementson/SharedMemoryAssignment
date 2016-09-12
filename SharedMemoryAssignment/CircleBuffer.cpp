#include "CircleBuffer.h"
#include <iostream>
#include <assert.h>
using namespace SharedMemory;





bool SharedMemory::CircleBuffer::Init(CommandArgs& info,
	LPCWSTR msgBufferName,
	LPCWSTR msgMutexName,
	LPCWSTR infoBufferName,
	LPCWSTR infoMutexName)
{
	SharedMemoryStruct* tempMessage	= new SharedMemoryStruct(); //this was a way of handling the deletion of memory if the init failed
	info.memorySize *= (1 << 20);//convert to bytes!

	InitReturnValues result = tempMessage->Init(&info, msgBufferName);
	if (!result.succeded)
	{
		delete tempMessage;
		return false;
	}
		 
	info.memorySize					= sizeof(SharedData::SharedInformation); //change the memory size for the info buffer,
	SharedMemoryStruct* tempInfo	= new SharedMemoryStruct();

	result = tempInfo->Init(&info, infoBufferName);
	if (!result.succeded)
	{
		delete tempMessage;
		delete tempInfo;
		return false;
	}


	SharedData::SharedInformation* temp = ((SharedData::SharedInformation*) tempInfo->vFileView);

	_MessageMem = std::shared_ptr<SharedMemoryStruct>(tempMessage);			//put them into the shared ptr
	_InfoMem    = std::shared_ptr<SharedMemoryStruct>(tempInfo);			//put them into the shared ptr
																		
	//Create the mutexes
	try {

		msgMutex  = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(msgMutexName));
		infoMutex = std::unique_ptr<SharedMemory::SharedMutex>(new SharedMemory::SharedMutex(infoMutexName));
	}
	catch (...)
	{
		MessageBox(GetConsoleWindow(), TEXT("error creating the mutexes"), TEXT("ERROR"), MB_OK);
		return false;
	}


	if (infoMutex->Lock(INFINITE))
	{
		shared_head  = (size_t*)_InfoMem->vFileView;	  //Connect the pointers
		shared_tail  = shared_head + 1;				  //Connect the pointers
		freeMem		 = shared_tail + 1;				  //Connect the pointers

		if (result.firstTimeInit) //set shared memory to zero
		{
			*shared_head = 0;
			*shared_tail = 0;
			*freeMem	 = _MessageMem->fileSize;
			
			((SharedData::SharedInformation*) _InfoMem->vFileView)->clients = 0;
		}
		((SharedData::SharedInformation*) _InfoMem->vFileView)->clients += 1;
		infoMutex->Unlock();
	}
	else
		MessageBox(GetConsoleWindow(), TEXT("error locking infomutex"), TEXT("ERROR"), MB_OK);

	//SharedData::SharedInformation* temp = ((SharedData::SharedInformation*) _InfoMem->vFileView);

	local_tail		 = *shared_tail;
	messagesSent	 = 0;
	messagesRecieved = 0;
	
	
	return true;
}



size_t SharedMemory::CircleBuffer::CalculatePadding(size_t msgSize, size_t chunkSize)
{

	size_t ret = ((msgSize / chunkSize) + 1)*chunkSize - msgSize;
	if (ret == chunkSize)
		ret = 0;
	return ret;
}

bool SharedMemory::CircleBuffer::Push(void * msg, size_t length)
{

	//calculate padding

	size_t padding = CalculatePadding(sizeof(SharedData::MesssageHeader) + length, chunkSize);
	
	assert((sizeof(SharedData::MesssageHeader)+length + padding) % chunkSize == 0); //just to make sure i've done it right
	
	size_t totalMsgLen = length + sizeof(SharedData::MesssageHeader) + padding;

	//first check if there is any available memory to write to
	if (totalMsgLen <= *freeMem) // if there is room!	
	{

		SharedData::SharedInformation* temp = (SharedData::SharedInformation*) _InfoMem->vFileView;

		SharedData::MesssageHeader header;
		header.msgId = messagesSent++;
		header.length = length;
		header.consumerQueue = temp->clients - 1; //exclude the client sending the message!

		std::cout << header.msgId << " " << (char*)msg << "\n";
		
		if (totalMsgLen + *shared_head > _MessageMem->fileSize) // if not the whole message fits at the end
		{
			if (sizeof(SharedData::MesssageHeader) + *shared_head <= _MessageMem->fileSize) // if the header fits at the end
			{
					size_t fitLength = _MessageMem->fileSize - *shared_head - sizeof(SharedData::MesssageHeader);				// get how many chars fit at the end.
				if (msgMutex->Lock(INFINITE)) //Get mutex and write the header ON�DIG
				{
					memcpy((char*)_MessageMem->vFileView + *shared_head, (char*)&header, sizeof(SharedData::MesssageHeader));	//copy the header
					memcpy((char*)_MessageMem->vFileView + *shared_head + sizeof(SharedData::MesssageHeader), (char*)msg, fitLength);

					memcpy((char*)_MessageMem->vFileView, (char*)msg + fitLength, length - fitLength);							//write at the start of the header
					msgMutex->Unlock();

					////calculate padding
			
					size_t padding = CalculatePadding(length - fitLength,chunkSize);
					
					assert(((length - fitLength) + padding) % chunkSize == 0); //just to make sure i've done it right
					totalMsgLen		= length + sizeof(SharedData::MesssageHeader) + padding;

					if (infoMutex->Lock(INFINITE)) // now update the information
					{
						*shared_head = 0  + (length - fitLength)+ padding;
						*freeMem -= totalMsgLen;
						infoMutex->Unlock();
					}
				}
			}
			else //Delete this after more tests to make sure it never enters. 
			{
				if (infoMutex->Lock(INFINITE))
				{
					*shared_head = 0;	// move the head to start
					infoMutex->Unlock();
					if (msgMutex->Lock(INFINITE)) //Get mutex and write the header
					{
						memcpy((char*)_MessageMem->vFileView + *shared_head, (char*)&header, sizeof(SharedData::MesssageHeader));
						memcpy((char*)_MessageMem->vFileView + *shared_head + sizeof(SharedData::MesssageHeader), (char*)msg, length);
						msgMutex->Unlock();

						if (infoMutex->Lock(INFINITE)) // now update the information
						{
							*shared_head = (*shared_head + totalMsgLen) % _MessageMem->fileSize;
							*freeMem -= totalMsgLen;
							infoMutex->Unlock();
						}
					}
				}
				else
					MessageBox(GetConsoleWindow(), TEXT("Could not lock infomutex in Push()"), TEXT("Abandon hope"), MB_OK);

			}


		}
		else
		{
			if (msgMutex->Lock(INFINITE)) //Get mutex and write the header
			{
				memcpy((char*)_MessageMem->vFileView + *shared_head, (char*)&header, sizeof(SharedData::MesssageHeader));
				memcpy((char*)_MessageMem->vFileView + *shared_head + sizeof(SharedData::MesssageHeader), (char*)msg, length);
				msgMutex->Unlock();

				if (infoMutex->Lock(INFINITE)) // now update the information
				{
					*shared_head = (*shared_head + totalMsgLen) % _MessageMem->fileSize;
					*freeMem -= totalMsgLen;
					infoMutex->Unlock();
				}
			}
		}
	} //endif there is memory to write to
	else
		return false; //could not write to buffer

	return true;
}

bool SharedMemory::CircleBuffer::Push(SharedData::SharedMessage * msg)
{
////calculate padding
//size_t offset  = msg->header.length % chunkSize;
//size_t padding = chunkSize - offset;
//
//assert((msg->header.length + padding) % chunkSize == 0); //just to make sure i've done it right
//
/////*todo  : make a check if i can write.*/
//memcpy(
//	(char*)_MessageMem->vFileView + head, 
//	(char*)&msg->header,
//	sizeof(SharedData::MesssageHeader));
//
//memcpy(
//	(char*)_MessageMem->vFileView + head + sizeof(SharedData::MesssageHeader)
//	, msg->message
//	, msg->header.length + padding);

	return true;
}

bool SharedMemory::CircleBuffer::Pop(char * msg, size_t & length)
{
	SharedData::SharedInformation* info = (SharedData::SharedInformation*) _InfoMem->vFileView;

	if (*freeMem >= _MessageMem->fileSize) // IF there is nothing to read
		return false;




	if (sizeof(SharedData::MesssageHeader) + local_tail <= _MessageMem->fileSize) // if the header fits 
	{
		SharedData::MesssageHeader* header = (SharedData::MesssageHeader*)((char*)_MessageMem->vFileView + local_tail);

		////calculate padding
		size_t offset = (sizeof(SharedData::MesssageHeader) + header->length) % chunkSize;
		size_t padding = chunkSize - offset;
		if (padding == 256)
			padding = 0;
		/////
		size_t msgLength = sizeof(SharedData::MesssageHeader) + header->length;

		if (local_tail + msgLength > _MessageMem->fileSize) // if not the whole message fits
		{
			size_t fitLength = _MessageMem->fileSize - local_tail - sizeof(SharedData::MesssageHeader);				// get how many chars fit at the end.

			memcpy(msg, _MessageMem->vFileView + local_tail + sizeof(SharedData::MesssageHeader), fitLength);

			memcpy(msg + fitLength, _MessageMem->vFileView, header->length - fitLength);


			////calculate new padding
			size_t offset = (header->length - fitLength) % chunkSize;
			size_t padding = chunkSize - offset;
			/////
			msgLength = header->length + sizeof(SharedData::MesssageHeader) + padding;
			//move local tail
			local_tail = 0 + (header->length - fitLength + padding );

			if (header->consumerQueue <= 1) //this means this reader is the last one
			{
				if (infoMutex->Lock(INFINITE))
				{
					*shared_tail = local_tail;
					*freeMem += msgLength;
					infoMutex->Unlock();
				}
				else
					MessageBox(GetConsoleWindow(), TEXT("Could not lock infoMutex in Pop()"), TEXT("Critical error"), MB_OK);
			}
			else
			{
				if (msgMutex->Lock(INFINITE))
				{
					header->consumerQueue -= 1;
					msgMutex->Unlock();
				}


			}
		}
		else //if the whole message fits at the same spot
		{
			//copy the message
			memcpy(msg, (char*)_MessageMem->vFileView + local_tail + sizeof(SharedData::MesssageHeader), header->length);
			//////

			//move local tail
			local_tail = (local_tail + header->length + sizeof(SharedData::MesssageHeader) + padding) % _MessageMem->fileSize;
			//
			if (header->consumerQueue <= 1) //this means this reader is the last one
			{
				if (infoMutex->Lock(INFINITE))
				{
					*shared_tail = (*shared_tail + header->length + sizeof(SharedData::MesssageHeader) + padding) % _MessageMem->fileSize;
					*freeMem += header->length + sizeof(SharedData::MesssageHeader) + padding;
					infoMutex->Unlock();
				}
				else
					MessageBox(GetConsoleWindow(), TEXT("Could not lock infoMutex in Pop()"), TEXT("Critical error"), MB_OK);
			}
			else
			{
				if (msgMutex->Lock(INFINITE))
				{
					header->consumerQueue -= 1;
					msgMutex->Unlock();
				}


			}
		}
	}
	else //if the header doesent fit!
		return false;
	
	std::cout << messagesRecieved++ << " " << msg << "\n";
	return true;
}

bool SharedMemory::CircleBuffer::Pop(SharedData::SharedMessage * msg)
{

	//copy header	CHANGE THIS! DO NOT COPY HEADER!
	//memcpy(&msg->header,(char*)_MessageMem->vFileView + tail,
	//	sizeof(SharedData::MesssageHeader));
	
//size_t readOffset = tail + sizeof(SharedData::MesssageHeader);
////copy message
//memcpy(msg->message,
//	(char*)_MessageMem->vFileView + readOffset, 
//	msg->header.length);
////
	std::cout << msg->message << std::endl;
		

	//calculate padding
	size_t offset = msg->header.length % chunkSize; //this can be dangerous if producer uses another chunk size!
	size_t padding = chunkSize - offset;

	//if (info->numProcesses == 1) // that means this client is the last one
	//{
	//	info->tail     += sizeof(SharedData::MesssageHeader) + msg->header.length + padding; //move the shared tail
	//}
	//else
	//{

	//}

	//this->tail += sizeof(SharedData::MesssageHeader) + msg->header.length + padding; //move the internal tail
	return true;
}

CircleBuffer::CircleBuffer()
{
	
}






CircleBuffer::~CircleBuffer()
{
	if (infoMutex->Lock(INFINITE))
	{
	((SharedData::SharedInformation*) _InfoMem->vFileView)->clients -= 1;
	infoMutex->Unlock();
	}
}
