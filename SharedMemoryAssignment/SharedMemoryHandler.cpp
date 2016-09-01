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
	if (pbuf		     != NULL)
		UnmapViewOfFile(pbuf);
	if (hMapFile		 != NULL)
		CloseHandle(hMapFile);
	if (hMutex			 != NULL)
		CloseHandle(hMutex);
	if (hWriteEvent		 != NULL)
		CloseHandle(hWriteEvent);
	if (hConnectEvent	 != NULL)
		CloseHandle(hConnectEvent);
	if (hDisconnectEvent != NULL)
		CloseHandle(hDisconnectEvent);
	if (hCloseEvent		 != NULL)
		CloseHandle(hCloseEvent);
}
