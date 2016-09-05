#include "CircleBuffer.h"



CircleBuffer::CircleBuffer()
{
}

CircleBuffer::CircleBuffer(CommandArgs info, LPCWSTR msgBufferName, LPCWSTR infoBufferName)
{
	try {
		_MessageMem = std::unique_ptr<SharedMemoryStruct>(new SharedMemoryStruct(info,msgBufferName));
		_InfoMem	= std::unique_ptr<SharedMemoryStruct>(new SharedMemoryStruct(info,infoBufferName));
	}
	catch (...)
	{
		throw(2);
	}

}




CircleBuffer::~CircleBuffer()
{

	
}
