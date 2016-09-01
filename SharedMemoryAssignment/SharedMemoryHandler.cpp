#include "SharedMemoryHandler.h"



LPCWSTR SharedMemoryHandler::GetFileName(Files fileName)
{

	switch (fileName)
	{
	case Files::MessageFile:
		return this->msgFileName;
		break;
	case Files::InformationFile:
		return this->infoFileName;
		break;

	}
	return LPCWSTR();
}

LPCWSTR SharedMemoryHandler::GetMutexName(Files fileName)
{

	switch (fileName)
	{
	case Files::MessageFile:
		return this->msgMutexName;
		break;
	case Files::InformationFile:
		return this->infomutexName;
		break;

	}
	return LPCWSTR();
}

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
	//Close message file
	if (pMsgbuf			!= NULL)
		UnmapViewOfFile(pMsgbuf);
	if (hMsgMapFile		!= NULL)
		CloseHandle(hMsgMapFile);
	if (hMsgMutex		!= NULL)
		CloseHandle(hMsgMutex);

	//close info file
	if (pInfobuf != NULL)
		UnmapViewOfFile(pInfobuf);
	if (hInfoMapFile != NULL)
		CloseHandle(hInfoMapFile);
	if (hInfoMutex != NULL)
		CloseHandle(hInfoMutex);

	//Close Events
	if (hWriteEvent		 != NULL)
		CloseHandle(hWriteEvent);
	
}
