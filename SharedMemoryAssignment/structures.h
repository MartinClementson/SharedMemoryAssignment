#pragma once
#include <Windows.h>
#include <string>
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
		unsigned int head;  //head index
		unsigned int tail;	//tail index

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

#pragma region SharedMemStruct
using namespace std;
namespace SharedMemory
{

	struct SharedMemoryStruct {
	
		HANDLE	hFileMap = NULL;
		LPCTSTR vFileView = NULL;
		SharedMemoryStruct() {};
	
		SharedMemoryStruct(CommandArgs info, LPCWSTR bufferName)
		{
	
			//try opening it first
			this->hFileMap = OpenFileMapping(
				FILE_MAP_ALL_ACCESS,
				FALSE,
				bufferName);
			if (hFileMap == NULL) //if it didnt open, create it
			{
				this->hFileMap = CreateFileMapping(
					INVALID_HANDLE_VALUE,	//Instead of a file in the system, we use a system paging file
					NULL,					//No extra attributes (default)
					PAGE_READWRITE,			//specifies the protection, all the views to the file need to på compatible with this!
					0,
					(info.memorySize * 1 << 20), //convert to megabyte
					bufferName
					);
	
				if (hFileMap == NULL) //if it still doesent work, throw error
				{
					MessageBox(GetConsoleWindow(), TEXT("Error creating fileMap"), TEXT("SharedMemoryStruct"), MB_OK);
					throw(string("Could not create fileMap"));
				}
			}
	
			vFileView = (LPTSTR)MapViewOfFile(hFileMap, //Create the view
				FILE_MAP_ALL_ACCESS,
				0,
				0,
				(info.memorySize * 1 << 20)); //convert to megabyte
			if (vFileView == NULL)
			{
				MessageBox(GetConsoleWindow(), TEXT("Error when mapping file view"), TEXT("SharedMemoryStruct"), MB_OK);
				throw(string("Could not map file view"));
			}

		}
		~SharedMemoryStruct() {
			if (hFileMap != NULL)
				CloseHandle(hFileMap);
			if (vFileView != NULL)
				UnmapViewOfFile(vFileView);
		}
	};
}	

#pragma endregion