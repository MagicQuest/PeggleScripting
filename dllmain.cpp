// dllmain.cpp : Defines the entry point for the DLL application.

#ifndef WIN32
#define V8_ENABLE_SANDBOX
#define V8_COMPRESS_POINTERS
#endif
#define V8_31BIT_SMIS_ON_64BIT_ARCH
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
//#include "v8helper.h"
#include "FileWatcher.h"
#include "v8helper2.h"
#include "dllmain.h"

HMODULE g_thismodule;
void* g_peggle;

//v8helper* virtualeight;

HMODULE extended_binary;
wchar_t g_filename[MAX_PATH]{};

//typedef now defined in dllmain.h
//typedef v8::Local<v8::ObjectTemplate>(*JBSInit)(v8::Isolate*, const wchar_t*, int);

JBSInit daddygibmemyglobals = nullptr;

DWORD g_eax;
DWORD g_ecx;
DWORD g_edx;
DWORD g_ballCount;

//void* g_logicmgr; //RTTI says that this random important struct is called the LogicMgr
//void* g_board;
//void* g_ball;

//HANDLE g_watcherthread;
//HANDLE notifyEvent;

//typedef void(__thiscall *activateAbilitySignature)(void*, void*, DWORD, DWORD);
//activateAbilitySignature g_peggle_internal_activate_ability;
void* g_peggle_internal_activate_ability;

void showOpenFilePicker(wchar_t* out_file_name, size_t length) {
    OPENFILENAME ofn{};
    wchar_t file_name[MAX_PATH]{};

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    wchar_t filter[MAX_PATH]{};

    GetOpenFileName(&ofn);
    //_alloca()

    if (+file_name[0]) {
        memcpy(out_file_name, file_name, length);
    }
}

//Highlight defined in v8helper.cpp

extern "C" {
    __declspec(dllexport) void __stdcall ChooseLeScript() {
        //std::wstring shit;

        showOpenFilePicker(g_filename, MAX_PATH * sizeof(wchar_t));
    }
    __declspec(dllexport) void __stdcall ChooseExtendedGlobals() {
        wchar_t path[MAX_PATH];
        showOpenFilePicker(path, MAX_PATH * sizeof(wchar_t));
        extended_binary = LoadLibrary(path);
        if (extended_binary) {
            daddygibmemyglobals = (JBSInit)GetProcAddress(extended_binary, "InitGlobals");
            if (daddygibmemyglobals == NULL) {
                OutputDebugString(L"GetProcAddress somehow failed?");
            }
        }
        else {
            OutputDebugString(L"ayo that shit didn't work we couldn't load library that bih");
            return;
        }
    }
    //__declspec(dllexport) DWORD __stdcall RunLeCode(DWORD eax, DWORD ecx, DWORD edx, DWORD* ballCount, void* mainstruct) { //aw shit i expected all my functions to use __stdcall even though i didn't add it
    //    //__debugbreak();
    //    if (nullptr == virtualeight) {
    //        char exe[MAX_PATH]; GetModuleFileNameA(g_thismodule, exe, MAX_PATH);
    //        print(exe);
    //
    //        virtualeight = new v8helper(exe);
    //    }
    //    bool newFile = g_filename[0] == '\0';
    //    while (g_filename[0] == '\0') {
    //        ChooseLeScript();
    //    }
    //    if (newFile) {
    //        //g_watcherthread = FileWatcher::StartThread();
    //        //FileWatcher::StartThread(g_filename);
    //    }
    //    g_eax = eax;
    //    g_ecx = ecx;
    //    g_edx = edx;
    //    g_ballCount = *ballCount;
    //    g_logicmgr = mainstruct;
    //    print(ULONG_PTR(g_logicmgr));
    //    print(ULONG_PTR(g_logicmgr) + 0x104);
    //    print(*(ULONG_PTR*)(ULONG_PTR(g_logicmgr) + 0x104)); //address of board
    //    g_board = *(void**)(ULONG_PTR(g_logicmgr) + 0x104);
    //    print(ULONG_PTR(g_board));
    //    print(ULONG_PTR(g_board) + 0x1A0);
    //
    //    //the ball might not always be valid since i eval 
    //    g_ball = TryGetBallFromBoard(g_board);
    //    //g_ball = *(void**)((**(ULONG_PTR**)(ULONG_PTR(g_board) + 0x1A0)) + 8);
    //    //so lame no capture we gotta go global son
    //    //virtualeight->RunLeScript(filename, [](v8::Isolate* isolate) -> v8::Local<v8::ObjectTemplate> {
    //        
    //    //    });
    //    virtualeight->RunLeScript(g_filename, InitPeggleGlobals);
    //
    //    print("g_returnval " << g_returnval);
    //
    //    return g_returnval;
    //}
    /*__declspec(dllexport) void __stdcall Eval() {
        SetForegroundWindow(GetConsoleWindow());
        if (nullptr == virtualeight) {
            char exe[MAX_PATH]; GetModuleFileNameA(g_thismodule, exe, MAX_PATH);
            print(exe);

            virtualeight = new v8helper(exe);
        }
        
        //oops i didn't initalize g_logicmgr or g_ballCount here
        g_board = GetBoard();
        g_logicmgr = GetLogicMgrFromBoard(g_board); //*(void**)(ULONG_PTR(g_board) + 0x154); //yeah
        //g_ballCount = *(DWORD*)(ULONG_PTR(g_logicmgr) + 0x17C);
        g_ball = TryGetBallFromBoard(g_board);

        wchar_t scriptwstr[256];
        std::wcout << L">>> ";
        std::wcin.getline(scriptwstr, 256);
        virtualeight->Eval((const uint16_t*)scriptwstr, InitPeggleGlobals, true);
    }*/
    __declspec(dllexport) DWORD __stdcall RunEvent(DWORD id, DWORD eax, DWORD ecx, DWORD edx) {
        if (id != V8_NOTIFY_EVAL && g_filename[0] == '\0') {
            while (g_filename[0] == '\0') {
                ChooseLeScript();
            }
            FileWatcher::StartThread(g_filename);
        }
        if (!v8helper2::thread) {
            //should be good...
            v8helper2::thread = CreateThread(NULL, 0, v8helper2::SetupLoop, NULL, 0, &v8helper2::threadID);
            if (!v8helper2::thread) {
                MessageBoxA(NULL, "oouuuuhh something went wrong creating the v8 thread", "yeah this multithreading stuff is not my cup of tea", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
                __debugbreak();
            }
        }
        g_eax = eax;
        g_ecx = ecx;
        g_edx = edx;
        v8helper2::Notify(id);
        DWORD waitTime = 15000;
        if (id == V8_NOTIFY_FILECHANGE) {
            //don't wait because i lowkey don't send a response for this one
            return 0;
        }
        else if (id == V8_NOTIFY_EVAL) {
            waitTime = INFINITE;
        }
        DWORD result = WaitForSingleObject(v8helper2::finishEvent, waitTime); //yeah im not sure why this started hanging so clearly i shouldn't use infinite lmao
        if (result == WAIT_OBJECT_0) {
            return v8helper2::returnval; //well im setting it on a diff thread but i wait for it so...
        }
        else {
            print("(RunEvent) WaitForSingleObject failed with " << result << " " << GetLastError());
            MessageBoxA(NULL, NULL, NULL, MB_OK);
        }
        return -1;
    }
}

void* FindPeggleBase() {
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32{};

    DWORD pid = GetCurrentProcessId();

    //  Take a snapshot of all modules in the specified process. 
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        print("CreateToolhelp32Snapshot (of modules)");
        return(FALSE);
    }

    //  Set the size of the structure before using it. 
    me32.dwSize = sizeof(MODULEENTRY32);

    //  Retrieve information about the first module, 
    //  and exit if unsuccessful 
    if (!Module32First(hModuleSnap, &me32))
    {
        print("Module32Firstfailed");  // Show cause of failure 
        CloseHandle(hModuleSnap);     // Must clean up the snapshot object! 
        return(FALSE);
    }

    void* peggle = nullptr;

    //  Now walk the module list of the process, 
    //  and display information about each module 
    do
    {
        wprintf(L"\n\n     MODULE NAME:     %s", me32.szModule);
        wprintf(L"\n     executable     = %s", me32.szExePath);
        wprintf(L"\n     process ID     = 0x%08X", me32.th32ProcessID);
        wprintf(L"\n     ref count (g)  =     0x%04X", me32.GlblcntUsage);
        wprintf(L"\n     ref count (p)  =     0x%04X", me32.ProccntUsage);
        wprintf(L"\n     base address   = 0x%08X", (DWORD)me32.modBaseAddr);
        wprintf(L"\n     base size      = %d", me32.modBaseSize);

        if (wcscmp(L"popcapgame1.exe", me32.szModule) == 0) { //bruh i forgot == 0
            peggle = me32.modBaseAddr;
            print("found peggle");
        }

    } while (Module32Next(hModuleSnap, &me32));

    printf(("\n"));

    //  Do not forget to clean up the snapshot object. 
    CloseHandle(hModuleSnap);

    if (peggle) {
        char buffer[100]; //idgaf
        snprintf(buffer, 100, "don't worry we goot em. %u", peggle);
        print(buffer);
        OutputDebugStringA(buffer);
    }
    return peggle;
}

void* WriteAllocatedBytes(BYTE* bytes, size_t length, ULONG_PTR jmpLocation, bool writejump = true) {
    void* alloc = VirtualAlloc(NULL, length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!alloc) {
        MessageBoxA(NULL, "(Overwriting) VirtualAlloc failed!", "fuck. this might crash the process now", MB_OK);
        __debugbreak(); //KeBugCheck
    }
    print(alloc);
    memcpy(alloc, bytes, length);
    if (writejump) {
        DWORD* jmpRelBytes = (DWORD*)((DWORD)alloc + length - 4);
        *jmpRelBytes = (jmpLocation)-(DWORD(alloc) + length); //-5); //(DWORD)alloc - (modifyBallCount + 15 + 7);
    }
    return alloc;
}

//static inline void* bytecopy(void* const dest, void const* const src, size_t bytes)
//{
//    while (bytes-- > (size_t)0)
//        ((unsigned char*)dest)[bytes] = ((unsigned char const*)src)[bytes];
//
//    return dest;
//}

void WriteJump(ULONG_PTR destination, ULONG_PTR source, BYTE* overwriteBytes, size_t overwriteLength) {
    DWORD relativeAddr = (DWORD)destination - (source + 5); //+5 for the jump instruction itself
    BYTE* dwordarr = (BYTE*)&relativeAddr;

    //*(DWORD*)(ULONG_PTR(overwriteBytes) + 1) = relativeAddr; //dwordarr;
    //print("what the fuck " << (DWORD)overwriteBytes[0]);
    overwriteBytes[1] = dwordarr[0];
    overwriteBytes[2] = dwordarr[1];
    overwriteBytes[3] = dwordarr[2];
    overwriteBytes[4] = dwordarr[3];
    //print("1 " << (DWORD)overwriteBytes[1]);
    //print("2 " << (DWORD)overwriteBytes[2]);
    //print("3 " << (DWORD)overwriteBytes[3]);
    //print("4 " << (DWORD)overwriteBytes[4]);
    
    //print("0 again " << (DWORD)overwriteBytes[0]);
    //original size of the add instruction at modifyBallCount + 15 is 7 so let's put some int3 or something
    //add dword ptr ds:[esi+eax*4+0x17C], ecx
    //aw damn this failed because i forgot that executable pages aren't normally supposed to be written to and thus, don't have the write access flag or whatever
    DWORD oldprotectionidc;
    if (!VirtualProtect((void*)(source), overwriteLength, PAGE_EXECUTE_READWRITE, &oldprotectionidc)) {
        OutputDebugStringA("that ol'e VirtualProtect failed");
        MessageBoxA(NULL, "(Overwrite) VirtualAlloc failed!", "fuck. this will crash the process with an access violation", MB_OK);
        __debugbreak();
    }
    //print("cpy to source " << (ULONG_PTR)source);
    //print("cverwritebiyes " << overwriteBytes << " len " << overwriteLength);
    memcpy((void*)(source), overwriteBytes, overwriteLength);
    //bruhhhhh when i was using this function with OverwriteLevelLoadEnd it wouldn't write the first byte for some reason and it was because i had a breakpoint there with x32dbg :(
    //bytecopy((void*)(source), overwriteBytes, overwriteLength);
}

void OverwriteModifyBallPtr(void* peggle) {
    //so we need to find the modify ball ptr func
    //in the PE file, it's located at 0005D880h bytes from the base (i found this out by putting the pointer to the function as it was already loaded in memory into the VA section of the address converter (fortunately the exe was loaded where it wanted to be (OptionalHeader->ImageBase == 00400000h)))
    //all we gotta do is find the base + this offset
    //https://stackoverflow.com/questions/2170843/va-virtual-address-rva-relative-virtual-address
    constexpr DWORD offset = 0x0005D880;
    //lowkey i COULD call load library on the peggle exe itself to find the base BUT you probably aren't supposed to call LoadLibrary in the DllMain...
    //
    DWORD modifyBallCount = (DWORD)peggle + offset;
    //DWORD nextInstruction = modifyBallCount + 15 + 7; //well write that in manually

    //__debugbreak();

    /*DWORD RLE_PTR = (DWORD)&RunLeCode;
    BYTE* RLE_PTR_BYTES = (BYTE*)&RLE_PTR;

    BYTE bytes[] = { //https://defuse.ca/online-x86-assembler.htm#disassembly
        //0x50,       //push eax
        //0x68,       //push imm32
        0x53,                                                                           //push ebx

        //0x51,                                                                                          //push ecx

        0x52,                                                                           //push edx

        //0x57,                                                                                          //push edi

        0x50,                                                                           //push eax
        //0x50,                                                                           //push eax again because im lazy and don't feel like finding it on the stack (wait nevermind wouldn't it be just esp because i just pushewd it)

        //push arguments to the stack for RunLeCode (since i made RunLeCode use __stdcall)
        0x8d, 0x84, 0x86, 0x7c, 0x01, 0x00, 0x00,                                       //lea eax, [esi+eax*4+0x17C] //ball count ptr
        0x56,                                                                           //oush esi
        0x50,                                                                           //push eax
        0x52,                                                                           //push edx
        0x51,                                                                           //push ecx
        0xff, 0x74, 0x24, 0x10,                                                         //push DWORD PTR [esp+0x10]
        //0x68, 0x8d, 0x63, 0x01, 0x00,                                                   //push 91021

        //move the address of RunLeCode into eax and call it (i could do a relative call too but i don't wanna lol my math was off for all of the relative shits the first time)
        0xb8, RLE_PTR_BYTES[0], RLE_PTR_BYTES[1], RLE_PTR_BYTES[2], RLE_PTR_BYTES[3],   //mov eax, imm32 (pointer to RunLeCode)
        0xff, 0xd0,                                                                     //call eax

        //store the return value of RunLeCode into edi and pop eax for the add calculation
        //0x89, 0xc7,                                                                                    //mov edi, eax
        //0x58,                                                                                          //pop eax
        //0x01, 0xbc, 0x86, 0x7c, 0x01, 0x00, 0x00,                                                      //add DWORD PTR [esi+eax*4+0x17C],edi

        //nevermind i want to store the return value of RunLeCode into ecx and pop eax for the add calculation
        0x89, 0xc1,                                                                     //mov ecx, eax
        0x58,                                                                           //pop eax
        0x01, 0x8c, 0x86, 0x7c, 0x01, 0x00, 0x00,                                       //add DWORD PTR [esi+eax*4+0x17C],ecx

        //0x5f,                                                                                          //pop edi

        0x5a,                                                                           //pop edx

        //0x59,                                                                                          //pop ecx

        0x5b,                                                                           //pop ebx

        //finally at the end jmp back to the next instruction in modifyBallCount
        0xE9, 0x00, 0x00, 0x00, 0x00      //jmp rel32 (we will write that in ourselves after we alloc)
    };*/

    DWORD RE_PTR = (DWORD)&RunEvent;
    BYTE* RE_PTR_BYTES = (BYTE*)&RE_PTR;

    DWORD RETURNVAL_PTR = (DWORD)&v8helper2::returnval;
    BYTE* RETURNVAL_PTR_BYTES = (BYTE*)&RETURNVAL_PTR;

    BYTE bytes[] = { //https://defuse.ca/online-x86-assembler.htm#disassembly
        0x52,                                                                           //push edx
        0x50,                                                                           //push eax

        //push arguments to the stack for RunEvent (since i made RunEvent use __stdcall)
        0x8d, 0x84, 0x86, 0x7c, 0x01, 0x00, 0x00,                                       //lea eax, [esi+eax*4+0x17C] //ball count ptr
        //0x56,                                                                           //oush esi (LogicMgr)
        //0x50,                                                                           //push eax (Ball Count Ptr)
        0x52,                                                                           //push edx (idk)
        0x51,                                                                           //push ecx (amount of balls to add)
        0xff, 0x74, 0x24, 0x10,                                                         //push DWORD PTR [esp+0x10]
        0x6a, V8_NOTIFY_RUN_MODIFYBALLPTR,                                              //push imm8 (id)
        //0x68, 0x8d, 0x63, 0x01, 0x00,                                                   //push 91021

        //move the address of RunLeCode into eax and call it (i could do a relative call too but i don't wanna lol my math was off for all of the relative shits the first time)
        0xb8, RE_PTR_BYTES[0], RE_PTR_BYTES[1], RE_PTR_BYTES[2], RE_PTR_BYTES[3],   //mov eax, imm32 (pointer to RunEvent)
        0xff, 0xd0,                                                                     //call eax (returns void, get return value through v8helper2::returnval)
        0x8b, 0x0d, RETURNVAL_PTR_BYTES[0], RETURNVAL_PTR_BYTES[1], RETURNVAL_PTR_BYTES[2], RETURNVAL_PTR_BYTES[3], //mov ecx, [v8helper2::returnval]
        //store the return value of RunLeCode into edi and pop eax for the add calculation
        //0x89, 0xc7,                                                                                    //mov edi, eax
        //0x58,                                                                                          //pop eax
        //0x01, 0xbc, 0x86, 0x7c, 0x01, 0x00, 0x00,                                                      //add DWORD PTR [esi+eax*4+0x17C],edi

        //nevermind i want to store the return value of RunLeCode into ecx and pop eax for the add calculation
        //0x89, 0xc1,                                                                     //mov ecx, eax
        0x58,                                                                           //pop eax
        0x01, 0x8c, 0x86, 0x7c, 0x01, 0x00, 0x00,                                       //add DWORD PTR [esi+eax*4+0x17C],ecx

        0x5a,                                                                           //pop edx

        //finally at the end jmp back to the next instruction in modifyBallCount
        0xE9, 0x00, 0x00, 0x00, 0x00      //jmp rel32 (we will write that in ourselves after we alloc)
    };
    /*

    //DWORD pid = GetCurrentProcessId();
    //well since we are in the process, do i even need to use VirtualAllocEx?
    //void* alloc = VirtualAllocEx(GetCurrentProcess(), NULL, sizeof(bytes), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); //kernel32 we good
    void* alloc = VirtualAlloc(NULL, sizeof(bytes), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!alloc) {
        MessageBoxA(NULL, "(OverwriteModifyBallPtr) VirtualAlloc failed!", "fuck. this might crash the process now", MB_OK);
        __debugbreak(); //KeBugCheck
    }
    print(alloc);
    memcpy(alloc, bytes, sizeof(bytes));
    DWORD* jmpRelBytes = (DWORD*)((DWORD)alloc + sizeof(bytes) - 4);

    //sizeof(bytes)-5 because that's where the jmp is...
    //oopsies apparently the jmp is not included? (that doesn't really make sense)
    //i guess even the jmp to alloc includes the jmp so i guess the jmp is actually included and i don't subtract 5 because eip has passed it
    *jmpRelBytes = (modifyBallCount + 15 + 7) - (DWORD(alloc) + sizeof(bytes)); //-5); //(DWORD)alloc - (modifyBallCount + 15 + 7);

    */

    void* alloc = WriteAllocatedBytes(bytes, sizeof(bytes), (modifyBallCount + 15 + 7));

    /*
    //so we need to write some shit at modifyBallCount + 15
    //wait do we need WriteProcessMemory() ? isn't this our process
    //let\'s just memcpy
    DWORD relativeAddr = (DWORD)alloc - (modifyBallCount + 15 + 5); //+5 for the jump instruction itself
    BYTE* dwordarr = (BYTE*)&relativeAddr;
    BYTE overwriteBytes[] = {
        0xE9, dwordarr[0], dwordarr[1], dwordarr[2], dwordarr[3], //jmp rel32 (we gotta calculate that)
        0xCC,                                                     //int3
        0xCC,                                                     //int3
    };
    //original size of the add instruction at modifyBallCount + 15 is 7 so let's put some int3 or something
    //add dword ptr ds:[esi+eax*4+0x17C], ecx
    //aw damn this failed because i forgot that executable pages aren't normally supposed to be written to and thus, don't have the write access flag or whatever
    DWORD oldprotectionidc;
    if (!VirtualProtect((void*)(modifyBallCount + 15), sizeof(overwriteBytes), PAGE_EXECUTE_READWRITE, &oldprotectionidc)) {
        OutputDebugStringA("that ol'e VirtualProtect failed");
        MessageBoxA(NULL, "(OverwriteModifyBallPtr) VirtualProtect failed!", "fuck. this might crash the process later", MB_OK);
        __debugbreak();
    }
    memcpy((void*)(modifyBallCount + 15), overwriteBytes, sizeof(overwriteBytes));
    */

    BYTE overwriteBytes[] = {
        0xE9, 0x00, 0x00, 0x00, 0x00, //jmp rel32 (we gotta calculate that)
        0xCC,                                                     //int3
        0xCC,                                                     //int3
    };

    WriteJump((ULONG_PTR)alloc, modifyBallCount + 15, overwriteBytes, sizeof(overwriteBytes));
}

void OverwriteWindowProcKeyDownSection(void* peggle) {
    constexpr DWORD offset = 0x00002936;
    DWORD keyDownSection = ULONG_PTR(peggle) + offset; //this isn't the start of this procedure, it's the instruction i want to change in it
    //we are gonna change this big instruction into a jmp like i did for modify ball count
    //DWORD E_PTR = (DWORD)&Eval;
    //BYTE* E_PTR_BYTES = (BYTE*)&E_PTR;

    DWORD RE_PTR = (DWORD)&RunEvent;
    BYTE* RE_PTR_BYTES = (BYTE*)&RE_PTR;

    ULONG_PTR GAKS_PTR = (ULONG_PTR)&GetAsyncKeyState;
    BYTE* GAKS_PTR_BYTES = (BYTE*)&GAKS_PTR;
    //GetAsyncKeyState(VK_CONTROL) & 0x8000

    /*BYTE destinationBytes[] = {
        0x50,                                       //push eax
        0x53,                                       //push ebx
        0x51,                                       //push ecx
        0x52,                                       //push edx

        //check if edi holds the virtual keycode for 'E'
        //0x83, 0xff, 0x45,                           //cmp edi, 0x45
        //haha oops the keycode is actually passed through [ebp+8] and stored in edi AFTER the place i insert this code at
        //we gotta dereference it on the stack lol
        
        0x83, 0x7d, 0x08, 0x45,                     //cmp dword ptr [ebp+8], 0x45
        0x75, 0x17,                                 //jne finish (rel8)

        //now lets check if control is being held by calling GetAsyncKeyState and passing VK_CONTROL (0x11)
        //the following 4 lines of asm should be equivalent to GetAsyncKeyState(VK_CONTROL) & 0x8000
        0xb8, GAKS_PTR_BYTES[0], GAKS_PTR_BYTES[1], GAKS_PTR_BYTES[2], GAKS_PTR_BYTES[3],   //mov eax, imm32 (pointer to GetAsyncKeyState)
        //GetAsyncKeyState uses the __stdcall calling convention so we push arguments to the stack and the function adjusts the stack (instead of us doing it like with __cdecl) (this time the calling conventions actually mean something since this is x86)
        0x6a, 0x11,                                                             //push 0x11 (VK_CONTROL)
        0xff, 0xd0,                                                             //call eax

        //efficient asm (the and instruction will set the ZF (zero) flag and jz/je will jump if it is 0)
        0x25, 0x00, 0x80, 0x00, 0x00,                                           //and eax, 0x8000
        0x74, 0x07,                                                             //je finish (rel8)

        0xb8, E_PTR_BYTES[0], E_PTR_BYTES[1], E_PTR_BYTES[2], E_PTR_BYTES[3],   //mov eax, imm32 (pointer to Eval)
        0xff, 0xd0,                                                             //call eax

        //finish:

        0x80, 0xbe, 0xea, 0x00, 0x00, 0x00, 0x00,                               //cmp byte ptr [esi+0xEA], 0 ;original instruction we overwrote lol
                                                                                
        0x5a,                                                                   //pop edx
        0x59,                                                                   //pop ecx
        0x5b,                                                                   //pop ebx
        0x58,                                                                   //pop eax
                                                                                
        0xe9, 0x00, 0x00, 0x00, 0x00                                            //jmp rel32                  ;we'll write this in after we alloc like earlier
    };*/
    BYTE destinationBytes[] = {
        0x50,                                       //push eax
        0x53,                                       //push ebx              ;yo who was gonna tell me ebx wasn't volatile bruh
        0x51,                                       //push ecx
        0x52,                                       //push edx

        0xbb, V8_NOTIFY_RUN_KEYDOWN, 0x00, 0x00, 0x00,               //mov ebx, 4 (key down event for when we call RunEvent)

        //check if edi holds the virtual keycode for 'E'
        //0x83, 0xff, 0x45,                           //cmp edi, 0x45
        //haha oops the keycode is actually passed through [ebp+8] and stored in edi AFTER the place i insert this code at
        //we gotta dereference it on the stack lol

        0x83, 0x7d, 0x08, 0x45,                     //cmp dword ptr [ebp+8], 0x45
        0x75, 0x15,                                 //jne RunEvent (rel8)

        //now lets check if control is being held by calling GetAsyncKeyState and passing VK_CONTROL (0x11)
        //the following 4 lines of asm should be equivalent to GetAsyncKeyState(VK_CONTROL) & 0x8000
        0xb8, GAKS_PTR_BYTES[0], GAKS_PTR_BYTES[1], GAKS_PTR_BYTES[2], GAKS_PTR_BYTES[3],   //mov eax, imm32 (pointer to GetAsyncKeyState)
        //GetAsyncKeyState uses the __stdcall calling convention so we push arguments to the stack and the function adjusts the stack (instead of us doing it like with __cdecl) (this time the calling conventions actually mean something since this is x86)
        0x6a, 0x11,                                                             //push 0x11 (VK_CONTROL)
        0xff, 0xd0,                                                             //call eax

        //efficient asm (the and instruction will set the ZF (zero) flag and jz/je will jump if it is 0)
        0x25, 0x00, 0x80, 0x00, 0x00,                                           //and eax, 0x8000
        0x74, 0x05,                                                             //je RunEvent (rel8)

        0xbb, V8_NOTIFY_EVAL, 0x00, 0x00, 0x00,                                 //mov ebx, 100 (eval event)

        //RunEvent:

        0xb8, RE_PTR_BYTES[0], RE_PTR_BYTES[1], RE_PTR_BYTES[2], RE_PTR_BYTES[3],   //mov eax, imm32 (pointer to RunEvent)
        0x6a, 0x00,                                                             //push 0 (g_edx)
        0x6a, 0x00,                                                             //push 0 (g_ecx)
        0xff, 0x75, 0x08,                                                       //push DWORD PTR [ebp+0x8] (the key that was pressed -> g_eax)
        0x53,                                                                   //push ebx (id)
        0xff, 0xd0,                                                             //call eax (RunEvent)

        //finish:

        0x80, 0xbe, 0xea, 0x00, 0x00, 0x00, 0x00,                               //cmp byte ptr [esi+0xEA], 0 ;original instruction we overwrote lol

        0x5a,                                                                   //pop edx
        0x59,                                                                   //pop ecx
        0x5b,                                                                   //pop ebx
        0x58,                                                                   //pop eax

        0xe9, 0x00, 0x00, 0x00, 0x00                                            //jmp rel32                  ;we'll write this in after we alloc like earlier
    };
    void* alloc = WriteAllocatedBytes(destinationBytes, sizeof(destinationBytes), keyDownSection + 7);

    BYTE overwriteBytes[] = {
        0xE9, 0x00, 0x00, 0x00, 0x00, //jmp rel32 (we gotta calculate that)
        0xCC,                                                     //int3
        0xCC,                                                     //int3
    };

    WriteJump((ULONG_PTR)alloc, keyDownSection, overwriteBytes, sizeof(overwriteBytes));
}

void OverwritePegHitSection(void* peggle) {
    constexpr DWORD offset = 0x0006F4B5;
    DWORD pegHitSection = ULONG_PTR(peggle) + offset; //this isn't the start of this procedure, it's the instruction i want to change in it
    //we are gonna change this big instruction into a jmp like i did for modify ball count
    //DWORD E_PTR = (DWORD)&Eval;
    //BYTE* E_PTR_BYTES = (BYTE*)&E_PTR;

    DWORD RE_PTR = (DWORD)&RunEvent;
    BYTE* RE_PTR_BYTES = (BYTE*)&RE_PTR;

    BYTE destinationBytes[] = {
        0x50,                                                                       //push eax
        0x51,                                                                       //push ecx
        0x52,                                                                       //push edx

        0x31, 0xc9,                                                                 //xor ecx, ecx
        0x8a, 0x4d, 0x10,                                                           //mov cl, BYTE PTR [ebp+0x10] (third parameter of PegHit)

        0xb8, RE_PTR_BYTES[0], RE_PTR_BYTES[1], RE_PTR_BYTES[2], RE_PTR_BYTES[3],   //mov eax, imm32 (pointer to RunEvent)
        //0xff, 0x75, 0x10,                                                           //push DWORD PTR [ebp+0x10]
        0x51,                                                                       //push ecx (third param of PegHit -> g_edx)
        0x57,                                                                       //push edi (second param of PegHit -> g_ecx)
        0x53,                                                                       //push ebx (first param of PegHit -> g_eax)
        0x6a, V8_NOTIFY_RUN_PEGHIT,                                                 //push 0x2 (peghit event for RunEvent);
        0xff, 0xd0,                                                                 //call eax (RunEvent)

        0x5a,                                                                       //pop edx
        0x59,                                                                       //pop ecx
        0x58,                                                                       //pop eax

        0x8b, 0x87, 0xd0, 0x00, 0x00, 0x00,                                         //mov eax, DWORD PTR [edi+0xD0]         ;the original instruction we overwrote lol

        0xe9, 0x00, 0x00, 0x00, 0x00                                                //jmp rel32                             ;we'll write this in after we alloc like earlier
    };
    void* alloc = WriteAllocatedBytes(destinationBytes, sizeof(destinationBytes), pegHitSection + 6);

    BYTE overwriteBytes[] = { //only 6 bytes this time because the instruction im overwriting is only 6 bytes long!
        0xE9, 0x00, 0x00, 0x00, 0x00, //jmp rel32 (we gotta calculate that)
        0xCC,                         //int3
    };

    WriteJump((ULONG_PTR)alloc, pegHitSection, overwriteBytes, sizeof(overwriteBytes));
}

//the reason i call V8_NOTIFY_FILECHANGE when the level is loaded is so that it rereads the js file and initializes the globals again (because if i didn't, the board and logicmgr global js objects would point to garbage!)
void OverwriteLevelLoadEnd(void* peggle) {
    constexpr DWORD offset = 0x0002AACF;
    DWORD levelLoadRet = ULONG_PTR(peggle) + offset; //this isn't the start of this procedure, it's the instruction i want to change in it
    //we are gonna change this big instruction into a jmp like i did for modify ball count
    //DWORD E_PTR = (DWORD)&Eval;
    //BYTE* E_PTR_BYTES = (BYTE*)&E_PTR;

    DWORD RE_PTR = (DWORD)&RunEvent;
    BYTE* RE_PTR_BYTES = (BYTE*)&RE_PTR;

    BYTE destinationBytes[] = {
        0x50,                                                                       //push eax
        0xb8, RE_PTR_BYTES[0], RE_PTR_BYTES[1], RE_PTR_BYTES[2], RE_PTR_BYTES[3],   //mov eax, (RunEvent)
        0x6a, 0x00,                                                                 //push 0 (g_edx)
        0x6a, 0x00,                                                                 //push 0 (g_ecx)
        0x6a, 0x00,                                                                 //push 0 (g_eax)
        0x6a, V8_NOTIFY_FILECHANGE,                                                 //push id
        0xff, 0xd0,                                                                 //call eax
        0x58,                                                                       //pop eax
        0xc2, 0x04, 0x00,                                                           //ret 4                 ;original instruction we overwrote
    };
    void* alloc = WriteAllocatedBytes(destinationBytes, sizeof(destinationBytes), NULL, false);

    BYTE overwriteBytes[] = { //honestly kinda dangerous to put a jump here since this is the end of the function (and the return instruction i overwrite is only 3 bytes)
        0xE9, 0x00, 0x00, 0x00, 0x00, //jmp rel32 (we gotta calculate that)
        0x90,                         //nop
    };
    //__debugbreak();
    WriteJump((ULONG_PTR)alloc, levelLoadRet, overwriteBytes, sizeof(overwriteBytes));
}

//APIENTRY -> WINAPI -> __stdcall pops own arguments from stack!
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    //https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices

    //DebugBreak();
    //__asm {
    //    INT 3
    //}
    OutputDebugStringA("DllMain called!");
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        OutputDebugStringA("dll starting...!");
        constexpr size_t bsize = 6 + 10; //mango + " " is 6, 10 is the length in characters of 2**32
        char buffer[bsize];
        snprintf(buffer, bsize, "mango %u", ul_reason_for_call);
        OutputDebugStringA("snprintf");
        if (AllocConsole()) {
            freopen("CONIN$", "r", stdin);
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
        }
        g_peggle = FindPeggleBase();
        if (!g_peggle) {
            MessageBoxA(NULL, "failed to find the peggle base (i mean it should just be at 0x00400000 right?)", "we'll just assume then", MB_OK | MB_SYSTEMMODAL);
            g_peggle = (void*)0x00400000; //i mean this is peggle's preferred base so i could be right?
        }
        //g_peggle_internal_activate_ability = (void*)(ULONG_PTR(g_peggle) + 0x0006edf0);
        MessageBoxA(NULL, (const char*)buffer, "phonk", MB_OK | MB_SYSTEMMODAL);
        OverwriteModifyBallPtr(g_peggle);
        OverwriteWindowProcKeyDownSection(g_peggle);
        OverwritePegHitSection(g_peggle);
        OverwriteLevelLoadEnd(g_peggle);

        //char exe[MAX_PATH]; GetModuleFileNameA(hModule, exe, MAX_PATH); //nah no way i wrote NULL instead of THIS module (it was using popcapgame1???)
        //print(exe);
        //nah im moving all this to the top of RunLeCode (and making the exe path automatic in the constructor);
        //since i use NtCreateThreadEx, dll main is run on a new thread and v8 doesn't like that!

        //notifyEvent = CreateEvent(NULL, FALSE, FALSE, L"NOTIFY");

        //virtualeight = new v8helper(exe);
        g_thismodule = hModule;
        print("this " << g_thismodule);

        //v8helper2::SetupLoop(NULL);
        
        //ok so it looks like you can't call GetOpenFileName in DllMain because it just gives you an error and i accidently made an infinite loop because it wasn't working lol
        //if (g_filename[0] == '\0') {
        //    while (g_filename[0] == '\0') {
        //        ChooseLeScript();
        //    }
        //    FileWatcher::StartThread(g_filename);
        //}


        //DANGEROUS!
        //v8helper2::thread = CreateThread(NULL, 0, v8helper2::SetupLoop, NULL, 0, &v8helper2::threadID);
        //if (!v8helper2::thread) {
        //    MessageBoxA(NULL, "oouuuuhh something went wrong creating the v8 thread", "yeah this multithreading stuff is not my cup of tea", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
        //    __debugbreak();
        //}

        //yeah scratch all that
        //all initialization will happen the first time we call RunEvent!
        //(besides this next call)
        v8helper2::CreateEvents();
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        //delete virtualeight;
        v8helper2::Dispose();
    }

    //switch (ul_reason_for_call)
    //{
    //case DLL_PROCESS_ATTACH:
    //
    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    //case DLL_PROCESS_DETACH:
    //    break;
    //}
    return TRUE;
}