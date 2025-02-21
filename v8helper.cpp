#include "pch.h"
#include "v8helper.h"

const char* Highlight(v8::Isolate* isolate, HANDLE console, const v8::Local<v8::Value>& value) {
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
                printf("    %s: ", (*v8::String::Utf8Value(isolate, name)));
                v8::Local<v8::Value> finalValue = value.As<v8::Object>()->Get(isolate->GetCurrentContext(), name).ToLocalChecked();
                printf("%s", Highlight(isolate, console, finalValue));
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

v8helper::v8helper(const char* modulename) {
    v8::V8::InitializeICUDefaultLocation(modulename);//argv[0]); //is this even doing anything ???
    v8::V8::InitializeExternalStartupData(modulename);//argv[0]);
    platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
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
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = v8::Isolate::New(create_params);

    //isolate->SetStackLimit(1 * 1024 * 1024);
}

v8helper::~v8helper() {
    this->isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::DisposePlatform();
    delete this->create_params.array_buffer_allocator;
}

void v8helper::RunLeScript(const wchar_t* filename, v8::Local<v8::ObjectTemplate> initglobalsfunc(v8::Isolate*)) {
    //if (!v8::Isolate::GetCurrent()) {
    //    print("executing on a different thread?");
    //}
    //
    //v8::Isolate::Scope isolate_scope(isolate);
    ////void *mango(int sigma);
    //
    //// Create a stack-allocated handle scope.
    //v8::HandleScope handle_scope(isolate);

    // v8::TryCatch trycatch(isolate);
     // Create a new context.
    //v8::Local<v8::ObjectTemplate> global = initglobalsfunc(isolate);
    //global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, Print));
    //global->Set(isolate, "printNoHighlight", v8::FunctionTemplate::New(isolate, RawPrint));
    //// Bind the global 'read' function to the C++ Read callback.
    //// Bind the 'version' function
    //global->Set(isolate, "version", v8::FunctionTemplate::New(isolate, Version));
    //global->Set(isolate, "OutputDebugString", v8::FunctionTemplate::New(isolate, OutputDebugStringWrapper));
    //v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global); //v8::ObjectTemplate::New(isolate)); //InitGlobals(isolate, nCmdList, nCmdShow);//argv[1]);//v8::Context::New(isolate, NULL, global);

    // Enter the context for compiling and running the hello world script.
    //v8::Context::Scope context_scope(context);

    std::wifstream file(filename);

    std::wstring shit{};

    if (file.is_open()) {
        shit = fs::LoadUtf8FileToString(filename);
    }
    else {
        file.close();
        print("yo we couldn't locate that file my boy");
        OutputDebugString(L"yo we couldn't locate that file my boy");
        return;
    }

    file.close();

    //v8::Local<v8::String> source = v8::String::NewFromTwoByte(isolate, (const uint16_t*)shit.c_str()).ToLocalChecked(); //, v8::NewStringType::kNormal, shit.length()).ToLocalChecked();//v8::String::NewFromUtf8(isolate, (const char*)shit, v8::NewStringType::kNormal, strlen(shit)).ToLocalChecked();
    this->Eval((const uint16_t*)shit.c_str(), initglobalsfunc, false);
}

void v8helper::Eval(const uint16_t* string, v8::Local<v8::ObjectTemplate> initglobalsfunc(v8::Isolate*), bool prettyprint) {
    if (!v8::Isolate::GetCurrent()) {
        print("executing on a different thread?");
    }

    v8::Isolate::Scope isolate_scope(isolate);
    //void *mango(int sigma);
    //isolate_scope = v8::Isolate::Scope(isolate);

    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);
    
    v8::Local<v8::ObjectTemplate> global = initglobalsfunc(isolate);

    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global); //v8::ObjectTemplate::New(isolate)); //InitGlobals(isolate, nCmdList, nCmdShow);//argv[1]);//v8::Context::New(isolate, NULL, global);

    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source = v8::String::NewFromTwoByte(isolate, string).ToLocalChecked();

    v8::Local<v8::Script> script;
    if (v8::Script::Compile(context, source).ToLocal(&script)) {
        // Run the script to get the result.
        v8::Local<v8::Value> result;
        if (script->Run(context).ToLocal(&result)) {
            // Convert the result to an UTF8 string and print it.
            //v8::String::Utf8Value utf8(isolate, result);
            //printf("%s\n", *utf8);
            using namespace v8;
            //if (!result->IsNullOrUndefined()) { //huh i wonder why i included this
            OutputDebugStringA(CStringFI(result));
            if (prettyprint) {
                printf("%s", Highlight(isolate, GetStdHandle(STD_OUTPUT_HANDLE), result));
                printf("%s", CStringFI(result));

                printf("\n");
                fflush(stdout);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
            //return IntegerFI(result);
            //printf("%s", Highlight(isolate, GetStdHandle(STD_OUTPUT_HANDLE), result));
            //printf("%s", CStringFI(result));
            //
            //printf("\n");
            //fflush(stdout);
            //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            //}
        }
    }
    //return;
}