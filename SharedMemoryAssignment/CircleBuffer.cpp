#include "CircleBuffer.h"
#include <iostream>
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
	
	///*todo  : make a check if i can write.*/
	memcpy((LPVOID)_MessageMem->vFileView /*+ head*/, (char*)msg, length);

	SharedData::SharedMessage tempMsg;

	memcpy(&tempMsg.header,(char*)_MessageMem->vFileView, sizeof(SharedData::MesssageHeader));
	tempMsg.message = new char[length];
	char* print = (char*)msg;
	memcpy(tempMsg.message, (char*)_MessageMem->vFileView + sizeof(SharedData::MesssageHeader), length -sizeof(SharedData::MesssageHeader));
	
	std::cout << "THIS IS THE READ MESSAGE :";
	std::cout << (char*)_MessageMem->vFileView << std::endl;
	
	//std::cout << tempMsg.message << std::endl; 
	return true;
}

bool SharedMemory::CircleBuffer::Pop(char * msg, size_t & length)
{
	return false;
}

bool SharedMemory::CircleBuffer::Pop(SharedData::SharedMessage * msg)
{

	
//memcpy(&msg->header,(char*)_MessageMem->vFileView + tail, sizeof(SharedData::MesssageHeader));
//size_t readOffset = tail + sizeof(SharedData::MesssageHeader);
//memcpy(msg->message, (char*)_MessageMem->vFileView + readOffset, msg->header.length);
//
//std::cout << msg->message << std::endl;
	//SharedData::SharedInformation* info = (SharedData::SharedInformation*)_InfoMem->vFileView;

	////calculate padding
	//size_t offset = msg->header.length % 256;
	//size_t padding = 256 - offset;

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
