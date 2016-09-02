#pragma once
#include <windows.h>
namespace SharedMemory
{

	class SharedMutex
	{
	private:
		HANDLE mutexHandle;			 // mutex Handle
		SharedMutex();
	public:
		SharedMutex(LPCWSTR mutexName);
		bool Lock(DWORD waitTime = INFINITE);
		void Unlock();
		virtual ~SharedMutex();
	};
}

