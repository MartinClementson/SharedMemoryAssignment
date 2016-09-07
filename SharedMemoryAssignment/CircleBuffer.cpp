#include "CircleBuffer.h"

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

bool SharedMemory::CircleBuffer::Push(const void * msg, size_t length)
{
	return false;
}

bool SharedMemory::CircleBuffer::Pop(char * msg, size_t & length)
{
	return false;
}

CircleBuffer::CircleBuffer()
{
	
}




CircleBuffer::~CircleBuffer()
{

	
}
