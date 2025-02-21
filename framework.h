#pragma once

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <iostream>
#include <string>
#include <tlhelp32.h> 

#pragma comment(lib, "v8_monolith") //>ninja -C out/x64.release v8_monolith
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shlwapi.lib")

#define print(msg) std::cout << msg << std::endl
#define wprint(msg) std::wcout << msg << std::endl

#include "libplatform/libplatform.h"
#include "v8-context.h"
#include "v8-initialization.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include "v8-function.h"
#include "v8-container.h"
#include "v8-exception.h"

#define LITERAL(cstr) String::NewFromUtf8Literal(isolate, cstr)
#define CStringFI(e) *String::Utf8Value(isolate, e)
#define WStringFI(e) (const wchar_t*)*String::Value(isolate, e)
#define IntegerFI(e) e/*.As<Number>()*/->IntegerValue(isolate->GetCurrentContext()).FromJust()
#define FloatFI(e) e.As<Number>()->Value()

#define CHECKEXCEPTIONS(shit) if (shit.HasCaught()) {                                                                                                    \
                                  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);                                                                      \
                                  SetConsoleTextAttribute(console, 4);                                                                                   \
                                  print("[LINE " << shit.Message()->GetLineNumber(isolate->GetCurrentContext()).FromJust() << "] " << CStringFI(shit.Message()->Get()) << "\007");    \
                                  SetConsoleTextAttribute(console, 7);                                                                                   \
                              }