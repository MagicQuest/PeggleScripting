#include "pch.h"
#include "v8helper2.h"
#include "Peggle.h"
#include "dllmain.h"

std::unique_ptr<v8::Platform> v8helper2::platform;
v8::Isolate::CreateParams v8helper2::create_params;
v8::Isolate* v8helper2::isolate;

HANDLE v8helper2::thread;
DWORD v8helper2::threadID;

HANDLE v8helper2::notifyEvent;
HANDLE v8helper2::finishEvent;
volatile DWORD v8helper2::notify_id;
volatile DWORD v8helper2::returnval;

const char* Highlight(v8::Isolate* isolate, HANDLE console, const v8::Local<v8::Value>& value, int depth) {
    using namespace v8;
    if (!value->IsString()) {
        if (value->IsNumber()) {//atoi(cstr)) {
            //color = "\033[35;0m";
            SetConsoleTextAttribute(console, 6);
        }
        else if (value->IsBoolean()) {
            SetConsoleTextAttribute(console, 5);
        }
        else if (value->IsFunction()) {
            SetConsoleTextAttribute(console, 7);
        }
        else if (value->IsNullOrUndefined()) {
            SetConsoleTextAttribute(console, 8);
        }
        else if (value->IsObject()) {
            SetConsoleTextAttribute(console, 8);
            //std::string obj = "{\n";
            printf("{\n");
            //HandleScope handle_scope(isolate); //i feel like this is the best place to put this (but im not even creating anything in here...)
            //nevermind i already define a handle_scope earlier (in Print)
            v8::Local<v8::Array> varNames = value.As<v8::Object>()->GetPropertyNames(isolate->GetCurrentContext()).ToLocalChecked();
            for (uint32_t j = 0; j < varNames->Length(); j++) {
                v8::Local<v8::Value> name = varNames->Get(isolate->GetCurrentContext(), j).ToLocalChecked()->ToString(isolate->GetCurrentContext()).ToLocalChecked();
                //obj += std::string("    ") + (*v8::String::Utf8Value(info.GetIsolate(), name)) + std::string(": ") + (*v8::String::Utf8Value(info.GetIsolate(), info[i].As<v8::Object>()->Get(info.GetIsolate()->GetCurrentContext(), name).ToLocalChecked())) + "\n";
                SetConsoleTextAttribute(console, 8);
                for (int i = 0; i < depth; i++) {
                    printf("    ");
                }
                printf("%s: ", (*v8::String::Utf8Value(isolate, name)));
                v8::Local<v8::Value> finalValue = value.As<v8::Object>()->Get(isolate->GetCurrentContext(), name).ToLocalChecked();
                printf("%s", Highlight(isolate, console, finalValue, depth+1));
                printf("%s\n", (*v8::String::Utf8Value(isolate, finalValue)));
            }
            //obj += "}";
            return "} ";//obj.c_str();
        }
    }
    else {
        SetConsoleTextAttribute(console, 3);
    }
    return "";
}

void RawPrint(const v8::FunctionCallbackInfo<v8::Value>& info) {
    for (int i = 0; i < info.Length(); i++) {
        v8::HandleScope handle_scope(info.GetIsolate());
        //if (i > 0) {
        //    print(" "); 
        //}
        v8::String::Utf8Value str(info.GetIsolate(), info[i]);
        const char* cstr = *str ? *str : "<string conversion failed>";
        printf("%s", cstr);
        printf(" ");
    }
    printf("\n");
    fflush(stdout);
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& info) {
    //bool first = true;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int i = 0; i < info.Length(); i++) {
        v8::HandleScope handle_scope(info.GetIsolate());
        //if (i > 0) { //you know what no i don't even have to do THIS
        //    printf(" ");
        //}
        //if (first) { //what the fuck was i on brooooooooooo WHAT THE FUCK
        //    first = false;
        //}
        //else {
        //    printf(" ");
        //}
        v8::String::Utf8Value str(info.GetIsolate(), info[i]);
        const char* cstr = *str ? *str : "<string conversion failed>";//ToCString(str);
        printf("%s", Highlight(info.GetIsolate(), console, info[i], 1)); //print syntax colors for the console (actually because if info[i] is an object it returns stuff)
        //const char* color = "";
        //if (!info[i]->IsString()) {
        //    if (info[i]->IsNumber()) {//atoi(cstr)) {
        //        //color = "\033[35;0m";
        //        SetConsoleTextAttribute(console, 6);
        //    }
        //    else if (info[i]->IsBoolean()) {
        //        SetConsoleTextAttribute(console, 5);
        //    }
        //    else if (info[i]->IsObject()) {
        //        SetConsoleTextAttribute(console, 8);
        //        //std::string obj = "{\n";
        //        printf("{\n");
        //        v8::Local<v8::Array> varNames = info[i].As<v8::Object>()->GetPropertyNames(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        //        for (uint32_t j = 0; j < varNames->Length(); j++) {
        //            v8::Local<v8::Value> name = varNames->Get(info.GetIsolate()->GetCurrentContext(), j).ToLocalChecked()->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        //            //obj += std::string("    ") + (*v8::String::Utf8Value(info.GetIsolate(), name)) + std::string(": ") + (*v8::String::Utf8Value(info.GetIsolate(), info[i].As<v8::Object>()->Get(info.GetIsolate()->GetCurrentContext(), name).ToLocalChecked())) + "\n";
        //            printf("    %s: ", (*v8::String::Utf8Value(info.GetIsolate(), name)));
        //            printf("%s\n", (*v8::String::Utf8Value(info.GetIsolate(), info[i].As<v8::Object>()->Get(info.GetIsolate()->GetCurrentContext(), name).ToLocalChecked())));
        //        }
        //        //obj += "}";
        //        cstr = "}";//obj.c_str();
        //    }
        //}
        //else {
        //    SetConsoleTextAttribute(console, 3);
        //}
        printf("%s", cstr);
        printf(" ");
        //printf("%s%s\033[0m",color,cstr);
    }
    printf("\n");
    fflush(stdout);
    SetConsoleTextAttribute(console, 7);
}

void Version(const v8::FunctionCallbackInfo<v8::Value>& info) {
    info.GetReturnValue().Set(
        v8::String::NewFromUtf8(info.GetIsolate(), v8::V8::GetVersion())
        .ToLocalChecked());
}

void OutputDebugStringWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();

    OutputDebugString(WStringFI(info[0]));
    if (!info[1]->BooleanValue(isolate)) {
        OutputDebugString(L"\n");
    }
}

void SleepWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    Sleep(IntegerFI(info[0]));
}

void __debugbreakWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
    __debugbreak();
}

void GetMousePosWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();

    POINT p{}; GetCursorPos(&p);

    Local<Object> mouse = Object::New(isolate);
    mouse->Set(isolate->GetCurrentContext(), LITERAL("x"), Number::New(isolate, p.x));
    mouse->Set(isolate->GetCurrentContext(), LITERAL("y"), Number::New(isolate, p.y));

    info.GetReturnValue().Set(mouse);
}

void GetGUIThreadInfoWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    GUITHREADINFO guithreadinfo{};
    guithreadinfo.cbSize = sizeof(GUITHREADINFO);

    BOOL res = GetGUIThreadInfo(IntegerFI(info[0]), &guithreadinfo);
    Local<Object> jsgti = Object::New(isolate);
    jsgti->Set(context, LITERAL("flags"), Number::New(isolate, guithreadinfo.flags));
    jsgti->Set(context, LITERAL("hwndActive"), Number::New(isolate, (LONG_PTR)guithreadinfo.hwndActive));
    jsgti->Set(context, LITERAL("hwndFocus"), Number::New(isolate, (LONG_PTR)guithreadinfo.hwndFocus));
    jsgti->Set(context, LITERAL("hwndCapture"), Number::New(isolate, (LONG_PTR)guithreadinfo.hwndCapture));
    jsgti->Set(context, LITERAL("hwndMenuOwner"), Number::New(isolate, (LONG_PTR)guithreadinfo.hwndMenuOwner));
    jsgti->Set(context, LITERAL("hwndMoveSize"), Number::New(isolate, (LONG_PTR)guithreadinfo.hwndMoveSize));
    jsgti->Set(context, LITERAL("hwndCaret"), Number::New(isolate, (LONG_PTR)guithreadinfo.hwndCaret));
    //jsgti->Set(context, LITERAL("rcCaret"), jsImpl::createWinRect(isolate, guithreadinfo.rcCaret)); //lol snatched from jbs3
    Local<Object> jsRect = Object::New(isolate);

    jsRect->Set(isolate->GetCurrentContext(), LITERAL("left"), Number::New(isolate, guithreadinfo.rcCaret.left));
    jsRect->Set(isolate->GetCurrentContext(), LITERAL("top"), Number::New(isolate, guithreadinfo.rcCaret.top));
    jsRect->Set(isolate->GetCurrentContext(), LITERAL("right"), Number::New(isolate, guithreadinfo.rcCaret.right));
    jsRect->Set(isolate->GetCurrentContext(), LITERAL("bottom"), Number::New(isolate, guithreadinfo.rcCaret.bottom));

    jsgti->Set(context, LITERAL("rcCaret"), jsRect);

    info.GetReturnValue().Set(jsgti);
}

void GetWindowRectWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    RECT r{ 0 }; GetWindowRect((HWND)IntegerFI(info[0]), &r);

    Local<Object> jsRect = Object::New(isolate);

    jsRect->Set(isolate->GetCurrentContext(), LITERAL("left"), Number::New(isolate, r.left));
    jsRect->Set(isolate->GetCurrentContext(), LITERAL("top"), Number::New(isolate, r.top));
    jsRect->Set(isolate->GetCurrentContext(), LITERAL("right"), Number::New(isolate, r.right));
    jsRect->Set(isolate->GetCurrentContext(), LITERAL("bottom"), Number::New(isolate, r.bottom));

    info.GetReturnValue().Set(jsRect);
}

void v8helper2::Notify(DWORD id) {
    //print("notify event and notifiy-id" << (ULONG_PTR) & notifyEvent << " " << (ULONG_PTR)&notify_id);
    //print(notifyEvent);
    //__debugbreak();
    //print(sizeof(unsigned long long)); //aw SHIT i thought on x86 this shit would only be 4 bytes!!!!!!
    DWORD last = InterlockedExchange((volatile ULONG_PTR*)&v8helper2::notify_id, id);
    //print("last " << last << " current " << id << " " << notifyEvent);
    BOOL result = SetEvent(v8helper2::notifyEvent);
    if (!result) {
        print("i did that wrong -> " << GetLastError());
    }
}

void MainLoop();

void v8helper2::CreateEvents() {
    v8helper2::notifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    v8helper2::finishEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

DWORD WINAPI v8helper2::SetupLoop(LPVOID lpParam) {
    //set up le v8 i guess lol
    //alright so what's gonna happen is
    //i want v8 to use the same script and context until we update the file
    //once we update the file we are gonna have to leave the scope to destroy the old context and stuff...
    //im gonna have to make 2 loops lol
    
    //v8 initialization happens here!
    char exe[MAX_PATH]; GetModuleFileNameA(g_thismodule, exe, MAX_PATH);
    print(exe);

    v8::V8::InitializeICUDefaultLocation(exe);//argv[0]); //is this even doing anything ???
    v8::V8::InitializeExternalStartupData(exe);//argv[0]);
    v8helper2::platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(v8helper2::platform.get());
    v8::V8::Initialize();

    //for some reason when i use the NtCreateThreadEx injection technique, v8 says i've hit the maximum call stack size and won't execute anything
    //apparently this is the solution: https://github.com/denoland/rusty_v8/issues/486#issuecomment-2473948349
    //so let's (wait do i use the apostrophe or not?) set the stack size of this dll to 2MB
    //and also set the stack limit for v8 to 2*1024*1024

    //oh shoot... i just realized that im doing all of this initialization on a separate thread...
    //maybe i should hold off on initializing v8 until i actually call modify ball ptr...
    //yeah nevermind scratch everything i wrote above here 
    //i think the problem is that i initialize v8 on a different thread than the one i run it on
    //the new thread created by NtCreateThreadEx that runs the initalization on is NOT the same one that runs the main logic of peggle
    //that's the difference between manually calling load library myself (with x32dbg) and injecting it

    // Create a new Isolate and make it the current one.
    v8helper2::create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8helper2::isolate = v8::Isolate::New(v8helper2::create_params);

    while (true) {
        MainLoop();
    }
}

namespace fs {
    size_t GetSizeOfFile(const std::wstring& path)
    {
        struct _stat fileinfo;
        _wstat(path.c_str(), &fileinfo);
        return fileinfo.st_size;
    }

    std::wstring LoadUtf8FileToString(const std::wstring& filename) //https://stackoverflow.com/a/4775542
    {
        std::wstring buffer;            // stores file contents
        FILE* f = _wfopen(filename.c_str(), L"rtS, ccs=UTF-8");

        // Failed to open file
        if (f == NULL)
        {
            // ...handle some error...
            return buffer;
        }

        size_t filesize = GetSizeOfFile(filename);

        // Read entire file contents in to memory
        if (filesize > 0)
        {
            buffer.resize(filesize);
            size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, f);
            buffer.resize(wchars_read);
            buffer.shrink_to_fit();
        }

        fclose(f);

        return buffer;
    }
}

void LogResult(v8::Isolate* isolate, const v8::Local<v8::Value>& result) {
    using namespace v8;
    OutputDebugStringA(CStringFI(result));

    printf("%s", Highlight(isolate, GetStdHandle(STD_OUTPUT_HANDLE), result, 1));
    printf("%s", CStringFI(result));

    printf("\n");
    fflush(stdout);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void MainLoop() {
    //this main loop sets up the context for the script to run in
    //if we get a file change notification, we will return from this function to leave this scope and "reset" v8
    //if we get a normal event notification (like PegHit) we run the script again (the event functions in Peggle.h check which event happened)
    //if we get an eval event notification, we run the text as a script (still in the same context lol because that's how it works when you hit Ctrl+E on any d2dcontrols thing for JBS3)

    using namespace v8;
    using v8helper2::isolate;

    v8::Isolate::Scope isolate_scope(isolate);
    //void *mango(int sigma);
    //isolate_scope = v8::Isolate::Scope(isolate);

    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::ObjectTemplate> global;

    //daddygibmemyglobals from dllmain.h!
    if (daddygibmemyglobals != nullptr) {
        global = daddygibmemyglobals(isolate, g_filename, 0);
    }
    else {
        global = ObjectTemplate::New(isolate);

        global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, Print));
        global->Set(isolate, "printNoHighlight", v8::FunctionTemplate::New(isolate, RawPrint));
        // Bind the global 'read' function to the C++ Read callback.
        // Bind the 'version' function
        global->Set(isolate, "version", v8::FunctionTemplate::New(isolate, Version));
        global->Set(isolate, "OutputDebugString", v8::FunctionTemplate::New(isolate, OutputDebugStringWrapper));
        global->Set(isolate, "Sleep", v8::FunctionTemplate::New(isolate, SleepWrapper));
        global->Set(isolate, "__debugbreak", v8::FunctionTemplate::New(isolate, __debugbreakWrapper));
        global->Set(isolate, "GetMousePos", v8::FunctionTemplate::New(isolate, GetMousePosWrapper));
        global->Set(isolate, "GetCursorPos", v8::FunctionTemplate::New(isolate, GetMousePosWrapper));
        global->Set(isolate, "GetGUIThreadInfo", v8::FunctionTemplate::New(isolate, GetGUIThreadInfoWrapper));
        global->Set(isolate, "GetWindowRect", v8::FunctionTemplate::New(isolate, GetWindowRectWrapper));

        //i think including these would cause random exceptions to happen but im not 100% sure...
        //global->Set(isolate, "ChooseExtendedGlobals", v8::FunctionTemplate::New(isolate, ChooseExtendedGlobalsWrapper));
        //global->Set(isolate, "ChooseLeScript", v8::FunctionTemplate::New(isolate, ChooseLeScriptWrapper));
    }

    Peggle::InitPeggleGlobals(global, isolate);

    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global); //v8::ObjectTemplate::New(isolate)); //InitGlobals(isolate, nCmdList, nCmdShow);//argv[1]);//v8::Context::New(isolate, NULL, global);

    v8::Context::Scope context_scope(context);

    wprint(L"[V8 Thread] reading the ole " << g_filename);

    //bruh i forgot to ACTUALLY read the file and i was trying to execute the path as javascript

    std::wifstream file(g_filename);

    std::wstring shit{};

    if (file.is_open()) {
        shit = fs::LoadUtf8FileToString(g_filename);
    }
    else {
        file.close();
        print("yo we couldn't locate that file my boy");
        OutputDebugString(L"yo we couldn't locate that file my boy");
        return;
    }

    file.close();

    v8::Local<v8::String> source = v8::String::NewFromTwoByte(isolate, (const uint16_t*)shit.c_str()).ToLocalChecked();

    v8::Local<v8::Script> script;
    if (v8::Script::Compile(context, source).ToLocal(&script)) {
        Peggle::init = true;
        v8::Local<v8::Value> result;
        if (script->Run(context).ToLocal(&result)) {
            LogResult(isolate, result);
        }
        Peggle::init = false;
    }
    //v8::Local<v8::Script> script;
    //if (v8::Script::Compile(context, source).ToLocal(&script)) {
        while (true) {
            DWORD result = WaitForSingleObject(v8helper2::notifyEvent, INFINITE);
            if (result == WAIT_OBJECT_0) {
                DWORD id = InterlockedCompareExchange((volatile ULONG_PTR*) &v8helper2::notify_id, 0, 0);
                print("[V8 Thread] notify_id " << id);

                v8::Local<v8::Script> scriptToRun;

                if (id == V8_NOTIFY_EVAL) {
                    SetForegroundWindow(GetConsoleWindow());
                    wchar_t scriptwstr[256];
                    std::wcout << L">>> ";
                    std::wcin.getline(scriptwstr, 256);
                    //virtualeight->Eval((const uint16_t*)scriptwstr, InitPeggleGlobals, true);
                    //v8::Local<v8::Script> eval;
                    //just 'avin' a little goto fun innit
                    Local<String> wswsvs = String::NewFromTwoByte(isolate, (const uint16_t*)scriptwstr).ToLocalChecked();

                    if (v8::Script::Compile(context, wswsvs).ToLocal(&scriptToRun)) {
                        goto executePeggle;
                    }
                    else {
                        print("compile failed just gonna return execution");
                        goto exit;
                    }
                }
                else if (id == V8_NOTIFY_FILECHANGE) {
                    break;
                }

                scriptToRun = script;
                executePeggle: {
                    //assuming RunLeCode...
                    if (scriptToRun.IsEmpty()) {
                        print("[V8 Thread] your code is dog shit and it didn't compile, resave it with something that works buddy \x07");
                        goto exit;
                    }
                    v8::Local<v8::Value> result;
                    if (scriptToRun->Run(context).ToLocal(&result)) {
                        LogResult(isolate, result);
                        /*
                        // Convert the result to an UTF8 string and print it.
                        //v8::String::Utf8Value utf8(isolate, result);
                        //printf("%s\n", *utf8);
                        //using namespace v8;
                        //if (!result->IsNullOrUndefined()) { //huh i wonder why i included this
                        OutputDebugStringA(CStringFI(result));
                        //if (prettyprint) {
                        printf("%s", Highlight(isolate, GetStdHandle(STD_OUTPUT_HANDLE), result, 1));
                        printf("%s", CStringFI(result));

                        printf("\n");
                        fflush(stdout);
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        //}
                        //return IntegerFI(result);
                        //printf("%s", Highlight(isolate, GetStdHandle(STD_OUTPUT_HANDLE), result));
                        //printf("%s", CStringFI(result));
                        //
                        //printf("\n");
                        //fflush(stdout);
                        //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        //}
                        */
                    }
                }

                exit:
                SetEvent(v8helper2::finishEvent);
            }
            else {
                print("[V8 Thread] some shit went wrong BUDDY " << result << " " << GetLastError());
                __debugbreak();
            }
        }
    //}
    return;
}

void v8helper2::Dispose() {
    v8helper2::isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::DisposePlatform();
    delete v8helper2::create_params.array_buffer_allocator;
}