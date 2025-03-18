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
	extern HANDLE waitEvent;
	extern HANDLE finishEvent;
	void CreateEvents();

	inline void tellWaitingThreadsToWaitLonger() {
		SetEvent(v8helper2::waitEvent);
	}

	inline char* allocateStackStr(const v8::Local<v8::String>& jsstr) {
		using namespace v8;
		char* stackstr = (char*)_alloca(jsstr->Length() + 1); //this is awesome (don't let the haters tell you otherwise bruh)
		strcpy(stackstr, CStringFI(jsstr)); //almost didn't allocate space for the null terminator i would've had the worst bug ever
		return stackstr;
	}

	extern volatile DWORD notify_id;
	extern volatile DWORD returnval;
	BOOL Notify(DWORD id);

	DWORD WINAPI SetupLoop(LPVOID lpParam);

	void Dispose();
}
