// dllmain.cpp : Defines the entry point for the DLL application.

#ifndef WIN32
#define V8_ENABLE_SANDBOX
#define V8_COMPRESS_POINTERS
#endif
#define V8_31BIT_SMIS_ON_64BIT_ARCH
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "v8helper.h"
//#include "FileWatcher.h"

HMODULE g_thismodule;
void* g_peggle;

v8helper* virtualeight;

HMODULE extended_binary;
wchar_t g_filename[MAX_PATH]{};

typedef v8::Local<v8::ObjectTemplate>(*JBSInit)(v8::Isolate*, const wchar_t*, int);

JBSInit daddygibmemyglobals = nullptr;

DWORD g_eax;
DWORD g_ecx;
DWORD g_edx;
DWORD g_ballCount;

void* g_logicmgr; //RTTI says that this random important struct is called the LogicMgr
void* g_board;
void* g_ball;

//HANDLE g_watcherthread;
//HANDLE notifyEvent;

//typedef void(__thiscall *activateAbilitySignature)(void*, void*, DWORD, DWORD);
//activateAbilitySignature g_peggle_internal_activate_ability;
void* g_peggle_internal_activate_ability;

DWORD g_returnval;

void showOpenFilePicker(wchar_t* out_file_name, size_t length) {
    OPENFILENAME ofn{};
    wchar_t file_name[MAX_PATH]{};

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    wchar_t filter[MAX_PATH]{};

    GetOpenFileName(&ofn);
    //_alloca()

    if (+file_name[0]) {
        memcpy(out_file_name, file_name, length);
    }
}

//Highlight defined in v8helper.cpp

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
        printf("%s", Highlight(info.GetIsolate(), console, info[i])); //print syntax colors for the console (actually because if info[i] is an object it returns stuff)
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

void onBallCountModify(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    Local<Function> func = info[0].As<Function>();
    Local<Value> args[] = { Integer::New(isolate, g_eax), Integer::New(isolate, g_ecx), Integer::New(isolate, g_edx), Integer::New(isolate, g_ballCount) };
    TryCatch errhandler(isolate);
    MaybeLocal<Value> result = func->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), 4, args);
    CHECKEXCEPTIONS(errhandler);
    if (!result.IsEmpty()) {
        g_returnval = IntegerFI(result.ToLocalChecked());
    }
    else {
        g_returnval = g_ecx;
    }
}

//#define DLL_EXPORT extern "C" __declspec(dllexport)
//
//DLL_EXPORT void __stdcall ChooseLeScript();
//DLL_EXPORT void __stdcall ChooseExtendedGlobals();
//
//void ChooseLeScriptWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
//    ChooseLeScript();
//}
//
//void ChooseExtendedGlobalsWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
//    ChooseExtendedGlobals();
//}

void __debugbreakWrapper(const v8::FunctionCallbackInfo<v8::Value>& info) {
    __debugbreak();
}

//#define setPointerProperty(OT, name, offset) OT->SetAccessorProperty(LITERAL(name),                                                         \
//                                        FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                    \
//                                            Isolate* isolate = info.GetIsolate();                                                       \
//                                            info.GetReturnValue().Set(Number::New(isolate, *(DWORD*)(base + offset)));                  \
//                                        }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                \
//                                            Isolate* isolate = info.GetIsolate();                                                       \
//                                            DWORD* idk = (DWORD*)(base + offset);                                      \
//                                            *idk = IntegerFI(info[0]);                                                                  \
//                                        })                                                                                              \
//                                    );                                                                                                  \
//
//#define setFloatPointerProperty(OT, name, offset) OT->SetAccessorProperty(LITERAL(name),                                                         \
//                                        FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                    \
//                                            Isolate* isolate = info.GetIsolate();                                                       \
//                                            info.GetReturnValue().Set(Number::New(isolate, *(float*)(base + offset))); \
//                                        }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                \
//                                            Isolate* isolate = info.GetIsolate();                                                       \
//                                            float* idk = (float*)(base + offset);                                      \
//                                            *idk = FloatFI(info[0]);                                                                  \
//                                        })                                                                                              \
//                                    );                                                                                                \

#define setPointerProperty(OT, name, offset) OT->SetAccessorProperty(LITERAL(name),                                                                                     \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     info.GetReturnValue().Set(Number::New(isolate, *(DWORD*)(base + offset)));                                         \
                                                 }),                                                                                                                    \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     DWORD* prop = (DWORD*)(base + offset);                                                                              \
                                                     *prop = IntegerFI(info[0]);                                                                                         \
                                                 })                                                                                                                     \
                                             );

#define setFloatPointerProperty(OT, name, offset) OT->SetAccessorProperty(LITERAL(name),                                                                                     \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     info.GetReturnValue().Set(Number::New(isolate, *(float*)(base + offset)));                                         \
                                                 }),                                                                                                                    \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     float* prop = (float*)(base + offset);                                                                              \
                                                     *prop = FloatFI(info[0]);                                                                                         \
                                                 })                                                                                                                     \
                                             );

//#define PeggleProperty(name, offset) setPointerProperty(peggle, name, (ULONG_PTR)g_logicmgr, offset)
//#define LogicMgrProperty(name, offset) setPointerProperty(LogicMgr, name, offset)
//#define BoardProperty(name, offset) setPointerProperty(board, name, *(ULONG_PTR*)((ULONG_PTR)g_logicmgr+0x104), offset)
//#define BallProperty(name, offset) setPointerProperty(ball, name, (ULONG_PTR)(g_ball), offset)
//#define BallFloatProperty(name, offset) setFloatPointerProperty(ball, name, (ULONG_PTR)(g_ball), offset)
//#define BoardProperty(name, offset) setPointerProperty(board, name, offset)
//#define BallProperty(name, offset) setPointerProperty(ball, name, offset)
//#define BallFloatProperty(name, offset) setFloatPointerProperty(ball, name, offset)
#define Property(OT, name, offset) setPointerProperty(OT, name, offset)
#define FloatProperty(OT, name, offset) setFloatPointerProperty(OT, name, offset)

#define ConstProperty(OT, constname) OT->Set(isolate, #constname, Number::New(isolate, constname))

#define ABILITY_SUPERGUIDE 1
#define ABILITY_FLIPPERS 2
#define ABILITY_MULTIBALL 3
#define ABILITY_PYRAMID 4
#define ABILITY_SPACEBLAST 5
#define ABILITY_SPOOKYBALL 6
#define ABILITY_ZENSHOT 7
#define ABILITY_SHOTEXTENDER 8

//DWORD peggle_getAbilityOffset(DWORD ability) {
//    return ((2 * ability) * 4) + 0x1D4;
//}

void generic_set_dword(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    DWORD* idk = (DWORD*)(base + IntegerFI(info[0]));
    *idk = IntegerFI(info[1]);
}

void generic_get_dword(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    info.GetReturnValue().Set(Number::New(isolate, *(DWORD*)(base + IntegerFI(info[0]))));
}

void generic_set_float(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    float* idk = (float*)(base + IntegerFI(info[0]));
    *idk = FloatFI(info[1]);
}

void generic_get_float(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    info.GetReturnValue().Set(Number::New(isolate, *(float*)(base + IntegerFI(info[0]))));
}

void peggle_activate_ability(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    //uhh we need to call this function
    //uh oh how the fuck do i call a member function myself
    //well since we've got the option i might have to __asm this shit!

    constexpr DWORD offset = 0x0006EDF0;


    //i think there are 2 ways to call this function
    //if you know which ability you want, you pass it
    //if you don't, set ability to 0
    //the problem is i don't really know what you pass as the first 2 arguments
    //if you don't know the ability (meaning it's implied), the first 2 arguments seem to point to 2 different Ball objects (which is weird because why are there 2)

    //ok for some reason the first 2 parameters point to the same address so lets allocate that on le stack right here
    //yeah im not gonna lie idk if these were ever right? they still worked though so imma use them as default params
    DWORD unknown1 = 0x005F19B4; //these are the addresses of some function in a vtable!
    //DWORD unknown2 = 0x005F1FDC; //for some reason this one was different! (when i used warren and got uhh i forgot what exactly it gave me thhis)
    DWORD unknown2 = unknown1;

    //yeah so i asked chat and he said asm was the goat so...
    //DWORD unknown1 = IntegerFI(info[0]);

    //DWORD unknown2;// = IntegerFI(info[1]);

    if (info[0]->BooleanValue(isolate)) {
        unknown1 = IntegerFI(info[0]);
    }
    if (info[1]->BooleanValue(isolate)) {
        unknown2 = IntegerFI(info[1]);
    }

    DWORD ability = IntegerFI(info[2]);

    ULONG_PTR peggle_internal_activate_ability = (ULONG_PTR)g_peggle + offset;

    //bruh this actually worked but didn't at the same time
    __asm {
        //int 3
        push 1
        push ability
        lea eax, unknown2
        push eax
        lea eax, unknown1
        push eax
        mov ecx, g_logicmgr
        call peggle_internal_activate_ability
    }
    //g_peggle_internal_activate_ability(nullptr, nullptr, IntegerFI(info[0]), 1); //idk im assuming this shit...
}

/*void peggle_get_board(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    //there's a function that actually gets the board for you in the peggle binary but also g_logicmgr holds a pointer to a Board object at 0x104 bytes in

}*/

void peggle_something_about_shooting_the_ball(DWORD mouseX, DWORD mouseY, DWORD mouseButton = 1) { //ohhhh right click is -1 and middle mouse is 3
    //third parameter is usually one
    constexpr DWORD offset = 0x00002810;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        //this function is a method of the Board class and it's definitely related to shooting the ball in some way lol but the main important part is super long and i haven't been bothered to analyze it yet
        //yeah i think that this function is normally called when you interact with the main screen and it happens to call the actual ball shooting function in it (i think i still haven't analyzed that big ass function)
        //int 3
        mov eax, funcaddr
        push mouseButton
        push mouseY
        push mouseX
        mov ecx, g_board
        call eax
    }
}

void peggle_add_balls(DWORD amount, DWORD unknown, DWORD b_unknown2) { //b_unknown2 is some kinda boolean or something
    //unknown is usually 0
    //b_unknown2 is usually 1
    //i think

    constexpr DWORD offset = 0x0005D880;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        //this function is a method of the LogicMgr class and it directly changes the amount of balls you have
        //it's also the function i change when this Dll is loaded
        //if you call this function in a script it will infinitely loop so probably only eval this one
        //int 3
        mov eax, funcaddr
        push b_unknown2
        push unknown
        push amount
        mov ecx, g_logicmgr
        call eax
    }
}

const v8::Local<v8::ObjectTemplate>& GenericPtrObject(v8::Isolate* isolate, void* ptr) {
    using namespace v8;
    Local<ObjectTemplate> ptr_object = ObjectTemplate::New(isolate);
    ptr_object->Set(isolate, "set_dword", FunctionTemplate::New(isolate, generic_set_dword));
    ptr_object->Set(isolate, "get_dword", FunctionTemplate::New(isolate, generic_get_dword));
    ptr_object->Set(isolate, "location", Number::New(isolate, (ULONG_PTR)ptr));
    //ptr_object->SetAccessorProperty(LITERAL("location"), Local<FunctionTemplate>(), Local<FunctionTemplate>(), PropertyAttribute::ReadOnly);
    //ptr_object->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_logicmgr));
    //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);
    return ptr_object;
}

constexpr DWORD c_getAbilityOffset(DWORD ability) {
    return ((2 * ability) * 4) + 0x1D4;
}

v8::Local<v8::ObjectTemplate> InitPeggleGlobals(v8::Isolate* isolate) {
    using namespace v8;
    Local<ObjectTemplate> global;// = ObjectTemplate::New(isolate);
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

        //i think including these would cause random exceptions to happen but im not 100% sure...
        //global->Set(isolate, "ChooseExtendedGlobals", v8::FunctionTemplate::New(isolate, ChooseExtendedGlobalsWrapper));
        //global->Set(isolate, "ChooseLeScript", v8::FunctionTemplate::New(isolate, ChooseLeScriptWrapper));
    }
    global->Set(isolate, "onBallCountModify", FunctionTemplate::New(isolate, onBallCountModify));
    Local<ObjectTemplate> LogicMgr = GenericPtrObject(isolate, g_logicmgr); //ObjectTemplate::New(isolate);

    //peggle->SetAccessorProperty(LITERAL("balls"), 
    //    FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
    //        //getter
    //        Isolate* isolate = info.GetIsolate();
    //        info.GetReturnValue().Set(Number::New(isolate, *(DWORD*)((ULONG_PTR)g_structptr + 0x17C)));
    //    }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
    //        //setter
    //        Isolate* isolate = info.GetIsolate();
    //        DWORD* ballCount = (DWORD*)((ULONG_PTR)g_structptr + 0x17C);
    //        *ballCount = IntegerFI(info[0]);
    //    })
    //);
    //
    //peggle->SetAccessorProperty(LITERAL("specials"),
    //    FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
    //        //getter
    //        Isolate* isolate = info.GetIsolate();
    //        info.GetReturnValue().Set(Number::New(isolate, *(DWORD*)((ULONG_PTR)g_structptr + 0x200)));
    //    }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
    //        //setter
    //        Isolate* isolate = info.GetIsolate();
    //        DWORD* specials = (DWORD*)((ULONG_PTR)g_structptr + 0x200);
    //        *specials = IntegerFI(info[0]);
    //    })
    //);

    Property(LogicMgr, "balls", 0x17C);
    Property(LogicMgr, "superGuideCount", c_getAbilityOffset(1));
    //PeggleProperty("flipperCount", 0x1E4);
    //PeggleProperty("spookyCount", 0x204);
    //PeggleProperty("zenCount", 0x20C);
    Property(LogicMgr, "flipperCount", c_getAbilityOffset(2));
    Property(LogicMgr, "pyramidCount", c_getAbilityOffset(3));
    //PeggleProperty("spaceBlastCount", getAbilityOffset(4));
    Property(LogicMgr, "spookyCount", c_getAbilityOffset(6));
    Property(LogicMgr, "zenCount", c_getAbilityOffset(7));

    Property(LogicMgr, "ballsShot", 0x248);
    Property(LogicMgr, "freeBallsGivenByBucket", 0x254);
    Property(LogicMgr, "freeBallsGivenByPoints", 0x254);
    Property(LogicMgr, "score", 0x174);
    Property(LogicMgr, "time", 0x8); //idk what the purpose of this one is
    Property(LogicMgr, "specialsomething", 0x20);

    Property(LogicMgr, "currentAbility", 0x1C4);

    ConstProperty(global, ABILITY_SUPERGUIDE);
    ConstProperty(global, ABILITY_FLIPPERS);
    ConstProperty(global, ABILITY_MULTIBALL);
    ConstProperty(global, ABILITY_PYRAMID);
    ConstProperty(global, ABILITY_SPACEBLAST);
    ConstProperty(global, ABILITY_SPOOKYBALL);
    ConstProperty(global, ABILITY_ZENSHOT);
    ConstProperty(global, ABILITY_SHOTEXTENDER);

    LogicMgr->SetAccessorProperty(LITERAL("text"),
        FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            //getter
            Isolate* isolate = info.GetIsolate();
            Local<String> str;
            if (String::NewFromUtf8(isolate, (const char*)(ULONG_PTR(g_logicmgr) + 0x80)).ToLocal(&str)) {
                info.GetReturnValue().Set(str);
            }
            else {
                print("something went wrong reading peggle.text");
            }
        }), 
        FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            //setter
            Isolate* isolate = info.GetIsolate();
            char* text = (char*)((ULONG_PTR)g_logicmgr + 0x80);
            Local<String> str = info[0].As<String>();
            //memcpy(text, CStringFI(info[0]), str->Length());
            strcpy(text, CStringFI(str)); //also copies null terminator
        })
    );
    Property(LogicMgr, "textLength", 0x90);

    //LogicMgr->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_logicmgr));
    //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);
    //
    //LogicMgr->Set(isolate, "set_dword", FunctionTemplate::New(isolate, generic_set_dword));
    //LogicMgr->Set(isolate, "get_dword", FunctionTemplate::New(isolate, generic_get_dword));
    LogicMgr->Set(isolate, "activate_ability", FunctionTemplate::New(isolate, peggle_activate_ability));
    LogicMgr->Set(isolate, "shoot_ball_idk", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        peggle_something_about_shooting_the_ball(IntegerFI(info[0]), IntegerFI(info[1]), IntegerFI(info[2]));
    }));
    LogicMgr->Set(isolate, "add_balls", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        peggle_add_balls(IntegerFI(info[0]), IntegerFI(info[1]), IntegerFI(info[2]));
    }));
    //peggle->Set(isolate, "get_board", FunctionTemplate::New(isolate, peggle_get_board));

    /*peggle->SetAccessorProperty(LITERAL("windowTitle"),
        FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            info.GetReturnValue().Set(String::NewFromTwoByte(isolate, (const uint16_t*)GetWindowText()))
        }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {

        })
    );*/

    Local<ObjectTemplate> Board = GenericPtrObject(isolate, g_board);
    Property(Board, "ballHasBeenShot", 0x1A4); //idk about this one lol (seems very very likely tho)
    Property(Board, "viewX", 0x1A8);
    Property(Board, "viewY", 0x1AC);
    //Board->Set(isolate, "set_dword", FunctionTemplate::New(isolate, generic_set_dword));
    //Board->Set(isolate, "get_dword", FunctionTemplate::New(isolate, generic_get_dword));
    //Board->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_board));
    //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);

    if (g_ball != nullptr) {
        Local<ObjectTemplate> Ball = GenericPtrObject(isolate, g_ball); //ObjectTemplate::New(isolate);
        
        //setPointerProperty(ball, "x", g_ball, 0x134);
        //setPointerProperty(ball, "y", g_ball, 0x134);
        FloatProperty(Ball, "drawX", 0x134); //honestly i don't know yet lol
        FloatProperty(Ball, "drawY", 0x138); //yeah idk it seems like it might be where it draws it at?
        FloatProperty(Ball, "radius", 0x13C); //WHY THE FUCK IS THERE A RADIUS PROPERTY AND WHY DOES IT ACTUALLY WORK?!

        FloatProperty(Ball, "x", 0xEC); //YES!
        FloatProperty(Ball, "y", 0xF0); //this is the real ball location!

        //Ball->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        //    Isolate* isolate = info.GetIsolate();
        //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_ball));
        //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);

        Board->Set(isolate, "Ball", Ball);
    }

    global->Set(isolate, "Board", Board);
    global->Set(isolate, "LogicMgr", LogicMgr);
    //global->Set(isolate, "peggle", peggle);

    //global->Set(isolate, "eax", Integer::New(isolate, g_eax));
    //global->Set(isolate, "ecx", Integer::New(isolate, g_ecx));
    //global->Set(isolate, "edx", Integer::New(isolate, g_edx));
    //global->Set(isolate, "ballCount", Integer::New(isolate, g_ballCount));
    return global;
}

void* GetThunderballApp() {
    return *(void**)0x00687394; //im not sure if this changes if the peggle base is placed somewhere else but...
}

void* GetBoard() {
    //well i wanted to call the actual GetBoard function in peggle (at least that's what im pretty sure it did) but i lost the pointer to it because x32dbg crashed and didn't save half my comments :( (don't run low on space bruh)
    //__asm {
    //    jmp 
    //}
    return *(void**)(ULONG_PTR(GetThunderballApp())+0x7B8);
}

void* GetLogicMgrFromBoard(void* board) {
    return *(void**)(ULONG_PTR(board) + 0x154);
}

void* TryGetBallFromBoard(void* board) {
    //__debugbreak();

    //nah sometimes third be non-null but still be wrong lol lets just check the uhhh the variable in it
    DWORD ballHasBeenShot = *(DWORD*)(ULONG_PTR(board) + 0x1A4);

    if (ballHasBeenShot) { //if there is no ball the double dereference still works but + 8 holds 0! (yeah i guess sometimes that's not true lol)
        ULONG_PTR first = ULONG_PTR(board) + 0x1A0;
        ULONG_PTR second = **(ULONG_PTR**)(first);
        ULONG_PTR third = *(ULONG_PTR*)(second + 8);
        //ULONG_PTR ball_address = ((**(ULONG_PTR**)(ULONG_PTR(board) + 0x1A0)) + 8);

        return (void*)third;
    }
    return nullptr;
}

extern "C" {
    __declspec(dllexport) void __stdcall ChooseLeScript() {
        //std::wstring shit;

        showOpenFilePicker(g_filename, MAX_PATH * sizeof(wchar_t));
    }
    __declspec(dllexport) void __stdcall ChooseExtendedGlobals() {
        wchar_t path[MAX_PATH];
        showOpenFilePicker(path, MAX_PATH * sizeof(wchar_t));
        extended_binary = LoadLibrary(path);
        if (extended_binary) {
            daddygibmemyglobals = (JBSInit)GetProcAddress(extended_binary, "InitGlobals");
            if (daddygibmemyglobals == NULL) {
                OutputDebugString(L"GetProcAddress somehow failed?");
            }
        }
        else {
            OutputDebugString(L"ayo that shit didn't work we couldn't load library that bih");
            return;
        }
    }
    __declspec(dllexport) DWORD __stdcall RunLeCode(DWORD eax, DWORD ecx, DWORD edx, DWORD* ballCount, void* mainstruct) { //aw shit i expected all my functions to use __stdcall even though i didn't add it
        //__debugbreak();
        if (nullptr == virtualeight) {
            char exe[MAX_PATH]; GetModuleFileNameA(g_thismodule, exe, MAX_PATH);
            print(exe);

            virtualeight = new v8helper(exe);
        }
        bool newFile = g_filename[0] == '\0';
        while (g_filename[0] == '\0') {
            ChooseLeScript();
        }
        if (newFile) {
            //g_watcherthread = FileWatcher::StartThread();
            //FileWatcher::StartThread(g_filename);
        }
        g_eax = eax;
        g_ecx = ecx;
        g_edx = edx;
        g_ballCount = *ballCount;
        g_logicmgr = mainstruct;
        print(ULONG_PTR(g_logicmgr));
        print(ULONG_PTR(g_logicmgr) + 0x104);
        print(*(ULONG_PTR*)(ULONG_PTR(g_logicmgr) + 0x104)); //address of board
        g_board = *(void**)(ULONG_PTR(g_logicmgr) + 0x104);
        print(ULONG_PTR(g_board));
        print(ULONG_PTR(g_board) + 0x1A0);

        //the ball might not always be valid since i eval 
        g_ball = TryGetBallFromBoard(g_board);
        //g_ball = *(void**)((**(ULONG_PTR**)(ULONG_PTR(g_board) + 0x1A0)) + 8);
        //so lame no capture we gotta go global son
        //virtualeight->RunLeScript(filename, [](v8::Isolate* isolate) -> v8::Local<v8::ObjectTemplate> {
            
        //    });
        virtualeight->RunLeScript(g_filename, InitPeggleGlobals);

        print("g_returnval " << g_returnval);

        return g_returnval;
    }
    __declspec(dllexport) void __stdcall Eval() {
        SetForegroundWindow(GetConsoleWindow());
        if (nullptr == virtualeight) {
            char exe[MAX_PATH]; GetModuleFileNameA(g_thismodule, exe, MAX_PATH);
            print(exe);

            virtualeight = new v8helper(exe);
        }
        
        //oops i didn't initalize g_logicmgr or g_ballCount here
        g_board = GetBoard();
        g_logicmgr = GetLogicMgrFromBoard(g_board); //*(void**)(ULONG_PTR(g_board) + 0x154); //yeah
        g_ballCount = *(DWORD*)(ULONG_PTR(g_logicmgr) + 0x17C);
        g_ball = TryGetBallFromBoard(g_board);

        wchar_t scriptwstr[256];
        std::wcout << L">>> ";
        std::wcin.getline(scriptwstr, 256);
        virtualeight->Eval((const uint16_t*)scriptwstr, InitPeggleGlobals, true);
    }
}

void* FindPeggleBase() {
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32{};

    DWORD pid = GetCurrentProcessId();

    //  Take a snapshot of all modules in the specified process. 
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        print("CreateToolhelp32Snapshot (of modules)");
        return(FALSE);
    }

    //  Set the size of the structure before using it. 
    me32.dwSize = sizeof(MODULEENTRY32);

    //  Retrieve information about the first module, 
    //  and exit if unsuccessful 
    if (!Module32First(hModuleSnap, &me32))
    {
        print("Module32Firstfailed");  // Show cause of failure 
        CloseHandle(hModuleSnap);     // Must clean up the snapshot object! 
        return(FALSE);
    }

    void* peggle = nullptr;

    //  Now walk the module list of the process, 
    //  and display information about each module 
    do
    {
        wprintf(L"\n\n     MODULE NAME:     %s", me32.szModule);
        wprintf(L"\n     executable     = %s", me32.szExePath);
        wprintf(L"\n     process ID     = 0x%08X", me32.th32ProcessID);
        wprintf(L"\n     ref count (g)  =     0x%04X", me32.GlblcntUsage);
        wprintf(L"\n     ref count (p)  =     0x%04X", me32.ProccntUsage);
        wprintf(L"\n     base address   = 0x%08X", (DWORD)me32.modBaseAddr);
        wprintf(L"\n     base size      = %d", me32.modBaseSize);

        if (wcscmp(L"popcapgame1.exe", me32.szModule) == 0) { //bruh i forgot == 0
            peggle = me32.modBaseAddr;
            print("found peggle");
        }

    } while (Module32Next(hModuleSnap, &me32));

    printf(("\n"));

    //  Do not forget to clean up the snapshot object. 
    CloseHandle(hModuleSnap);

    if (peggle) {
        char buffer[100]; //idgaf
        snprintf(buffer, 100, "don't worry we goot em. %u", peggle);
        print(buffer);
        OutputDebugStringA(buffer);
    }
    return peggle;
}

void OverwriteModifyBallPtr(void* peggle) {
    //so we need to find the modify ball ptr func
    //in the PE file, it's located at 0005D880h bytes from the base (i found this out by putting the pointer to the function as it was already loaded in memory into the VA section of the address converter (fortunately the exe was loaded where it wanted to be (OptionalHeader->ImageBase == 00400000h)))
    //all we gotta do is find the base + this offset
    //https://stackoverflow.com/questions/2170843/va-virtual-address-rva-relative-virtual-address
    constexpr DWORD offset = 0x0005D880;
    //lowkey i COULD call load library on the peggle exe itself to find the base BUT you probably aren't supposed to call LoadLibrary in the DllMain...
    //
    DWORD modifyBallCount = (DWORD)peggle + offset;
    //DWORD nextInstruction = modifyBallCount + 15 + 7; //well write that in manually

    //__debugbreak();

    DWORD RLE_PTR = (DWORD)&RunLeCode;
    BYTE* RLE_PTR_BYTES = (BYTE*)&RLE_PTR;

    BYTE bytes[] = { //https://defuse.ca/online-x86-assembler.htm#disassembly
        //0x50,       //push eax
        //0x68,       //push imm32
        0x53,                                                                           //push ebx

        //0x51,                                                                                          //push ecx

        0x52,                                                                           //push edx

        //0x57,                                                                                          //push edi

        0x50,                                                                           //push eax
        //0x50,                                                                           //push eax again because im lazy and don't feel like finding it on the stack (wait nevermind wouldn't it be just esp because i just pushewd it)

        //push arguments to the stack for RunLeCode (since i made RunLeCode use __stdcall)
        0x8d, 0x84, 0x86, 0x7c, 0x01, 0x00, 0x00,                                       //lea eax, [esi+eax*4+0x17C] //ball count ptr
        0x56,                                                                           //oush esi
        0x50,                                                                           //push eax
        0x52,                                                                           //push edx
        0x51,                                                                           //push ecx
        0xff, 0x74, 0x24, 0x10,                                                         //push DWORD PTR [esp+0x10]
        //0x68, 0x8d, 0x63, 0x01, 0x00,                                                   //push 91021

        //move the address of RunLeCode into eax and call it (i could do a relative call too but i don't wanna lol my math was off for all of the relative shits the first time)
        0xb8, RLE_PTR_BYTES[0], RLE_PTR_BYTES[1], RLE_PTR_BYTES[2], RLE_PTR_BYTES[3],   //mov eax, imm32 (pointer to RunLeCode)
        0xff, 0xd0,                                                                     //call eax

        //store the return value of RunLeCode into edi and pop eax for the add calculation
        //0x89, 0xc7,                                                                                    //mov edi, eax
        //0x58,                                                                                          //pop eax
        //0x01, 0xbc, 0x86, 0x7c, 0x01, 0x00, 0x00,                                                      //add DWORD PTR [esi+eax*4+0x17C],edi

        //nevermind i want to store the return value of RunLeCode into ecx and pop eax for the add calculation
        0x89, 0xc1,                                                                     //mov ecx, eax
        0x58,                                                                           //pop eax
        0x01, 0x8c, 0x86, 0x7c, 0x01, 0x00, 0x00,                                       //add DWORD PTR [esi+eax*4+0x17C],ecx

        //0x5f,                                                                                          //pop edi

        0x5a,                                                                           //pop edx

        //0x59,                                                                                          //pop ecx

        0x5b,                                                                           //pop ebx

        //finally at the end jmp back to the next instruction in modifyBallCount
        0xE9, 0x00, 0x00, 0x00, 0x00      //jmp rel32 (we will write that in ourselves after we alloc)
    };
    //DWORD pid = GetCurrentProcessId();
    //well since we are in the process, do i even need to use VirtualAllocEx?
    //void* alloc = VirtualAllocEx(GetCurrentProcess(), NULL, sizeof(bytes), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); //kernel32 we good
    void* alloc = VirtualAlloc(NULL, sizeof(bytes), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!alloc) {
        MessageBoxA(NULL, "(OverwriteModifyBallPtr) VirtualAlloc failed!", "fuck. this might crash the process now", MB_OK);
        __debugbreak(); //KeBugCheck
    }
    print(alloc);
    memcpy(alloc, bytes, sizeof(bytes));
    DWORD* jmpRelBytes = (DWORD*)((DWORD)alloc + sizeof(bytes) - 4);
    //sizeof(bytes)-5 because that's where the jmp is...
    //oopsies apparently the jmp is not included? (that doesn't really make sense)
    //i guess even the jmp to alloc includes the jmp so i guess the jmp is actually included and i don't subtract 5 because eip has passed it
    *jmpRelBytes = (modifyBallCount + 15 + 7) - (DWORD(alloc) + sizeof(bytes)); //-5); //(DWORD)alloc - (modifyBallCount + 15 + 7);

    //so we need to write some shit at modifyBallCount + 15
    //wait do we need WriteProcessMemory() ? isn't this our process
    //let\'s just memcpy
    DWORD relativeAddr = (DWORD)alloc - (modifyBallCount + 15 + 5); //+5 for the jump instruction itself
    BYTE* dwordarr = (BYTE*)&relativeAddr;
    BYTE overwriteBytes[] = {
        0xE9, dwordarr[0], dwordarr[1], dwordarr[2], dwordarr[3], //jmp rel32 (we gotta calculate that)
        0xCC,                                                     //int3
        0xCC,                                                     //int3
    };
    //original size of the add instruction at modifyBallCount + 15 is 7 so let's put some int3 or something
    //add dword ptr ds:[esi+eax*4+0x17C], ecx
    //aw damn this failed because i forgot that executable pages aren't normally supposed to be written to and thus, don't have the write access flag or whatever
    DWORD oldprotectionidc;
    if (!VirtualProtect((void*)(modifyBallCount + 15), sizeof(overwriteBytes), PAGE_EXECUTE_READWRITE, &oldprotectionidc)) {
        OutputDebugStringA("that ol'e VirtualProtect failed");
        MessageBoxA(NULL, "(OverwriteModifyBallPtr) VirtualProtect failed!", "fuck. this might crash the process later", MB_OK);
        __debugbreak();
    }
    memcpy((void*)(modifyBallCount + 15), overwriteBytes, sizeof(overwriteBytes));
}

void OverwriteWindowProcKeyDownSection(void* peggle) {
    constexpr DWORD offset = 0x00002936;
    DWORD keyDownSection = ULONG_PTR(peggle) + offset; //this isn't the start of this procedure, it's the instruction i want to change in it
    //we are gonna change this big instruction into a jmp like i did for modify ball count
    DWORD E_PTR = (DWORD)&Eval;
    BYTE* E_PTR_BYTES = (BYTE*)&E_PTR;

    ULONG_PTR GAKS_PTR = (ULONG_PTR)&GetAsyncKeyState;
    BYTE* GAKS_PTR_BYTES = (BYTE*)&GAKS_PTR;
    //GetAsyncKeyState(VK_CONTROL) & 0x8000

    BYTE destinationBytes[] = {
        0x50,                                       //push eax
        0x53,                                       //push ebx
        0x51,                                       //push ecx
        0x52,                                       //push edx

        //check if edi holds the virtual keycode for 'E'
        //0x83, 0xff, 0x45,                           //cmp edi, 0x45
        //haha oops the keycode is actually passed through [ebp+8] and stored in edi AFTER the place i insert this code at
        //we gotta dereference it on the stack lol
        
        0x83, 0x7d, 0x08, 0x45,                     //cmp dword ptr [ebp+8], 0x45
        0x75, 0x17,                                 //jne finish (rel8)

        //now lets check if control is being held by calling GetAsyncKeyState and passing VK_CONTROL (0x11)
        //the following 4 lines of asm should be equivalent to GetAsyncKeyState(VK_CONTROL) & 0x8000
        0xb8, GAKS_PTR_BYTES[0], GAKS_PTR_BYTES[1], GAKS_PTR_BYTES[2], GAKS_PTR_BYTES[3],   //mov eax, imm32 (pointer to GetAsyncKeyState)
        //GetAsyncKeyState uses the __stdcall calling convention so we push arguments to the stack and the function adjusts the stack (instead of us doing it like with __cdecl) (this time the calling conventions actually mean something since this is x86)
        0x6a, 0x11,                                                             //push 0x11 (VK_CONTROL)
        0xff, 0xd0,                                                             //call eax

        //efficient asm (the and instruction will set the ZF (zero) flag and jz/je will jump if it is 0)
        0x25, 0x00, 0x80, 0x00, 0x00,                                           //and eax, 0x8000
        0x74, 0x07,                                                             //je finish (rel8)

        0xb8, E_PTR_BYTES[0], E_PTR_BYTES[1], E_PTR_BYTES[2], E_PTR_BYTES[3],   //mov eax, imm32 (pointer to Eval)
        0xff, 0xd0,                                                             //call eax

        //finish:

        0x80, 0xbe, 0xea, 0x00, 0x00, 0x00, 0x00,                               //cmp byte ptr [esi+0xEA], 0 ;original instruction we overwrote lol
                                                                                
        0x5a,                                                                   //pop edx
        0x59,                                                                   //pop ecx
        0x5b,                                                                   //pop ebx
        0x58,                                                                   //pop eax
                                                                                
        0xe9, 0x00, 0x00, 0x00, 0x00                                            //jmp rel32                  ;we'll write this in after we alloc like earlier
    };
    void* alloc = VirtualAlloc(NULL, sizeof(destinationBytes), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!alloc) {
        MessageBoxA(NULL, "(OverwriteWindowProcKeyDownSection) VirtualAlloc failed!", "fuck. this might crash the process now", MB_OK);
        __debugbreak(); //KeBugCheck
    }
    print(alloc);
    memcpy(alloc, destinationBytes, sizeof(destinationBytes));
    DWORD* jmpRelBytes = (DWORD*)((DWORD)alloc + sizeof(destinationBytes) - 4);
    *jmpRelBytes = (keyDownSection + 7) - (DWORD(alloc) + sizeof(destinationBytes)); //-5); //(DWORD)alloc - (modifyBallCount + 15 + 7);

    DWORD relativeAddr = (DWORD)alloc - (keyDownSection + 5); //+5 for the jump instruction itself
    BYTE* dwordarr = (BYTE*)&relativeAddr;
    BYTE overwriteBytes[] = {
        0xE9, dwordarr[0], dwordarr[1], dwordarr[2], dwordarr[3], //jmp rel32 (we gotta calculate that)
        0xCC,                                                     //int3
        0xCC,                                                     //int3
    };
    //original size of the add instruction at modifyBallCount + 15 is 7 so let's put some int3 or something
    //add dword ptr ds:[esi+eax*4+0x17C], ecx
    //aw damn this failed because i forgot that executable pages aren't normally supposed to be written to and thus, don't have the write access flag or whatever
    DWORD oldprotectionidc;
    if (!VirtualProtect((void*)(keyDownSection), sizeof(overwriteBytes), PAGE_EXECUTE_READWRITE, &oldprotectionidc)) {
        OutputDebugStringA("that ol'e VirtualProtect failed");
        MessageBoxA(NULL, "(OverwriteWindowProcKeyDownSection) VirtualAlloc failed!", "fuck. this might crash the process on the next keypress", MB_OK);
        __debugbreak();
    }
    memcpy((void*)(keyDownSection), overwriteBytes, sizeof(overwriteBytes));
}

//APIENTRY -> WINAPI -> __stdcall pops own arguments from stack!
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    //https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices

    //DebugBreak();
    //__asm {
    //    INT 3
    //}
    OutputDebugStringA("DllMain called!");
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        OutputDebugStringA("dll starting...!");
        constexpr size_t bsize = 6 + 10; //mango + " " is 6, 10 is the length in characters of 2**32
        char buffer[bsize];
        snprintf(buffer, bsize, "mango %u", ul_reason_for_call);
        OutputDebugStringA("snprintf");
        if (AllocConsole()) {
            freopen("CONIN$", "r", stdin);
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
        }
        g_peggle = FindPeggleBase();
        //g_peggle_internal_activate_ability = (void*)(ULONG_PTR(g_peggle) + 0x0006edf0);
        MessageBoxA(NULL, (const char*)buffer, "phonk", MB_OK);
        OverwriteModifyBallPtr(g_peggle);
        OverwriteWindowProcKeyDownSection(g_peggle);

        //char exe[MAX_PATH]; GetModuleFileNameA(hModule, exe, MAX_PATH); //nah no way i wrote NULL instead of THIS module (it was using popcapgame1???)
        //print(exe);
        //nah im moving all this to the top of RunLeCode (and making the exe path automatic in the constructor);
        //since i use NtCreateThreadEx, dll main is run on a new thread and v8 doesn't like that!

        //notifyEvent = CreateEvent(NULL, FALSE, FALSE, L"NOTIFY");

        //virtualeight = new v8helper(exe);
        g_thismodule = hModule;
        print(g_thismodule);
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        delete virtualeight;
    }

    //switch (ul_reason_for_call)
    //{
    //case DLL_PROCESS_ATTACH:
    //
    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    //case DLL_PROCESS_DETACH:
    //    break;
    //}
    return TRUE;
}