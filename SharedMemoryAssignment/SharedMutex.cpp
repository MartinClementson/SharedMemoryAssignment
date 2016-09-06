#include "SharedMutex.h"


using namespace SharedMemory;

SharedMutex::SharedMutex()
{
}

SharedMutex::SharedMutex(LPCWSTR mutexName)
{

	mutexHandle = OpenMutex(
		MUTEX_ALL_ACCESS,
		FALSE,
		mutexName);
	if (mutexHandle == NULL) // if it didnt open, create it
	{
		this->mutexHandle = CreateMutex(
			NULL,
			FALSE,
			mutexName);
		if (mutexHandle == NULL) // if it still doesent work, it's a critical error
		{
			MessageBox(GetConsoleWindow(), TEXT("Error creating mutex") + GetLastError(), TEXT("CRY"), MB_OK);
			DWORD error = GetLastError();
			throw 20;
		}
	}

}

bool SharedMutex::Lock(DWORD waitTime)
{
	DWORD waitResult = WaitForSingleObject(
		mutexHandle,
		waitTime);
	if (waitResult == WAIT_OBJECT_0)
		return true;
	else
		return false;
}

void SharedMutex::Unlock()
{
	if (!ReleaseMutex(mutexHandle))			// Release the ownership of the mutex so that other processes can access it
			MessageBox(GetConsoleWindow(), TEXT("COULD NOT RELEASE MUTEX!"), TEXT("Obsessed handle"), MB_OK);

}


SharedMutex::~SharedMutex()
{
	CloseHandle(mutexHandle);
}
