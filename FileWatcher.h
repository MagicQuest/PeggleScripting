#pragma once
namespace FileWatcher
{
	extern HANDLE thread;
	extern DWORD threadID;
	void SetFileName(const wchar_t* filename, BOOL bAlert);
	void StartThread(const wchar_t* filename);

	extern HANDLE newFileEvent;

	//void CloseThread();
	namespace ThreadOnly {
		extern volatile wchar_t* filename;
		HANDLE GetFileChangeNotificationFromPath(const wchar_t* path);
		DWORD WINAPI ThreadProc(LPVOID lpParam);
		extern HANDLE fileChangeNotification;
		
		//these were used with ReadDirectoryChangesW but i didn't want to use it lol
		//HANDLE OpenDirectory(const wchar_t* path);
		//extern std::wstring directoryString;
		//extern HANDLE directory;
	}
};