#pragma once
namespace FileWatcher
{
	extern HANDLE thread;
	extern DWORD threadID;
	void SetFileName(const wchar_t* filename);
	void StartThread(const wchar_t* filename);

	extern HANDLE newFileEvent;

	//void CloseThread();
	namespace ThreadOnly {
		extern volatile wchar_t* filename;
		DWORD WINAPI ThreadProc(LPVOID lpParam);
		extern HANDLE fileChangeNotification;
	}
};

