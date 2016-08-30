#include "SharedMemoryHandler.h"



SharedMemoryHandler::SharedMemoryHandler()
{
}

SharedMemoryHandler::SharedMemoryHandler(CommandArgs & commands)
{
	//this->hMapFile = CreateFileMapping(
	//	INVALID_HANDLE_VALUE,
	//	NULL,
	//	PAGE_READWRITE,
	//	0,
	//	commands.memorySize,
	//	TEXT("Test")
	//	);
	//
	//if (hMapFile == NULL)
	//{
	//	_tprintf(TEXT("FAILED!"));
	//	DebugBreak();
	//}
	//
	//
	//pbuf = (LPTSTR)MapViewOfFile(hMapFile,
	//	FILE_MAP_ALL_ACCESS,
	//	0,
	//	0,
	//	commands.memorySize);
	//
	//if (pbuf == NULL)
	//{
	//	_tprintf(TEXT("FAILED!"));
	//	CloseHandle(hMapFile);
	//	DebugBreak();
	//}
	//TCHAR szMsg[] = TEXT("Message from first process.");
	//CopyMemory((PVOID)pbuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

}


SharedMemoryHandler::~SharedMemoryHandler()
{
	UnmapViewOfFile(pbuf);
	CloseHandle(hMapFile);
	CloseHandle(hMutex);
	CloseHandle(hWriteEvent);
}
