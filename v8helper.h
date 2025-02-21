#pragma once
class v8helper
{
public:
    std::unique_ptr<v8::Platform> platform;
    v8::Isolate::CreateParams create_params;
    v8::Isolate* isolate;

    //v8::Isolate::Scope isolate_scope; //erm you can't assign Isolate::Scope objects to each other...

    v8helper(const char*);
    ~v8helper();

    void RunLeScript(const wchar_t* filename, v8::Local<v8::ObjectTemplate> initglobalsfunc(v8::Isolate*));
    void Eval(const uint16_t* string, v8::Local<v8::ObjectTemplate> initglobalsfunc(v8::Isolate*), bool prettyprint);
};

const char* Highlight(v8::Isolate* isolate, HANDLE console, const v8::Local<v8::Value>& value);