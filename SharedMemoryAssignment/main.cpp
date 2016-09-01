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

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

std::unique_ptr<SharedMemoryHandler> memoryHandle = nullptr;
bool process = true;

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	
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
		std::cout << "Not enough arguments" << std::endl;
		std::getchar();
		return -1;
	}

	CommandArgs commands = ParseCommands(&argc, argv); //parse the commands and put the information into the commands structure

	if(commands.producer == true)			//Create producer or consumer, depending on the commands
		memoryHandle = std::unique_ptr<Producer>(new Producer(commands));
	else
		memoryHandle = std::unique_ptr<Consumer>(new Consumer(commands));



	

	while (process)
	{

		memoryHandle->Exec();

	}
	




	
	//if(memoryHandle != nullptr)
		//delete memoryHandle;
	return 0;


}

CommandArgs ParseCommands(int* argc, char* argv[])
{
	CommandArgs toReturn;

	for (size_t i = 1; i < *argc; i++) //skip the first argument, it is only the executable line
	{
		std::cout << argv[i] << std::endl;
	}




#pragma region Producer|Consumer
	if (strcmp(argv[1], "producer") == 0)
		toReturn.producer = true;

	else if (strcmp(argv[1], "consumer") == 0)
		toReturn.producer = false;
	else
	{
		std::cout << "error reading commandline arguments" << std::endl;
		std::getchar();
	}
#pragma endregion


	toReturn.msDelay		= std::atof(argv[2]); //convert from string to double
	toReturn.memorySize		= std::atoi(argv[3]); //convert from string to int
	toReturn.numMessages	= std::atoi(argv[4]); //convert from string to int

	if (strcmp(argv[5], "random") == 0)
		toReturn.random = true;
	else
		toReturn.msgSize	= std::atoi(argv[5]); //convert from string to int

	return toReturn;

}


BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	char mesg[128];

	switch (CEvent)
	{
	
	case CTRL_CLOSE_EVENT:
		memoryHandle.~unique_ptr();
		process = false;
		MessageBox(NULL,
			TEXT("Program being closed!"), TEXT("CEvent"), MB_OK);
		Sleep(500);
		break;
	

	}
	return FALSE;
}

