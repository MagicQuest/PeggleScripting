#include "pch.h"
#include "FileWatcher.h"

HANDLE FileWatcher::thread;
DWORD FileWatcher::threadID;
HANDLE FileWatcher::newFileEvent;

volatile wchar_t* FileWatcher::ThreadOnly::filename;
HANDLE FileWatcher::ThreadOnly::fileChangeNotification;

void FileWatcher::SetFileName(const wchar_t* filename) {
    //ooohhh this is getting weird
    //i might have to make another project to make sure im doing this right (yeah i did and im gonna add my changes in the next commit lol)
    char* atomic_filename = (char*)InterlockedCompareExchangePointer((volatile PVOID*)&ThreadOnly::filename, 0, 0);
    if (atomic_filename[0] != '\0') {
        free(atomic_filename);
    }
    wchar_t* alloc_filename = (wchar_t*)malloc(wcslen(filename)+2);
    if (!alloc_filename) {
        MessageBoxA(NULL, "malloc failed in SetFileName", "yeah we gonna crash the process", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
    }
    wcscpy(alloc_filename, filename);
    //ThreadOnly::filename = alloc_filename;
    //they shouldn't read and write at the same time or whatever but just to be on the thread safe side
    InterlockedExchange((volatile unsigned long long*)&ThreadOnly::filename, (unsigned long long)alloc_filename);
}

void FileWatcher::StartThread(const wchar_t* filename) {
    //using namespace FileWatcher;
    SetFileName(filename);
    thread = CreateThread(NULL, 0, ThreadOnly::ThreadProc, NULL, 0, &threadID);
}

DWORD WINAPI FileWatcher::ThreadOnly::ThreadProc(LPVOID lpParam) {
    //stack list because they are not a regular listlol
    //FindFirstChangeNotificationA()
    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    //char __dirname[MAX_PATH]{};
    _wsplitpath((const wchar_t*)ThreadOnly::filename, drive, dir, NULL, NULL);
    std::wstring lazystring = std::wstring(drive) + dir;
    ThreadOnly::fileChangeNotification = FindFirstChangeNotification(lazystring.c_str(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
    
    HANDLE events[] = {
        FileWatcher::newFileEvent,
        FileWatcher::ThreadOnly::fileChangeNotification,
    };
    while (true) {
        DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        switch (result) {
            case WAIT_OBJECT_0:
                //newFileEvent
                FindCloseChangeNotification(ThreadOnly::fileChangeNotification);
                //FindFirstChangeNotification()
                break;

            case WAIT_OBJECT_0 + 1:
                //fileChangeNotification
                FindNextChangeNotification(ThreadOnly::fileChangeNotification);
                break;

            case WAIT_TIMEOUT:
                //shouldn't happen lmao
                MessageBoxA(NULL, "fym WaitForMultipleObjects returned WAIT_TIMEOUT", "well that shouldn't have happened but it actually doesn't matter", MB_OK);
                break;

            default:
                DWORD g = GetLastError();
                printf("Wait error: %d\n", g);
                __debugbreak();
        }
    }
}

/*void FileWatcher::CloseThread() {
    
}*/