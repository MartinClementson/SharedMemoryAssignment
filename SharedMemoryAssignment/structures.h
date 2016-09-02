#pragma once

#define CLOSE_EVENT_NAME (LPCWSTR)TEXT("Local\CloseEvent")

struct CommandArgs
{
	unsigned int numMessages		= 0;
	unsigned int memorySize = 0;

	bool producer = false;
	bool random	  = false;
	unsigned int msgSize	= 0;

	double msDelay;


};

namespace SharedData
{

	struct SharedInformation
	{
		unsigned int numProcesses = 0;

		//buffer info here
	};

	struct MesssageHeader
	{
		size_t consumerQueue;
		size_t msgId;
		size_t length;


	};
}

struct SharedCircleBuffer
{
	//LPCTSTR pBuffer; //pointer to the data
	unsigned int bufferSize;
	unsigned int head;  //head index
	unsigned int tail;	//tail index
	unsigned int count; //total amount of elements currently in the queue


//SharedCircleBuffer(LPCTSTR fileView,
//	unsigned int bufferSize,
//	unsigned int head,
//	unsigned int tail,
//	unsigned int count)
//{
//	//this->pBuffer		= fileView;
//	this->bufferSize	= bufferSize;
//	this->head			= head;  
//	this->tail			= tail;
//	this->count			= count;
//
//
//};
	/*	  
		Functions
		Init() clear data, set head==tail
		Empty() checks if it's empty
		Full()	checks if its full
		Get() Get a byte from the queue (TAIL)
		Put() Put a byte to the queue (HEAD)
		Flush() Flush the queue and clear the bytes to 0
	*/

};