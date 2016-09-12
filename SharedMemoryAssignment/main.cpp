#include "structures.h"
#include <iostream>
#include <csignal>
#include <string>
#include <stdlib.h>
#include "SharedMemoryHandler.h"
#include "Consumer.h"
#include "Producer.h"
#include <memory>
CommandArgs ParseCommands(int* argc, char* argv[]);
BOOL WINAPI ConsoleHandler(DWORD CEvent);
void SetUpEvents();

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


bool process = true;
HANDLE closeEventHandle;

int main(int argc, char* argv[])
{
	/* TODO
	if custom message size
	Check that the message size is not bigger than  a quarter of the total memory size*/
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	SetUpEvents();

	if (SetConsoleCtrlHandler(
		(PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
	{
		// unable to install handler... 
		// display message to the user
		printf("Unable to install handler!\n");
		return -1;
	}


	if (argc < 5)
	{
		MessageBox(GetConsoleWindow(), TEXT("Not enough arguments"), TEXT("Error"), MB_OK);
		std::getchar();
		return -1;
	}


	{ //scope for the smart pointer
		std::unique_ptr<SharedMemoryHandler> memoryHandle = nullptr;

		CommandArgs commands = ParseCommands(&argc, argv); //parse the commands and put the information into the commands structure

		if (commands.producer == true)			//Create producer or consumer, depending on the commands
		{

			memoryHandle = std::unique_ptr<Producer>(new Producer(commands));
			Sleep(5000); //so that all consumers can start.
		}
		else
			memoryHandle = std::unique_ptr<Consumer>(new Consumer(commands));



		

		while (process)
			if (memoryHandle->Exec() == false) //run the memory program.
				process = false;
		
	} //smart pointer deletes the data


	return 0;


}

void SetUpEvents()
{
	closeEventHandle = CreateEvent(
		NULL,				 //default security attr
		FALSE,				 //Automatic reset event
		FALSE,			     // non signaled when initializing
		CLOSE_EVENT_NAME  // name
		);
	if (closeEventHandle == NULL)
	{
	
		MessageBox(GetConsoleWindow(), TEXT("Could not init closeEvent"), TEXT("Abandon hope"), MB_OK);
	}


}

CommandArgs ParseCommands(int* argc, char* argv[])
{
	CommandArgs toReturn;

//for (size_t i = 1; i < *argc; i++) //skip the first argument, it is only the executable line
//{
//	std::cout << argv[i] << std::endl;
//}




#pragma region Producer|Consumer
	if (strcmp(argv[1], "producer") == 0)
		toReturn.producer = true;

	else if (strcmp(argv[1], "consumer") == 0)
		toReturn.producer = false;
	else
	{
		MessageBox(GetConsoleWindow(), TEXT("error reading commandline arguments"), TEXT("Error"), MB_OK);
		
	}
#pragma endregion


	toReturn.msDelay		= std::atof(argv[2]); //convert from string to double
	toReturn.memorySize		= std::atoi(argv[3]); //convert from string to int
	toReturn.numMessages	= std::atoi(argv[4]); //convert from string to int

	if (strcmp(argv[5], "random") == 0 || std::atoi(argv[5]) == 0)
		toReturn.random = true;
	else
		toReturn.msgSize	= std::atoi(argv[5]); //convert from string to int

	return toReturn;

}


BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	char msg[128];

	switch (CEvent)
	{
	
	case CTRL_CLOSE_EVENT:
	
		//SetEvent(closeEventHandle);
		process = false;
		//MessageBox(NULL,
		//	TEXT("Program being closed!"), TEXT("CEvent"), MB_OK);
		Sleep(500000);
		break;
	

	}
	return FALSE;
}

