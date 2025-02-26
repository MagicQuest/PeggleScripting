#include "PeggleUI.h"
#include "main.h"
#include "goodmacrosfordirect2dandwichelper.h"
//#include <comdef.h>
#include <tlhelp32.h>
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>

HANDLE hProcessFromHWND(HWND hwnd) { //snatched from the ol'e memoview.js
    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);
    if (!processID) {
        DWORD g = GetLastError();
        wprint(L"GetWindowThreadProcessId failed\x07 " << g << L" " << _com_error(g).ErrorMessage());
    }
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID); //OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, processID); //oh lol i forgot query!
    if (!hProcess) {
        DWORD g = GetLastError();
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "OpenProcess failed(what was it tho %u)\x07", g);
        print(buffer);
    }
    return hProcess;
}

void errMessage(const wchar_t* msg) {
    PlaySoundSpecial(std::string(__dirname) + "/psgui/tone.mp3", "tone", NULL);
    DWORD g = GetLastError();
    wprint(msg << L"\x07" << g << L" " << _com_error(g).ErrorMessage());
    MessageBox(NULL, msg, _com_error(g).ErrorMessage(), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
}

void* LocateAndGetProcAddressEx(HANDLE hProcess, const wchar_t* moduleName, const char* procName) {
    DWORD pid = GetProcessId(hProcess);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid); //the PeggleScripting.dll also uses this function to find the base of popcapgame1.exe
    if (snapshot == INVALID_HANDLE_VALUE) {
        //this.hookErrCheck("CreateToolhelp32Snapshot");
        errMessage(L"(LocateAndGetProcAddressEx) CreateToolhelp32Snapshot failed!");
        return nullptr;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(me32);
    if (!Module32First(snapshot, &me32)) {
        //this.hookErrCheck("Module32First failed!", snapshot);
        errMessage(L"(LocateAndGetProcAddressEx) Module32First failed!");
        return nullptr;
    }

    do {
        wprint("MODULE NAME:" << me32.szModule);
        wprint("executable    = " << me32.szExePath);
        wprint("process id    = " << me32.th32ProcessID);
        wprint("ref count (g) = " << me32.GlblcntUsage);
        wprint("ref count (p) = " << me32.ProccntUsage);
        wprint("base address  = " << me32.modBaseAddr);
        wprint("base size     = " << me32.modBaseSize);
        //if (me32.szModule.toLowerCase().includes(moduleName)) {
        //    //ejhll euah
        //    print(`${moduleName
        //} found!`); //now how do get proc address from another process (maybe i'll just copy the header and read through it)
        if (wcscmp(me32.szModule, moduleName) == 0) {
            wprint(moduleName << " FOUND!");
            break;
        }
    }while (Module32Next(snapshot, &me32)); //directly modifies me32!!!!!!!!!

    CloseHandle(snapshot);

    IMAGE_DOS_HEADER DOS{};
    IMAGE_NT_HEADERS32 NT{};

    SIZE_T bytesRead;

    if (!ReadProcessMemory(hProcess, me32.modBaseAddr, &DOS, sizeof(DOS), &bytesRead)) {
        //fucke
        __debugbreak();
    }
    if (!ReadProcessMemory(hProcess, me32.modBaseAddr + DOS.e_lfanew, &NT, sizeof(NT), &bytesRead)) {
        //fucke
        __debugbreak();
    }

    //yeah i think im gonna have to read the headers myself to find the function's address

    //print(NT.OptionalHeader.DataDirectory[0]);
    DWORD exportDirectoryRVA = NT.OptionalHeader.DataDirectory[0].VirtualAddress;
    BYTE* entireExportDirectory = new BYTE[NT.OptionalHeader.DataDirectory[0].Size];
    if (!ReadProcessMemory(hProcess, me32.modBaseAddr + exportDirectoryRVA, entireExportDirectory, NT.OptionalHeader.DataDirectory[0].Size, &bytesRead)) {
        __debugbreak();
    }

    IMAGE_EXPORT_DIRECTORY EXPORT_DIRECTORY{};

    if (!ReadProcessMemory(hProcess, me32.modBaseAddr + exportDirectoryRVA, &EXPORT_DIRECTORY, sizeof(IMAGE_EXPORT_DIRECTORY), &bytesRead)) {
        __debugbreak();
    }

    ULONG_PTR nameslistaddress = (ULONG_PTR)me32.modBaseAddr + EXPORT_DIRECTORY.AddressOfNames;
    //const namesarray = NewPtrArray("INT", EXPORT_DIRECTORY.NumberOfNames);
    //namesarray.setData(ReadProcessMemory(hProcess, nameslistaddress, namesarray.data.byteLength));
    INT* namesarray = new INT[EXPORT_DIRECTORY.NumberOfNames];
    if (!ReadProcessMemory(hProcess, (void*)nameslistaddress, namesarray, EXPORT_DIRECTORY.NumberOfNames*sizeof(INT), &bytesRead)) {
        __debugbreak();
    }
    print("namesarray read " << bytesRead << " shoudl be " << (EXPORT_DIRECTORY.NumberOfNames * sizeof(INT)));

    ULONG_PTR functionslistaddress = (ULONG_PTR)me32.modBaseAddr + EXPORT_DIRECTORY.AddressOfFunctions;
    INT* functionsarray = new INT[EXPORT_DIRECTORY.NumberOfFunctions];
    //const functionsarray = NewPtrArray("INT", EXPORT_DIRECTORY.NumberOfFunctions);
    //functionsarray.setData(ReadProcessMemory(hProcess, functionslistaddress, functionsarray.data.byteLength));
    // print(functionslistaddress, functionsarray);
    if (!ReadProcessMemory(hProcess, (void*)functionslistaddress, functionsarray, EXPORT_DIRECTORY.NumberOfFunctions*sizeof(INT), &bytesRead)) {
        __debugbreak();
    }
    print("functionsarray read " << bytesRead << " shoudl be " << (EXPORT_DIRECTORY.NumberOfFunctions * sizeof(INT)));

    ULONG_PTR ordinalslistaddress = (ULONG_PTR)me32.modBaseAddr + EXPORT_DIRECTORY.AddressOfNameOrdinals;
    WORD* ordinalsarray = new WORD[EXPORT_DIRECTORY.NumberOfNames]; //NewPtrArray("WORD", EXPORT_DIRECTORY.NumberOfNames); //SHORT
    //ordinalsarray.setData(ReadProcessMemory(hProcess, ordinalslistaddress, ordinalsarray.data.byteLength));
    if (!ReadProcessMemory(hProcess, (void*)ordinalslistaddress, ordinalsarray, EXPORT_DIRECTORY.NumberOfNames * sizeof(WORD), &bytesRead)) {
        __debugbreak();
    }
    print("ordinalsarray read " << bytesRead << " shoudl be " << (EXPORT_DIRECTORY.NumberOfNames * sizeof(WORD)));

    //print(nameslist, namesarray);
    DWORD procOrdinal = -1;
    //oh boy this is getting harder to understand since i have to ReadProcessMemory every time :| (but i COULD just read the ENTIRE module)
    //my boy i have no idea what just happened but this loop almost crashed my computer
    //it sucked all my memory up dawg what did i just write?????
    for (size_t i = 0; i < EXPORT_DIRECTORY.NumberOfNames; i++) {
        ULONG_PTR nameptr = (ULONG_PTR)me32.modBaseAddr + namesarray[i]; //the actual address of the name in memory (but we don't know how long it is)
        //ummm idk how big it's gonna be
        //is there a VirtualStrlenEx ?
        //ok i'll just read the ENTIRE export directory and then do some offset magic on that
        ULONG_PTR offsetMagic = (nameptr - (ULONG_PTR)me32.modBaseAddr) - (exportDirectoryRVA);
        //print(nameptr, offsetMagic);
        //__debugbreak();
        //let str = "";                       //bigger than 0 here because i said so
        //std::string str = "";
        //for (size_t j = offsetMagic; entireExportDirectory[j] > 0; j++) { //im not sure if i've seen a for loop that does something like this before
        //    str += (char*)(entireExportDirectory[j]);
        //}
        //wchar_t* nigga = (wchar_t*)_alloca((str.length()+1)*2); //oooooo scary _alloca (since i started learning asm i wondered why there was no way to allocate stack space)
        //ZeroMemory(nigga, (str.length() + 1) * 2);
        //mbstowcs(nigga, str.c_str(), (str.length() + 1) * 2);
        //print(str, i, ordinalsarray[i]);
        //if (wcscmp(nigga, procName)) {
        //    procOrdinal = ordinalsarray[i];
        //    // procindex = i; //wait no i need the ordinal not i!
        //    
        //    break;
        //}
        //print(nameptr << " " << offsetMagic << " " << &entireExportDirectory);
        std::string str = "";
        for (int j = offsetMagic; entireExportDirectory[j] > 0; j++) {
            //print("j " << j);
            str += (char)(entireExportDirectory[j]);
        }
        print(str << " " << i << " " << ordinalsarray[i]);
        if (str == procName) {
            procOrdinal = ordinalsarray[i];
            break;
        }
    }

    if (procOrdinal != -1) {
        print(procOrdinal << " " << functionsarray[procOrdinal]);
        ULONG_PTR fapO = functionsarray[procOrdinal];
        delete[] entireExportDirectory;
        delete[] namesarray;
        delete[] functionsarray;
        delete[] ordinalsarray;
        return (void*)((ULONG_PTR)me32.modBaseAddr + fapO);
    }

    delete[] entireExportDirectory;
    delete[] namesarray;
    delete[] functionsarray;
    delete[] ordinalsarray;
}

double lerp(double a, double b, double t) {
    return t * (b - a) + a; //ima geinus
}

#pragma region Ball

Ball::Ball(float _x, float _y, float _width, float _height, const wchar_t* path, float opacity) : Controls::Bitmap(_x, _y, _width, _height, path, opacity) {
    this->intendedX = _x;
    this->intendedY = _y;
    this->velocity = D2D1::Point2F(10, 0);
    this->bounced = false;
}

void Ball::tick(float ratioX, float ratioY) {
    this->velocity.y += 1;
    this->intendedX += this->velocity.x;
    this->intendedY += this->velocity.y;
    this->x = ratioX * this->intendedX;
    this->y = ratioY * this->intendedY;
}

void Ball::bounce() {
    this->bounced = true;
    this->velocity.y *= -.7;
    PlaySoundSpecial(std::string(__dirname) + "/psgui/peghit_low.mp3", "peghit", NULL);
}

#pragma endregion

#pragma region PeggleUI

PeggleUI::PeggleUI(HWND _window, float width, float height) : hwnd(_window), Parent(0, 0, width, height, std::vector<Base*>(), Controls::Helper::Hittest(NULL, NULL)) {
    using namespace Controls;
    wchar_t dir[MAX_PATH]; GetCurrentDirectory(MAX_PATH, dir);

    this->sunBmp = new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/SunGLow.png").c_str(), 1.0);
    this->shadowBmp = new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/Shadow.png").c_str(), 0.0);
    this->peggleBmp = new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/peggleloader.png").c_str(), 1.0);

    this->backgroundimgs.push_back(new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/LoaderBack.png").c_str(), 1.0));
    this->backgroundimgs.push_back(this->sunBmp);
    this->backgroundimgs.push_back(new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/LoaderHill.png").c_str(), 1.0));
    this->backgroundimgs.push_back(this->shadowBmp);
    this->backgroundimgs.push_back(this->peggleBmp);
    this->backgroundimgs.push_back(new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/screenframeleft.png").c_str(), 1.0));
    this->backgroundimgs.push_back(new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/screenframetop.png").c_str(), 1.0));
    this->backgroundimgs.push_back(new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/screenframeright.png").c_str(), 1.0));
    this->backgroundimgs.push_back(new Bitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/screenframebottom.png").c_str(), 1.0));

    IWICFormatConverter* wicConverter = wic->LoadBitmapFromFilename((std::wstring(dir) + L"/psgui/Loaderlights.png").c_str(), GUID_WICPixelFormat32bppPBGRA, 0);

    RetIfFailed(d2d->renderTarget->CreateBitmapFromWicBitmap(wicConverter, NULL, &this->loadingLights), "failed d2d->renderTarget->CreateBitmapFromWicBitmap");
    wicConverter->Release();

    this->ratioX = 1.0;
    this->ratioY = 1.0;

    Helper::Coords* modes = new Helper::Coords[2]{
        Helper::Coords(0.0f, 0.0f, 344.0f, 50.0f),
        Helper::Coords(0.0f, 50.0f, 344.0f, 50.0f),
    };
    GenericTiledBitmap* bitmap = new GenericTiledBitmap(0, 0, 0, 0, (std::wstring(dir) + L"/psgui/HookNOw.png").c_str(), 1.0, modes, 0);
    this->hookButton = new TiledBitmapButton(bitmap, [](Base* parent) {
        ((PeggleUI*)parent)->hook();
    }, this);
    this->appendChild(this->hookButton);
    this->resize(width, height);
}

PeggleUI::~PeggleUI() {
    //do the destructoring shit
    for (Controls::Bitmap* bitmap : this->backgroundimgs) {
        delete bitmap;
    }
    this->loadingLights->Release();
}

void PeggleUI::startLoading() {
    this->hookButton->visible = false;
    this->loading = 1;
}

void PeggleUI::setLoading(int v) {
    this->loading = v;
    d2d->paint();
}

void PeggleUI::stopLoading() {
    this->hookButton->visible = true;
    this->loading = 0;
}

void PeggleUI::hookErr(const wchar_t* msg, void* addressToWriteDllPath) {//, void* pathptr) {
    errMessage(msg);
    PlaySoundSpecial(std::string(__dirname) + "/psgui/sigh.mp3", "sigh", this->hwnd);
    if (addressToWriteDllPath != nullptr) {
        VirtualFreeEx(this->peggleProcess, addressToWriteDllPath, 0, MEM_FREE);
    }
    //if (pathptr) {
    //    delete[] pathptr;
    //}
    this->stopLoading();
}

void PeggleUI::hook() {
    DWORD exitCode = -1;
    this->startLoading();
    PlaySoundSpecial(std::string(__dirname) + "/psgui/timpaniroll.mp3", "timpaniroll", this->hwnd);
    std::vector<HWND> windows; //for some reason i called it titles on the js side
    EnumWindows([](HWND hwnd, LPARAM lp) -> int {
        std::vector<HWND>* windows = (std::vector<HWND>*)lp;

        wchar_t title[MAX_PATH];
        GetWindowText(hwnd, title, MAX_PATH);
        RECT rect; GetWindowRect(hwnd, &rect);
        RECT client; GetClientRect(hwnd, &client);
        if (wcscmp(title, L"Peggle Deluxe 1.01") == 0) {
            windows->push_back(hwnd);
        }

        return TRUE;
    }, (LPARAM) & windows);
    if (windows.size()) {
        print(windows.size() << " " << windows[0]);
        this->peggleProcess = hProcessFromHWND(windows[0]);
        void* proc_LoadLibraryA = LocateAndGetProcAddressEx(this->peggleProcess, L"KERNEL32.DLL", "LoadLibraryA");
        print("LLA: " << proc_LoadLibraryA);
        if (proc_LoadLibraryA == nullptr) {
            return;
        }
        std::string dllpath = std::string(__dirname) + "\\PeggleScripting.dll";
        void* addressToWriteDllPath = VirtualAllocEx(this->peggleProcess, NULL, dllpath.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (addressToWriteDllPath == nullptr) {
            this->hookErr(L"VirtualAllocEx failed! (perhaps you don't have permission? idk)", nullptr); //, nullptr);
            return;
        }

        this->setLoading(this->loading + 6);
        //void* pathptr = new char[dllpath]; //oh wait i don't have to do this because im actually using c++
        const char* pathptr = dllpath.c_str();
        print(addressToWriteDllPath << " " << pathptr);
        SIZE_T bytesWritten;
        BOOL success = WriteProcessMemory(this->peggleProcess, addressToWriteDllPath, pathptr, dllpath.length(), &bytesWritten);
        if (!success) {
            this->hookErr(L"WriteProcessMemory failed!", addressToWriteDllPath);
            return;
        }

        //delete[] pathptr;

        this->setLoading(this->loading + 6);

        //honestly i don't have to use NtCreateThreadEx i just wanted to do it lol
        HANDLE thread = CreateRemoteThreadEx(this->peggleProcess, NULL, 0, (LPTHREAD_START_ROUTINE)proc_LoadLibraryA, addressToWriteDllPath, 0, NULL, NULL);
        print(thread);
        if (!thread) {
            this->hookErr(L"CreateRemoteThreadEx failed!", addressToWriteDllPath);
            return;
        }

        this->setLoading(this->loading + 6);
        
        //const t = Date.now();
        unsigned long long t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        WaitForSingleObject(thread, INFINITE);

        //print("waited", Date.now()-t, "ms");
        unsigned long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        print("waited " << (now - t) << " ms");

        GetExitCodeThread(thread, &exitCode);
        print("exit code " << exitCode);
        
        CloseHandle(thread);

        success = VirtualFreeEx(this->peggleProcess, addressToWriteDllPath, 0, MEM_RELEASE); //you can't pass a size with MEM_RELEASE
        if (!success) {
            this->hookErr(L"VirtualFreeEx failed but it's not that big of a deal!", addressToWriteDllPath); //lol try again
        }
        this->setLoading(this->loading + 6);
    }
    else {
        this->hookErr(L"Couldn't find peggle window?!", nullptr); //, nullptr);
        return;
    }

    PlaySoundSpecial(std::string(__dirname) + "/psgui/AwardFanfareV2_applause.mp3", "affv2_a", this->hwnd);
    this->stopLoading();
    char buffer[100]; snprintf(buffer, sizeof(buffer), "it's probably working now (%u)", exitCode);
    MessageBoxA(this->hwnd, "Success!", buffer, MB_OK | MB_SYSTEMMODAL);
}

void PeggleUI::resize(LONG newwidth, LONG newheight) {
    this->width = (float)newwidth;
    this->height = (float)newheight;

    float ratioX = newwidth / 800.f;
    float ratioY = newheight / 600.f;

    this->backgroundimgs[0]->transform(0.f, 0.f, ratioX * 800.f, ratioY * 432.f);
    //this.backgroundimgs[1].transform(0, 0, 0, 0); //handled in the uhhh the uhhhh introLoop
    this->backgroundimgs[2]->transform(0.f, ratioY * (600.f - 316.f), ratioX * 800.f, ratioY * 316.f);
    //this.backgroundimgs[2].transform(ratioX*228, ratioY*445, ratioX*344, ratioY*50);
    this->hookButton->transform(ratioX * 228.f, ratioY * 445.f, ratioX * 344.f, ratioY * 50.f);
    //this.backgroundimgs[3].transform(ratioX*163, ratioY*286, ratioX*460, ratioY*236);
    this->shadowBmp->transform(ratioX * 163.f, ratioY * 286.f, ratioX * 460.f, ratioY * 236.f);
    //this.backgroundimgs[4].transform(ratioX*170, ratioY*139, ratioX*426, ratioY*212);
    this->peggleBmp->transform(ratioX * 170.f, ratioY * 139.f, ratioX * 426.f, ratioY * 212.f);

    this->backgroundimgs[5]->transform(0.f, 0.f, ratioX * 50.f, ratioY * 600.f); //left
    this->backgroundimgs[6]->transform(ratioX * 50.f, 0.f, ratioX * 700.f, ratioY * 33.f); //top
    this->backgroundimgs[7]->transform(ratioX * (800.f - 50.f), 0.f, ratioX * 50.f, ratioY * 600.f); //right
    this->backgroundimgs[8]->transform(ratioX * 50.f, ratioY * (600.f - 34.f), ratioX * 700.f, ratioY * 34.f); //bottom

    this->ratioX = ratioX;
    this->ratioY = ratioY;
}

void PeggleUI::redraw() {
    for (Controls::Bitmap* bitmap : this->backgroundimgs) {
        bitmap->redraw();
    }
    this->hookButton->redraw();
    if (this->loading) {
        //float ratioX = this->width / 800.f;
        //float ratioY = this->height / 600.f;

        float almostx = this->ratioX * 224.f;
        float y = this->ratioY * 445.f;
        const float w = this->ratioX * 18.f;
        float h = y + (this->ratioY * 49.f);

        for (size_t i = 0; i < this->loading; i++) {
            const int index = PeggleUI::loadingIndices[i];
            const Slice sourceRect = PeggleUI::loadingSlices[index];
            //print(almostx, w, almostx+w);
            //print(i, index, sourceRect);
            d2d->renderTarget->DrawBitmap(this->loadingLights, D2D1::RectF(almostx, y, almostx + w, h), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(sourceRect.x, sourceRect.y, sourceRect.x + sourceRect.width, sourceRect.y + sourceRect.height));
            almostx += (this->ratioX * sourceRect.kerningWidth); //(w*i); //noooo the spacing is different sometimes...
            if (sourceRect.kerningHeight) {
                y += sourceRect.kerningHeight * this->ratioY;
                h += sourceRect.kerningHeight * this->ratioY;
            }
        }
    }
    if (this->postIntroBall != nullptr) {
        this->postIntroBall->redraw();
    }
}

void PeggleUI::windowResized(LONG newwidth, LONG newheight) {
    this->resize(newwidth, newheight);
}

void PeggleUI::beginIntro() {
    this->startLoading();
    this->timing = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    this->intro = true;
    this->introLoadingTiming = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    wchar_t dir[MAX_PATH]; GetCurrentDirectory(MAX_PATH, dir);
    this->postIntroBall = new Ball(100, -100, 80, 80, (std::wstring(dir) + L"/psgui/LogoBall.png").c_str(), 1.0);
}

float godlikebounceease(float t) { //oh shoot this stopped working because if t is 0 then this function returned infinity! (on the js side anyways)
    // const f = 2*(Math.ceil(t) || 1); //OR 1!
    // return Math.abs(Math.sin(t*Math.PI)/f); //https://www.desmos.com/calculator/jdklebdxwx

    //more gradual
    // const f = 0.5+((Math.ceil(t) || 1)/2);
    // return Math.abs(Math.sin(t*Math.PI)/f); //https://www.desmos.com/calculator/uczu1crnjd

    //good enough
    float f = (2.f * pow(((ceil(t) ? ceil(t) : 1.f) / 4.f), 2.f)) + 1.875f;
    return abs(sin(t * M_PI) / f); //https://www.desmos.com/calculator/lbsvrxuivb
}

void PeggleUI::onTimer() {
    using namespace Controls::Helper;

    //float ratioX = this->width / 800.f;
    //float ratioY = this->height / 600.f;

    if (this->intro) {
        unsigned long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if ((now - this->introLoadingTiming) > 500) {
            this->introLoadingTiming = now;
            this->setLoading(this->loading + 1);
            if (this->loading == 25) {
                this->intro = false;
                this->stopLoading();
            }
        }

        unsigned long long elapsed = (now - this->timing);

        float y = elapsed / 13000.f;

        if (y < 1.0f) {
            this->sunBmp->transform(this->ratioX * -30.f, this->ratioY * lerp(400.f, -250.f, y), this->ratioX * 800.f, this->ratioY * 800.f);
        }

        if (elapsed > 6000) {
            float shadowOpacity = (elapsed - 6000.f) / 6000.f;
            this->shadowBmp->opacity = shadowOpacity;
        }
    }
    else {
        this->sunBmp->rotation -= 1 + (((GetAsyncKeyState(VK_RBUTTON) & 0x8000) > 0) * 5);

        if (this->postIntroBall != nullptr) {
            this->postIntroBall->tick(this->ratioX, this->ratioY);

            if (!this->postIntroBall->bounced && withinBounds(Coords(this->peggleBmp->x, this->peggleBmp->y, this->peggleBmp->width, this->peggleBmp->height), POINT{ (LONG)this->postIntroBall->x, (LONG)this->postIntroBall->y + LONG(this->postIntroBall->height - 50) })) {
                this->postIntroBall->bounce();
                this->bounceTiming = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            }
            if (this->postIntroBall->intendedY > 600 + 80) {
                delete this->postIntroBall;
                this->postIntroBall = nullptr;
            }
        }
        if (this->bounceTiming) {
            float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - this->bounceTiming;
            float t = elapsed / 100.f;
            if (t > 50.0) {
                this->bounceTiming = 0;
            }
            else {
                float y = lerp(139, 145, godlikebounceease(t));
                this->peggleBmp->y = this->ratioY * y;
            }
        }
    }

    d2d->dirty = true;
}

#pragma endregion