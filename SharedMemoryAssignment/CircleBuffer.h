#pragma once
#include "structures.h"
#include "SharedMutex.h"
#include <Windows.h>
#include <memory>
#include <string>
using namespace std;

namespace SharedMemory
{
	//TODO
	/*Create this class that holds the handles and stuff

	Create functions, getter and setters, initialize fully in constructor*/

	/*
		Known problems:

		if a consumer reads from the buffer, and the producer disconnects. The consumer will wait for the write event
		in eternity. Remove write event!!
	*/


	class CircleBuffer
	{
	private:
		size_t*  shared_head		= 0;
		size_t*  shared_tail		= 0;
		size_t* freeMem;

		size_t  local_tail			= 0;
		size_t	messagesSent		= 0; // using this as an id system would need rework if it was to support multiple producers
		size_t	messagesRecieved	= 0;
		size_t chunkSize			= 256;

		shared_ptr<SharedMemory::SharedMemoryStruct> _MessageMem;
		shared_ptr<SharedMemory::SharedMemoryStruct> _InfoMem;

		std::unique_ptr<SharedMemory::SharedMutex> msgMutex;  // mutex											 
		std::unique_ptr<SharedMemory::SharedMutex> infoMutex; // mutex
		
	
	public:
	bool Push( void* msg, size_t length);
	bool Push(SharedData::SharedMessage* msg);
	bool Pop(char* msg, size_t& length);
	bool Pop(SharedData::SharedMessage* msg);
		CircleBuffer();
		bool Init(CommandArgs& info,
			LPCWSTR msgBufferName,
			LPCWSTR msgMutexName,
			LPCWSTR infoBufferName,
			LPCWSTR infoMutexName);
		void SetChunkSize(size_t size) { this->chunkSize = size; };
		shared_ptr<SharedMemory::SharedMemoryStruct> GetMessageBuffer() { return _MessageMem; };
		shared_ptr<SharedMemory::SharedMemoryStruct> GetInfoBuffer()	{ return _InfoMem;	  };
	

		virtual ~CircleBuffer();
	};
}

