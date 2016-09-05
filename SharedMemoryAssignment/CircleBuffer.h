#pragma once
#include "structures.h"
#include <Windows.h>
#include <memory>
#include <string>
using namespace std;
namespace SharedMemory
{
	class CircleBuffer;
}

//TODO
/*Create this class that holds the handles and stuff

Create functions, getter and setters, initialize fully in constructor*/


class CircleBuffer
{
#pragma region SharedMemStruct
	struct SharedMemoryStruct {

		HANDLE	hFileMap  = NULL;
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
					(info.memorySize * 1<<20), //convert to megabyte
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
			if (hFileMap  != NULL)
				CloseHandle(hFileMap);
			if (vFileView != NULL)
				UnmapViewOfFile(vFileView);
		}
	};
#pragma endregion

private:
	SharedMemoryStruct messageMem;
	SharedMemoryStruct infoMem;

	unique_ptr<SharedMemoryStruct> _MessageMem;
	unique_ptr<SharedMemoryStruct> _InfoMem;  
public:
	CircleBuffer();
	CircleBuffer(CommandArgs info,
		LPCWSTR msgBufferName,
		LPCWSTR infoBufferName);

	virtual ~CircleBuffer();
};

