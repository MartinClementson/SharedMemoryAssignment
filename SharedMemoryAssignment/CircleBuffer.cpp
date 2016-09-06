#include "CircleBuffer.h"

using namespace SharedMemory;

size_t CircleBuffer::CanRead()
{
	return size_t();
}

size_t CircleBuffer::CanWrite()
{
	return size_t();
}

CircleBuffer::CircleBuffer()
{
}

CircleBuffer::CircleBuffer(CommandArgs info, LPCWSTR msgBufferName, LPCWSTR infoBufferName)
{
	try {
		_MessageMem = std::shared_ptr<SharedMemoryStruct>(new SharedMemoryStruct(info,msgBufferName));
		_InfoMem	= std::shared_ptr<SharedMemoryStruct>(new SharedMemoryStruct(info,infoBufferName));
	}
	catch (...)
	{
		throw(2);
	}

}




CircleBuffer::~CircleBuffer()
{

	
}
