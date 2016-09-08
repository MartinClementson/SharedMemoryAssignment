#include "CircleBuffer.h"
#include <iostream>
#include <assert.h>
using namespace SharedMemory;





bool SharedMemory::CircleBuffer::Init(CommandArgs & info, LPCWSTR msgBufferName, LPCWSTR infoBufferName)
{
	SharedMemoryStruct* tempMessage	= new SharedMemoryStruct(); //this was a way of handling the deletion of memory if the init failed

	info.memorySize *= (1 << 20);//convert to bytes!
	if (!tempMessage->Init(&info, msgBufferName))
	{
		delete tempMessage;
		return false;
	}
		 
	info.memorySize					= sizeof(SharedData::SharedInformation); //change the memory size for the info buffer,
	SharedMemoryStruct* tempInfo	= new SharedMemoryStruct();
	if (!tempInfo->Init(&info, infoBufferName))
	{
		delete tempMessage;
		delete tempInfo;
		return false;
	}

	
	_MessageMem = std::shared_ptr<SharedMemoryStruct>(tempMessage);			//put them into the shared ptr
	_InfoMem    = std::shared_ptr<SharedMemoryStruct>(tempInfo);			//put them into the shared ptr
	
	return true;
}

bool SharedMemory::CircleBuffer::Push(void * msg, size_t length)
{
	/*maybe do padding here. but we don't need it if we're writing a header,*/


	///*todo  : make a check if i can write.*/
	memcpy((LPVOID)_MessageMem->vFileView /*+ head*/, (char*)msg, length);

	return true;
}

bool SharedMemory::CircleBuffer::Push(SharedData::SharedMessage * msg)
{
	//calculate padding
	size_t offset  = msg->header.length % chunkSize;
	size_t padding = chunkSize - offset;

	assert((msg->header.length + padding) % chunkSize == 0); //just to make sure i've done it right

	///*todo  : make a check if i can write.*/
	memcpy(
		(char*)_MessageMem->vFileView + head, 
		(char*)&msg->header,
		sizeof(SharedData::MesssageHeader));

	memcpy(
		(char*)_MessageMem->vFileView + head + sizeof(SharedData::MesssageHeader)
		, msg->message
		, msg->header.length + padding);

	return true;
}

bool SharedMemory::CircleBuffer::Pop(char * msg, size_t & length)
{
	return false;
}

bool SharedMemory::CircleBuffer::Pop(SharedData::SharedMessage * msg)
{

	//copy header
	memcpy(&msg->header,(char*)_MessageMem->vFileView + tail,
		sizeof(SharedData::MesssageHeader));
	
	size_t readOffset = tail + sizeof(SharedData::MesssageHeader);
	//copy message
	memcpy(msg->message,
		(char*)_MessageMem->vFileView + readOffset, 
		msg->header.length);
	//
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

	
}
