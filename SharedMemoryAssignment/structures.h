#pragma once
#include <Windows.h>
#include <string>

#define CLOSE_EVENT_NAME (LPCWSTR)TEXT("CloseEvent")

struct CommandArgs
{
	bool producer			 = false;
	double msDelay			 = 0;
	unsigned int memorySize  = 0;
	unsigned int numMessages = 0;
	bool random				 = false;
	unsigned int msgSize	 = 0;
};

namespace SharedData
{

	struct SharedInformation
	{
		size_t head		= 0;	//head offset
		size_t tail		= 0;	//tail offset
		size_t freeMem  = 0;	
		size_t clients  = 0;
	};

	struct MesssageHeader
	{
		int consumerQueue; // it helps that this can be negative. So that consumers can join mid-session without problem
		size_t msgId;
		size_t length;

		void Flush()
		{
			consumerQueue = 0;
			msgId		  = 0;
			length		  = 0;
		}

	};

	struct SharedMessage
	{
		MesssageHeader header;
		char* message = nullptr;
		SharedMessage() {};
		~SharedMessage()
		{
			if (message != nullptr)
				delete message;
				
		}
		void Flush()
		{
			//memset(this->message, '0', header.length);
			header.Flush();

		}
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

struct InitReturnValues
{
	bool firstTimeInit = false;
	bool succeded = false;


};

#pragma region SharedMemStruct
using namespace std;
namespace SharedMemory
{

	struct SharedMemoryStruct {
		size_t fileSize		= 0;
		HANDLE	hFileMap	= NULL;
		LPCTSTR vFileView	= NULL;
		SharedMemoryStruct() {};
	
		InitReturnValues Init(CommandArgs* info, LPCWSTR bufferName)
		{
			InitReturnValues toReturn;
			
			this->fileSize = size_t(info->memorySize);
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
					PAGE_READWRITE,			//specifies the protection, all the views to the file need to p� compatible with this!
					0,
					info->memorySize,	 // unsigned int
					bufferName
					);
	
				if (hFileMap == NULL) //if it still doesent work, throw error
				{

					MessageBox(GetConsoleWindow(), TEXT("Error creating fileMap"), TEXT("SharedMemoryStruct"), MB_OK);
					return toReturn;
				}
				else
					toReturn.firstTimeInit = true;
			}
	
			vFileView = (LPTSTR)MapViewOfFile(hFileMap, //Create the view
				FILE_MAP_ALL_ACCESS,
				0,
				0,
				info->memorySize ); 
			if (vFileView == NULL)
			{
				MessageBox(GetConsoleWindow(), TEXT("Error when mapping file view"), TEXT("SharedMemoryStruct"), MB_OK);
				return toReturn;
			}
			
			SharedData::SharedInformation* test = (SharedData::SharedInformation*) vFileView; //this should contain data already!

			toReturn.succeded = true;
			return toReturn;

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