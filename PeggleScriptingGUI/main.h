#pragma once
#include <iostream>
#include <windows.h>
#include "WICHelper.h"
#include "Direct2D.h"

using namespace std;
#define print(msg) cout << msg << endl
#define wprint(msg) wcout << msg << endl

extern char __dirname[MAX_PATH];
extern WICHelper* wic;
extern Direct2D* d2d;

void PlaySoundSpecial(std::string path, const char* id, HWND window);