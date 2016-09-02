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
	
}


SharedMemoryHandler::~SharedMemoryHandler()
{
	//Close message file
	if (pMsgbuf			!= NULL)
		UnmapViewOfFile(pMsgbuf);
	if (hMsgMapFile		!= NULL)
		CloseHandle(hMsgMapFile);
	

	//close info file
	if (pInfobuf != NULL)
		UnmapViewOfFile(pInfobuf);
	if (hInfoMapFile != NULL)
		CloseHandle(hInfoMapFile);
	//Close Events
	if (hWriteEvent		 != NULL)
		CloseHandle(hWriteEvent);
	
}
