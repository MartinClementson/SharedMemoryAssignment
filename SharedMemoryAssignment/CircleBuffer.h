#pragma once
#include "structures.h"
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
		in eternity
	*/


	class CircleBuffer
	{
	private:
		

		shared_ptr<SharedMemory::SharedMemoryStruct> _MessageMem;
		shared_ptr<SharedMemory::SharedMemoryStruct> _InfoMem;
		
	public:

		size_t CanRead(); //returns amount of bytes that can be read
		size_t CanWrite(); //returns amount of bytes that is free

	//bool Push(const void* msg, size_t length);
	//
	//bool Pop(char* msg, size_t& length);

		CircleBuffer();
		bool Init(CommandArgs& info,
			LPCWSTR msgBufferName,
			LPCWSTR infoBufferName);

		shared_ptr<SharedMemory::SharedMemoryStruct> GetMessageBuffer() { return _MessageMem; };
		shared_ptr<SharedMemory::SharedMemoryStruct> GetInfoBuffer()	{ return _InfoMem;	  };

		virtual ~CircleBuffer();
	};
}

