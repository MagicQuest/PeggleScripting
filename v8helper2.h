#pragma once

#define V8_NOTIFY_RUN_MODIFYBALLPTR 1
#define V8_NOTIFY_RUN_PEGHIT 2
#define V8_NOTIFY_RUN_TIMER 3
#define V8_NOTIFY_RUN_KEYDOWN 4

#define V8_NOTIFY_EVAL 100
#define V8_NOTIFY_FILECHANGE 101

const char* Highlight(v8::Isolate* isolate, HANDLE console, const v8::Local<v8::Value>& value);

namespace v8helper2 {
	extern std::unique_ptr<v8::Platform> platform;
	extern v8::Isolate::CreateParams create_params;
	extern v8::Isolate* isolate;

	extern HANDLE thread;
	extern DWORD threadID;

	extern HANDLE notifyEvent;
	extern HANDLE finishEvent;
	void CreateEvents();

	extern volatile DWORD notify_id;
	extern volatile DWORD returnval;
	void Notify(DWORD id);

	DWORD WINAPI SetupLoop(LPVOID lpParam);

	void Dispose();
}
