#include "structures.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include "SharedMemoryHandler.h"
#include "Consumer.h"
#include "Producer.h"
CommandArgs ParseCommands(int* argc, char* argv[]);

int main(int argc, char* argv[])
{
	SharedMemoryHandler* memoryHandle = nullptr;

	if (argc < 5)
	{
		std::cout << "Not enough arguments" << std::endl;
		std::getchar();
		return -1;
	}
	CommandArgs commands = ParseCommands(&argc, argv); //parse the commands and put the information into the commands structure

	if(commands.producer == true)
		memoryHandle = new Producer(commands);
	else
		memoryHandle = new Consumer(commands);


	memoryHandle->Exec();
	std::getchar();
	if(memoryHandle != nullptr)
		delete memoryHandle;
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