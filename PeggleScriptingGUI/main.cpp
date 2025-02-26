//ported from the jbs version
//i recreated this so good i reintroduced an error i had with the original one! (and i don't understand how i fixed it)

#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

//#include <windows.h>
//#include <iostream>
#include "main.h"
#include <thread>
#include "PeggleUI.h"
#include "goodmacrosfordirect2dandwichelper.h"


char __dirname[MAX_PATH];
PeggleUI* pui;
WICHelper* wic = nullptr;
Direct2D* d2d = nullptr;

//minimal rewrite from jbs
void PlaySoundSpecial(std::string path, const char* id, HWND window) {
	std::string* alias = new std::string(id); //this does NOT seem good but fuck it we ball (why did this actually work instead of strcpy)

	bool notify = (window != 0);

	std::string sound = path;
	//std::string type = sound.substr(sound.length() - 3, 3);  //ok idk why i added the type bruh that was CAP
	sound = "open \"" + sound + "\" type mpegvideo alias " + alias->c_str();
	print(sound);
	//char* alias = CStringFI(info[1]);
	//print(info[2]->IsNumber() << " " << IntegerFI(info[2]));
	print((std::string("play ") + alias->c_str() + (true ? " wait" : "") + (notify ? " notify" : "")).c_str());
	//had a bunch of stuff trying to get the short file name instead of using an alias but you have to enable short file names in the registry so that ain't happening (i could use SetShortFileName but idk if im doing all that)
	//char* soundcstr = new char[sound.size()];
	//strcpy_s(soundcstr, sound.size(), sound.c_str());
	std::string* soundcstr = new std::string(sound.c_str());

	std::thread f([=] {
		mciSendStringA(soundcstr->c_str(), NULL, 0, NULL);
		print((std::string("play ") + alias->c_str() + " wait" + (notify ? " notify" : "")).c_str());
		//damn mciSendStringA aliases only work if open is called on the same thread! (idk how this was missing in the last push but i noticed it the next day)
		int returned = mciSendStringA((std::string("play ") + alias->c_str() + " wait" + (notify ? " notify" : "")).c_str(), NULL, 0, window);

		//do i have to delete the persistent object since im done (how tf do i do that)
		//mciSendStringA((std::string("close ") + CStringFI(info[1])).c_str(), NULL, 0, (HWND)IntegerFI(info[2]));
		mciSendStringA((std::string("close ") + *alias).c_str(), NULL, 0, window);
		print("closed i think");
		alias->clear();
		delete/*[]*/ alias;
		soundcstr->clear();
		delete/*[]*/ soundcstr;
		//print(wait);
	});
	f.detach();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	if (msg == WM_CREATE) {
		wic = new WICHelper();
		wic->Init();
		//({ d2d, colorBrush, font, roundStrokeStyle } = D2DWindow.singleton.init(hwnd));
		d2d = new Direct2D();
		d2d->Init(hwnd, 0);
		// monofont = d2d.CreateFont("Consolas", 12);
		//print(d2d, colorBrush, font);
		pui = new PeggleUI(hwnd, 800, 600);
		d2d->panes.push_back(pui);
		//run intro
		pui->beginIntro();
		SetTimer(hwnd, 0, 16, NULL);
		PlaySoundSpecial(std::string(__dirname) + "/psgui/Koka_morning_finaledit.mp3", "mangophonksigma", hwnd);
	}
	else if (msg == WM_TIMER) {
		pui->onTimer();
		//if(pui.loading == 25) {
		//    //KillTimer(hwnd, 21);
		//}
	}
	else if (msg == WM_DESTROY) {
		PostQuitMessage(0);
	}

	if (d2d != nullptr) {
		LRESULT returnval = d2d->windowProc(hwnd, msg, wp, lp);
		if (returnval) {
			print("ret 1");
			return returnval;
		}
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, PWSTR nCmdList, int nCmdShow) {
	ContIfFailed(CoInitialize(NULL), "COINITIALIZE FAILED????");
	if (AllocConsole()) {
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}

	GetCurrentDirectoryA(sizeof(__dirname), __dirname);

	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	//wc.hCursor = (HCURSOR)LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"pegglegui";

	if (!RegisterClassEx(&wc)) {
		print("FUCJ");
		wchar_t buffer[100];
		DWORD error = GetLastError();
		wsprintf(buffer, L"we failed creating the window %u", error);
		MessageBox(NULL, buffer, L"ERM", MB_OK);
		return error;
	}

	CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"pegglegui", L"PeggleScripting GUI", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800+23, 600+43, NULL, NULL, hInst, NULL);

	MSG msg{};
	msg.message = ~WM_QUIT;
	while (msg.message != WM_QUIT) {
		if (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete wic;
	delete d2d;
}