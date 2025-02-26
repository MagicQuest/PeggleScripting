#include "pch.h"
#include "FileWatcher.h"
#include "v8helper2.h"

#pragma push_macro(print)
#pragma push_macro(wprint)
#define print(msg) std::cout << "[FileWatcher] " << msg << std::endl
#define wprint(msg) std::wcout << L"[FileWatcher] " << msg << std::endl

HANDLE FileWatcher::thread;
DWORD FileWatcher::threadID;
HANDLE FileWatcher::newFileEvent;

volatile wchar_t* FileWatcher::ThreadOnly::filename;
HANDLE FileWatcher::ThreadOnly::fileChangeNotification;

void FileWatcher::SetFileName(const wchar_t* filename, BOOL bAlert) {
    //ooohhh this is getting weird
    //i might have to make another project to make sure im doing this right
    wprint(filename);
    char* atomic_filename = (char*)InterlockedCompareExchange((volatile ULONG_PTR*)&ThreadOnly::filename, 0, 0);
    if (atomic_filename && atomic_filename[0] != '\0') {
        wprint(L"releasing old atomic_filename");
        free(atomic_filename);
    }
    wchar_t* alloc_filename = (wchar_t*)malloc((wcslen(filename) + 1) * sizeof(wchar_t)); //aw man i thought wcslen returned the size in bytes lmao (i was fucking up the heap)
    if (!alloc_filename) {
        MessageBoxA(NULL, "malloc failed in SetFileName", "yeah we gonna crash the process", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
    }
    wcscpy(alloc_filename, filename);
    wprint(L"alloc " << alloc_filename);
    //ThreadOnly::filename = alloc_filename;
    //they shouldn't read and write at the same time or whatever but just to be on the thread safe side
    InterlockedExchange((volatile ULONG_PTR*)&ThreadOnly::filename, (ULONG_PTR)alloc_filename);

    if (bAlert) {
        SetEvent(FileWatcher::newFileEvent);
    }
}

void FileWatcher::StartThread(const wchar_t* filename) {
    //using namespace FileWatcher;
    SetFileName(filename, FALSE);
    FileWatcher::newFileEvent = CreateEvent(NULL, FALSE, FALSE, L"NEW_FILE_EVENT");
    thread = CreateThread(NULL, 0, ThreadOnly::ThreadProc, NULL, 0, &threadID);
    if (!thread) {
        MessageBoxA(NULL, "CreateThread for FileWatcher failed?", "fuck.", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
    }
}

//-----------------------------------ThreadOnly-----------------------------------//

HANDLE FileWatcher::ThreadOnly::GetFileChangeNotificationFromPath(const wchar_t* path) {
    //FindFirstChangeNotificationA()
    wprint(L"waiting for " << path);

    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    //char __dirname[MAX_PATH]{};
    _wsplitpath((const wchar_t*)path, drive, dir, NULL, NULL);
    std::wstring lazystring = std::wstring(drive) + dir;
    return FindFirstChangeNotification(lazystring.c_str(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
}

DWORD WINAPI FileWatcher::ThreadOnly::ThreadProc(LPVOID lpParam) {
    {
        wchar_t* atomic_filename = (wchar_t*)InterlockedCompareExchange((volatile ULONG_PTR*)&ThreadOnly::filename, 0, 0);
        ThreadOnly::fileChangeNotification = ThreadOnly::GetFileChangeNotificationFromPath((const wchar_t*)atomic_filename);
    }
    //stack list because they are not a regular listlol
    while (true) {
        HANDLE events[] = {
            FileWatcher::newFileEvent,
            FileWatcher::ThreadOnly::fileChangeNotification,
        };
        DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        wprint(L"result " << result);
        switch (result) {
            case WAIT_OBJECT_0: {
                //newFileEvent
                BOOL result = FindCloseChangeNotification(ThreadOnly::fileChangeNotification);
                if (!result) {
                    wprint(L"fuck. FindCloseChangeNotification failed " << GetLastError());
                }
                wchar_t* atomic_filename = (wchar_t*)InterlockedCompareExchange((volatile ULONG_PTR*)&ThreadOnly::filename, 0, 0);
                ThreadOnly::fileChangeNotification = ThreadOnly::GetFileChangeNotificationFromPath((const wchar_t*)atomic_filename);
                //std::cout << "checking new file -> " << atomic_filename << std::endl;
                wprint(L"checking new file -> " << atomic_filename);
                break;
            }

            case WAIT_OBJECT_0 + 1: {
                //fileChangeNotification
                //std::cout << "re read file!" << std::endl;
                print("re read file!");
                v8helper2::Notify(V8_NOTIFY_FILECHANGE);
                BOOL result = FindNextChangeNotification(ThreadOnly::fileChangeNotification);
                if (!result) {
                    wprint(L"FindNextChangeNotification failed!" << GetLastError());
                }
                break;
            }

            case WAIT_TIMEOUT:
                //shouldn't happen lmao
                MessageBoxA(NULL, "fym WaitForMultipleObjects returned WAIT_TIMEOUT", "well that shouldn't have happened but it actually doesn't matter", MB_OK);
                break;

            default:
                DWORD g = GetLastError();
                printf("Result: %d\n", result);
                printf("Wait error: %d\n", g);
                __debugbreak();
        }
    }
}

/*void FileWatcher::CloseThread() {

}*/

#pragma pop_macro(wprint)
#pragma pop_macro(print)