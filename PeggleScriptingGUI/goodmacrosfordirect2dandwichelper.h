#pragma once

#include <comdef.h> //_com_error

#define SusIfFailed(x,y) if(FAILED(x)) {MessageBoxA(NULL, (std::string("[")+std::to_string(x)+"]"+y).c_str(), _bstr_t(_com_error(GetLastError()).ErrorMessage()), MB_OK | MB_SYSTEMMODAL);return false;}
#define ContIfFailed(x,y) if(FAILED(x)) {MessageBoxA(NULL, (std::string("[")+std::to_string(x)+"]"+y).c_str(), _bstr_t(_com_error(GetLastError()).ErrorMessage()), MB_OK);}
#define RetIfFailed(x,y) if(FAILED(x)) {MessageBoxA(NULL, (std::string("[")+std::to_string(x)+"]"+y).c_str(), _bstr_t(_com_error(GetLastError()).ErrorMessage()), MB_OK | MB_SYSTEMMODAL);return;}
#define RetPrintIfFailed(x,y) if(FAILED(x)) { \
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); \
	SetConsoleTextAttribute(console, 4); \
	std::cout << "[" << x << "] " << y << " " << _bstr_t(_com_error(GetLastError()).ErrorMessage()) << "\x07" << std::endl; \
	SetConsoleTextAttribute(console, 7); \
	return; \
}
#define PrintIfFailed(x,y) if(FAILED(x)) { \
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); \
	SetConsoleTextAttribute(console, 4); \
	std::cout << "[" << x << "] " << y << " " << _bstr_t(_com_error(GetLastError()).ErrorMessage()) << "\x07" << std::endl; \
	SetConsoleTextAttribute(console, 7); \
}

#define ERRORMB(x, y) MessageBoxA(NULL, (std::string("[")+std::to_string(x)+"]"+y).c_str(), _bstr_t(_com_error(GetLastError()).ErrorMessage()), MB_OK | MB_SYSTEMMODAL)
//#define SusIfFailed(x,y) if(FAILED(x)) {ERRHELP((std::string("[")+std::to_string(x)+"]"+y).c_str(), GetLastError());return false;}
//#define ContIfFailed(x,y) if(FAILED(x)) {ERRHELP((std::string("[")+std::to_string(x)+"]"+y).c_str(), GetLastError());}
//#define RetIfFailed(x,y) if(FAILED(x)) {ERRHELP((std::string("[")+std::to_string(x)+"]"+y).c_str(), GetLastError());return;}
//#define ERRORMB(x, y) ERRHELP((std::string("[")+std::to_string(x)+"]"+y).c_str(), GetLastError())
#define SafeRelease(punk) if(punk != nullptr) punk->Release(); punk = nullptr