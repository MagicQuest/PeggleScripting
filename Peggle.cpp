#include "pch.h"
#include "Peggle.h"
#include "dllmain.h"
#include "v8helper2.h"

bool Peggle::init = false;

void* Peggle::logicMgr = nullptr;
void* Peggle::board = nullptr;
void* Peggle::ball = nullptr;

struct TypeDescriptor {
    ULONG_PTR pVFTable;
    ULONG_PTR spare;
    char name[1];
};

struct RTTICompleteObjectLocator
{
    DWORD signature; //always zero ?
    DWORD offset;    //offset of this vtable in the complete class
    DWORD cdOffset;  //constructor displacement offset
    TypeDescriptor* pTypeDescriptor; //TypeDescriptor of the complete class
    struct RTTIClassHierarchyDescriptor* pClassDescriptor; //describes inheritance hierarchy
};

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

#define setBytePointerProperty(OT, name, offset) OT->SetAccessorProperty(LITERAL(name),                                                                                     \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     info.GetReturnValue().Set(Number::New(isolate, *(BYTE*)(base + offset)));                                         \
                                                 }),                                                                                                                    \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     DWORD base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     BYTE* prop = (BYTE*)(base + offset);                                                                              \
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
#define ByteProperty(OT, name, offset) setBytePointerProperty(OT, name, offset)
#define FloatProperty(OT, name, offset) setFloatPointerProperty(OT, name, offset)

#define ConstProperty(OT, constname) OT->Set(isolate, #constname, Number::New(isolate, constname))

#define ABILITY_SUPERGUIDE 1
#define ABILITY_FLIPPERS 2
#define ABILITY_MULTIBALL 3
#define ABILITY_PYRAMID 4
#define ABILITY_SPACEBLAST 5
#define ABILITY_SPOOKYBALL 6
#define ABILITY_ZENSHOT 7
//lowkey past here idk what these do
//nah wtf is shot extender a hidden ability or something?
#define ABILITY_SHOTEXTENDER 8
#define ABILITY_FLOWERPOWER 9
#define ABILITY_NUDGE 10
#define ABILITY_TIMEBOMB 11
#define ABILITY_FIREBALL 12
//im also not sure about this one lol
#define ABILITY_WARRENSPECIAL 13

#define PEG_BLUE 1
#define PEG_ORANGE 2
#define PEG_PURPLE 3
#define PEG_GREEN 4

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
    //hold on i just randomly thought about it and i think you could still call a method without asm!
    //void*(__fastcall mango)(DWORD register_ecx, DWORD register_edx_unused, DWORD stack_param2, DWORD stack_param3, ...);
    //using fastcall you could pass the object as the very first parameter
    //DWORD unknown1 = IntegerFI(info[0]);

    //DWORD unknown2;// = IntegerFI(info[1]);

    if (info[0]->BooleanValue(isolate)) {
        unknown1 = IntegerFI(info[0]);
    }
    if (info[1]->BooleanValue(isolate)) {
        unknown2 = IntegerFI(info[1]);
    }

    DWORD ability = 0; //IntegerFI(info[2]);
    if (info[2]->BooleanValue(isolate)) {
        ability = IntegerFI(info[2]);
    }


    BOOL isAbilitySpecified = false;

    if (info[3]->BooleanValue(isolate)) {
        isAbilitySpecified = IntegerFI(info[3]);
    }

    ULONG_PTR peggle_internal_activate_ability = (ULONG_PTR)g_peggle + offset;

    //bruh this actually worked but didn't at the same time
    __asm {
        //int 3
        push isAbilitySpecified
        push ability
        //lea eax, unknown2
        //push eax
        //lea eax, unknown1
        //push eax
        push unknown2
        push unknown1
        mov ecx, Peggle::logicMgr
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
        mov ecx, Peggle::board
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
        mov ecx, Peggle::logicMgr
        call eax
    }
}

void peggle_set_peginfo_hit(void* PegInfo, DWORD hit) {
    constexpr DWORD offset = 0x00076650;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov eax, funcaddr
        push hit
        mov ecx, PegInfo
        call eax
    }
}

void peggle_set_peg_type(void* BrickOrBall, DWORD ptype) { //im assuming i know what this function does lol (fortunately if the object doesn't have peginfo it will safely return)
    constexpr DWORD offset = 0x00079870;
    
    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov eax, funcaddr
        push ptype
        mov ecx, BrickOrBall
        call eax
    }
}

void* peggle_create_floating_text(void* board) {
    void* floatingTextMgr = *(void**)(ULONG_PTR(board) + 0x148);
    
    constexpr DWORD offset = 0x00069D20;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov eax, funcaddr
        mov ecx, floatingTextMgr
        call eax
    }
}

constexpr DWORD c_getAbilityOffset(DWORD ability) {
    return ((2 * ability) * 4) + 0x1D4;
}

namespace Templates {
    using namespace v8;

    Local<Object> getBallImpl(Isolate* isolate, void* ptr);
    Local<Object> getBrickImpl(Isolate * isolate, void* ptr);
    Local<Object> getPegInfoImpl(Isolate * isolate, void* ptr);
    Local<Object> getFloatingTextImpl(Isolate * isolate, void* ptr);
    v8::Local<v8::Object> GetBallOrBrickImplFromPtr(v8::Isolate * isolate, void* ptr);

    const v8::Local<v8::ObjectTemplate>& GenericPhysObject(v8::Isolate* isolate, void* ptr) {
        using namespace v8;
        Local<ObjectTemplate> ptr_object = ObjectTemplate::New(isolate);
        ptr_object->Set(isolate, "set_dword", FunctionTemplate::New(isolate, generic_set_dword));
        ptr_object->Set(isolate, "set_float", FunctionTemplate::New(isolate, generic_set_float));
        ptr_object->Set(isolate, "get_dword", FunctionTemplate::New(isolate, generic_get_dword));
        ptr_object->Set(isolate, "get_float", FunctionTemplate::New(isolate, generic_get_float));
        ptr_object->Set(isolate, "location", Number::New(isolate, (ULONG_PTR)ptr));

        //sometimes PegInfo is null so we gotta check first
        ptr_object->SetAccessorProperty(LITERAL("PegInfo"), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR pegInfoPtr = *(ULONG_PTR*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0xD0);
            //peginfoptr is the value at +0xD0
            //print(pegInfoPtr);
            if (pegInfoPtr) {
                info.GetReturnValue().Set(Templates::getPegInfoImpl(isolate, (void*)pegInfoPtr));
            }
        }), Local<FunctionTemplate>());
        ptr_object->Set(isolate, "set_peginfo_type", FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* thisObj = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            peggle_set_peg_type(thisObj, IntegerFI(info[0]));
        }));
        //ptr_object->SetAccessorProperty(LITERAL("location"), Local<FunctionTemplate>(), Local<FunctionTemplate>(), PropertyAttribute::ReadOnly);
        //ptr_object->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        //    Isolate* isolate = info.GetIsolate();
        //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_logicmgr));
        //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);
        return ptr_object;
    }

    const v8::Local<v8::ObjectTemplate>& GenericPtrObject(v8::Isolate* isolate, void* ptr) {
        using namespace v8;
        Local<ObjectTemplate> ptr_object = ObjectTemplate::New(isolate);
        ptr_object->Set(isolate, "set_dword", FunctionTemplate::New(isolate, generic_set_dword));
        ptr_object->Set(isolate, "set_float", FunctionTemplate::New(isolate, generic_set_float));
        ptr_object->Set(isolate, "get_dword", FunctionTemplate::New(isolate, generic_get_dword));
        ptr_object->Set(isolate, "get_float", FunctionTemplate::New(isolate, generic_get_float));
        ptr_object->Set(isolate, "location", Number::New(isolate, (ULONG_PTR)ptr));
        //ptr_object->SetAccessorProperty(LITERAL("location"), Local<FunctionTemplate>(), Local<FunctionTemplate>(), PropertyAttribute::ReadOnly);
        //ptr_object->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        //    Isolate* isolate = info.GetIsolate();
        //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_logicmgr));
        //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);
        return ptr_object;
    }

    Local<ObjectTemplate> _Ball;
    Local<ObjectTemplate> _Brick;
    Local<ObjectTemplate> _PegInfo;
    Local<ObjectTemplate> _FloatingText;
    void Init(v8::Isolate* isolate) {
        //i think ball and brick have some class in common where both Ball and Brick share a pointer to a PegInfo object at 0xD0
        _Ball = GenericPhysObject(isolate, nullptr);
        FloatProperty(_Ball, "multiball", 0xE8); //idk
        ByteProperty(_Ball, "fireball", 0x161);
        FloatProperty(_Ball, "drawX", 0x134); //honestly i don't know yet lol
        FloatProperty(_Ball, "drawY", 0x138); //yeah idk it seems like it might be where it draws it at?
        FloatProperty(_Ball, "radius", 0x13C); //WHY THE FUCK IS THERE A RADIUS PROPERTY AND WHY DOES IT ACTUALLY WORK?!
        FloatProperty(_Ball, "x", 0xEC); //YES!
        FloatProperty(_Ball, "y", 0xF0); //this is the real ball location!
        FloatProperty(_Ball, "velX", 0xFC); //with me randomly guessing i got it
        FloatProperty(_Ball, "velY", 0x100);

        _Brick = GenericPhysObject(isolate, nullptr);
        FloatProperty(_Brick, "threadStuff", 0x4); //prolly don't mess with this one
        FloatProperty(_Brick, "x", 0xEC); //honestly we're gonna assume the x and y are provided by PhysObject
        FloatProperty(_Brick, "y", 0xF0);
        FloatProperty(_Brick, "maybeX", 0x114); //hmm when the hell did i find this, it could be right (offset 00081740 GetY(?))
        FloatProperty(_Brick, "maybeY", 0x118);


        _PegInfo = GenericPtrObject(isolate, nullptr);
        Property(_PegInfo, "type", 0x10);
        //Property(_PegInfo, "hit", 0x14); //boolean
        ByteProperty(_PegInfo, "hit", 0x14);
        Property(_PegInfo, "timeSinceHit", 0x18); //for some reason there's 2
        Property(_PegInfo, "hitCount", 0x1C); //hmm
        //what a terrible name but this might be the best way to put it
        //it's the length of time for which this peg was the last one the ball hit
        Property(_PegInfo, "lengthOfTimeThisPegWasLastHit", 0x20); //for some reason there's 2 (oh wait)
        Property(_PegInfo, "popped", 0x24);
        _PegInfo->Set(isolate, "set_peg_hit", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* peginfo = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            peggle_set_peginfo_hit(peginfo, IntegerFI(info[0]));
        }));

        _FloatingText = GenericPtrObject(isolate, nullptr);
        FloatProperty(_FloatingText, "x", 0x80); //i mean im pretty sure
        FloatProperty(_FloatingText, "y", 0x84);
        FloatProperty(_FloatingText, "something", 0x8C); //is like -0.1
        //uhhh in the showHoveringScoreOverPeg function they set FloatingText+5C to 0x2D (45 idk what that is for)
        //FloatingText+0x60 is 0x00FFFF00
        //FloatingText+0x68 is 0x64
        //FloatingText+0x78 is later set to A (like the number 10?!)
        //FloatingText+0x7C is 1
        Property(_FloatingText, "number", 0x78); //maybe? (prolly not :( )
    }

    Local<Object> getBallImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _Ball->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        return object;
    }

    Local<Object> getBrickImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _Brick->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        return object;
    }

    Local<Object> getPegInfoImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _PegInfo->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        return object;
    }

    Local<Object> getFloatingTextImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _FloatingText->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        return object;
    }

    v8::Local<v8::Object> GetBallOrBrickImplFromPtr(v8::Isolate* isolate, void* ptr) {
        using namespace v8;
        ULONG_PTR* vtable = *(ULONG_PTR**)g_eax;
        //print(vtable);
        //print((ULONG_PTR)vtable - sizeof(ULONG_PTR));
        //print((ULONG_PTR)vtable - 4);
        //__debugbreak();
        
        //RTTICompleteObjectLocator objRTTI = *(RTTICompleteObjectLocator*)(ULONG_PTR(vtable) - sizeof(ULONG_PTR)); //on x86 this is vtable - 4
        //OH ok the problem was that the first dereference only got the VALUE at (vtable - 4) and i had to dereference it AGAIN if i wanted the object
        //orttip is actually what i wanted to do all along!

        RTTICompleteObjectLocator* orttip = *(RTTICompleteObjectLocator**)(ULONG_PTR(vtable) - sizeof(ULONG_PTR)); //bruh how am i struggling to dereference all this stuff i feel like i could do it in asm no problem lmao
        //print(orttip);
        //print(orttip->pTypeDescriptor);
        //__debugbreak();
        //print(objRTTI);
        //__debugbreak();
        //char* name = objRTTI->pTypeDescriptor->name;
        //print(objRTTI.pTypeDescriptor);
        //__debugbreak();
        //print((ULONG_PTR)objRTTI.pTypeDescriptor->name);
        //__debugbreak();
        //print(objRTTI.pTypeDescriptor->name);
        //__debugbreak();
        //char* name = objRTTI.pTypeDescriptor->name;
        char* name = orttip->pTypeDescriptor->name;
        //print(name);
        //__debugbreak();
        Local<Object> obj;
        if (strcmp(".?AVBall@Sexy@@", name) == 0) {
            obj = Templates::getBallImpl(isolate, ptr);
        }
        else if (strcmp(".?AVBrick@Sexy@@", name) == 0) {
            obj = Templates::getBrickImpl(isolate, ptr);
        }
        //print("about to return!");
        return obj;
    }
}

namespace Events {
    void onInit(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        Local<Function> func = info[0].As<Function>();

        if (Peggle::init) {
            TryCatch errhandler(isolate);
            MaybeLocal<Value> result = func->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), 0, nullptr);
            CHECKEXCEPTIONS(errhandler);
        }
    }

    void onBallCountModify(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        Local<Function> func = info[0].As<Function>();

        //print("ball count modify");

        DWORD id = InterlockedCompareExchange((volatile ULONG_PTR*)&v8helper2::notify_id, 0, 0);
        if (id == V8_NOTIFY_RUN_MODIFYBALLPTR) {
            //print("running ball count");
            Local<Value> args[] = { Integer::New(isolate, g_eax), Integer::New(isolate, g_ecx), Integer::New(isolate, g_edx), Integer::New(isolate, *(DWORD*)(ULONG_PTR(Peggle::GetLogicMgrFromBoard(Peggle::board)) + 0x17C)) };
            TryCatch errhandler(isolate);
            MaybeLocal<Value> result = func->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), 4, args);
            CHECKEXCEPTIONS(errhandler);
            if (!result.IsEmpty()) {
                v8helper2::returnval = IntegerFI(result.ToLocalChecked());
            }
            else {
                v8helper2::returnval = g_ecx;
            }
        }

        //print("ball count exit");
    }

    void onPegHit(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        Local<Function> func = info[0].As<Function>();

        //print("peg hit");

        DWORD id = InterlockedCompareExchange((volatile ULONG_PTR*)&v8helper2::notify_id, 0, 0);
        if (id == V8_NOTIFY_RUN_PEGHIT) {
            //print("running peg hit");
            //nah i gotta check the types of these bruh
            Local<Object> ball1 = Templates::GetBallOrBrickImplFromPtr(isolate, (void*)g_eax); //Templates::getBallImpl(isolate, (void*)g_eax);
            Local<Object> ball2 = Templates::GetBallOrBrickImplFromPtr(isolate, (void*)g_ecx);

            Local<Value> args[] = { ball1, ball2, Integer::New(isolate, g_edx) };
            TryCatch errhandler(isolate);
            MaybeLocal<Value> result = func->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), ARRAYSIZE(args), args);
            CHECKEXCEPTIONS(errhandler);
            if (!result.IsEmpty()) {
                v8helper2::returnval = IntegerFI(result.ToLocalChecked());
            }
            else {
                v8helper2::returnval = g_ecx;
            }
        }

        //print("peg hit exit");
    }

    void onKeyDown(const v8::FunctionCallbackInfo<v8::Value>& info) {
        using namespace v8;
        Isolate* isolate = info.GetIsolate();
        //print("on key down js callback");
        Local<Function> func = info[0].As<Function>();
        DWORD id = InterlockedCompareExchange((volatile ULONG_PTR*)&v8helper2::notify_id, 0, 0);
        if (id == V8_NOTIFY_RUN_KEYDOWN) {
            Local<Value> args[] = { Integer::New(isolate, g_eax), Integer::New(isolate, g_ecx), Integer::New(isolate, g_edx)};
            //print("running on keydow " << sizeof(args)); //what the hell is sizeof doing (oops it returns the size in bytes not elements!)
            TryCatch errhandler(isolate);
            MaybeLocal<Value> result = func->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), ARRAYSIZE(args), args);
            CHECKEXCEPTIONS(errhandler);
            if (!result.IsEmpty()) {
                v8helper2::returnval = IntegerFI(result.ToLocalChecked());
            }
            else {
                v8helper2::returnval = g_ecx;
            }
        }
        //print("key down eixt");
    }
}

/*v8::Local<v8::Object> GetBallObjectImpl(v8::Isolate* isolate) { //lazy function so i don't create more than one object template for the ball
    using namespace v8;
    static bool ran = false;
    static Local<ObjectTemplate> Ball = GenericPtrObject(isolate, nullptr); //ObjectTemplate::New(isolate);
    //print("getting le ball " << ran << " " << CStringFI(Ball->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()));
    if (!ran) {
        print("just made a new GenericPtrObject for Ball and am setting the properties now!");
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
    }
    ran = true;

    print("getting le ball " << Ball.IsEmpty());

    return Ball->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
}*/

void Peggle::InitPeggleGlobals(const v8::Local<v8::ObjectTemplate>& global, v8::Isolate* isolate) {
    using namespace v8;
    //Local<ObjectTemplate> global;// = ObjectTemplate::New(isolate);

    board = GetBoard();
    logicMgr = GetLogicMgrFromBoard(board);
    //g_ball = TryGetBallFromBoard(g_board);

    Templates::Init(isolate);
    
    global->Set(isolate, "onInit", FunctionTemplate::New(isolate, Events::onInit));
    global->Set(isolate, "onBallCountModify", FunctionTemplate::New(isolate, Events::onBallCountModify));
    global->Set(isolate, "onPegHit", FunctionTemplate::New(isolate, Events::onPegHit));
    global->Set(isolate, "onKeyDown", FunctionTemplate::New(isolate, Events::onKeyDown));
//#error LogicMgr will be invalid if we choose a new level, i'd have to re call init peggle globals
    Local<ObjectTemplate> LogicMgr = Templates::GenericPtrObject(isolate, logicMgr); //ObjectTemplate::New(isolate);

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


    Property(LogicMgr, "time", 0x8); //idk what the purpose of this one is
    Property(LogicMgr, "relatedX", 0xA0);
    Property(LogicMgr, "relatedY", 0xA4);
    Property(LogicMgr, "specialsomething", 0x20); //hmm this doesn't seem to change alot
    Property(LogicMgr, "multipliedPointsThisShot", 0x108); //YO LET'S GOT THIS HAS TO BE RIGHT!
    Property(LogicMgr, "originalPointsThisShot", 0x10C); //also special in some weay
    Property(LogicMgr, "pegsHit", 0x120);

    Property(LogicMgr, "specialsomething2", 0x170);
    Property(LogicMgr, "score", 0x174);
    Property(LogicMgr, "balls", 0x17C);
    
    Property(LogicMgr, "currentAbility", 0x1C4);

    Property(LogicMgr, "superGuideCount", c_getAbilityOffset(ABILITY_SUPERGUIDE));
    //PeggleProperty("flipperCount", 0x1E4);
    //PeggleProperty("spookyCount", 0x204);
    //PeggleProperty("zenCount", 0x20C);
    Property(LogicMgr, "flipperCount", c_getAbilityOffset(ABILITY_FLIPPERS));

    Property(LogicMgr, "pyramidCount", c_getAbilityOffset(ABILITY_PYRAMID));
    //PeggleProperty("spaceBlastCount", getAbilityOffset(4));
    Property(LogicMgr, "spookyCount", c_getAbilityOffset(ABILITY_SPOOKYBALL));
    Property(LogicMgr, "zenCount", c_getAbilityOffset(ABILITY_ZENSHOT));
    Property(LogicMgr, "fireBallCount", c_getAbilityOffset(ABILITY_FIREBALL));

    Property(LogicMgr, "ballsShot", 0x248);
    Property(LogicMgr, "freeBallsGivenByBucket", 0x254);
    Property(LogicMgr, "freeBallsGivenByPoints", 0x254); //oops what was it?

    ByteProperty(LogicMgr, "won", 0xF6);
    Property(LogicMgr, "checkshit", 0xFB); //idk yet lol

    Property(LogicMgr, "orangePegsLeft", 0x360); //yo this might be the amount of orange pegs left...
    Property(LogicMgr, "bluePegsLeft", 0x36C);
    Property(LogicMgr, "uniquePegsHit", 0x12C); //amount of pegs hit or SOMETHING?
    //Property(LogicMgr, "uniquePegsHit", 0x354); //um im pretty sure they both hold the exact same value (but technically this might be a different object)

    //i think i've addded them all but im not sure whawt some of them do yet because they have weird names
    ConstProperty(global, ABILITY_SUPERGUIDE);
    ConstProperty(global, ABILITY_FLIPPERS);
    ConstProperty(global, ABILITY_MULTIBALL);
    ConstProperty(global, ABILITY_PYRAMID);
    ConstProperty(global, ABILITY_SPACEBLAST);
    ConstProperty(global, ABILITY_SPOOKYBALL);
    ConstProperty(global, ABILITY_ZENSHOT);
    ConstProperty(global, ABILITY_SHOTEXTENDER);
    ConstProperty(global, ABILITY_FLOWERPOWER);
    ConstProperty(global, ABILITY_NUDGE);
    ConstProperty(global, ABILITY_TIMEBOMB);
    ConstProperty(global, ABILITY_FIREBALL);
    ConstProperty(global, ABILITY_WARRENSPECIAL);

    ConstProperty(global, PEG_BLUE);
    ConstProperty(global, PEG_ORANGE);
    ConstProperty(global, PEG_PURPLE);
    ConstProperty(global, PEG_GREEN);

    LogicMgr->SetAccessorProperty(LITERAL("text"),
        FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            //getter
            Isolate* isolate = info.GetIsolate();
            Local<String> str;
            if (String::NewFromUtf8(isolate, (const char*)(ULONG_PTR(logicMgr) + 0x80)).ToLocal(&str)) {
                info.GetReturnValue().Set(str);
            }
            else {
                print("something went wrong reading peggle.text");
            }
        }),
        FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            //setter
            Isolate* isolate = info.GetIsolate();
            char* text = (char*)((ULONG_PTR)logicMgr + 0x80);
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

    Local<ObjectTemplate> Board = Templates::GenericPtrObject(isolate, board);
    Board->Set(isolate, "createFloatingText", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        void* floatingText = peggle_create_floating_text((void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()));
        if (floatingText) {
            info.GetReturnValue().Set(Templates::getFloatingTextImpl(isolate, floatingText));
        }
    }));
    Property(Board, "ballHasBeenShot", 0x1A4); //idk about this one lol (seems very very likely tho)
    Property(Board, "viewX", 0x1A8);
    Property(Board, "viewY", 0x1AC);
    Property(Board, "paused", 0x1B8); //prolly  
    //Board->Set(isolate, "set_dword", FunctionTemplate::New(isolate, generic_set_dword));
    //Board->Set(isolate, "get_dword", FunctionTemplate::New(isolate, generic_get_dword));
    //Board->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_board));
    //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);

    /*if (g_ball != nullptr) {
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
    }*/
    Board->SetAccessorProperty(LITERAL("Ball"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        void* ball = TryGetBallFromBoard(board);
        if (ball) {
            //Local<Object> Ball = GetBallObjectImpl(isolate);
            //print("post get ball impl");
            //Ball->Set(isolate->GetCurrentContext(), LITERAL("location"), Number::New(isolate, (ULONG_PTR)ball));
            //print("ogmegdg");
            ////info.GetReturnValue().Set(GetBallObjectImpl(isolate, ball));
            //info.GetReturnValue().Set(Ball);
            //print("et");
            info.GetReturnValue().Set(Templates::getBallImpl(isolate, ball));
        }
    }), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        ULONG_PTR first = ULONG_PTR(board) + 0x1A0;
        ULONG_PTR second = **(ULONG_PTR**)(first);

        __debugbreak();
        if (info[0]->IsNumber()) {
            *(ULONG_PTR*)(second + 8) = IntegerFI(info[0]);
        }
        else if (info[0]->IsObject()) {
            *(ULONG_PTR*)(second + 8) = IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        }
    })); //lmao i accidently left that shit read only

    global->Set(isolate, "Board", Board);
    global->Set(isolate, "LogicMgr", LogicMgr);
    //global->Set(isolate, "peggle", peggle);

    //global->Set(isolate, "eax", Integer::New(isolate, g_eax));
    //global->Set(isolate, "ecx", Integer::New(isolate, g_ecx));
    //global->Set(isolate, "edx", Integer::New(isolate, g_edx));
    //global->Set(isolate, "ballCount", Integer::New(isolate, g_ballCount));
    //return global;
}

void* Peggle::GetThunderballApp() {
    return *(void**)0x00687394; //im not sure if this changes if the peggle base is placed somewhere else but...
}

void* Peggle::GetBoard() {
    //well i wanted to call the actual GetBoard function in peggle (at least that's what im pretty sure it did) but i lost the pointer to it because x32dbg crashed and didn't save half my comments :( (don't run low on space bruh)
    //__asm {
    //    jmp 
    //}
    return *(void**)(ULONG_PTR(GetThunderballApp()) + 0x7B8);
}

void* Peggle::GetLogicMgrFromBoard(void* board) {
    return *(void**)(ULONG_PTR(board) + 0x154);
}

void* Peggle::TryGetBallFromBoard(void* board) {
    //__debugbreak();

    //nah sometimes third be non-null but still be wrong lol lets just check the uhhh the variable in it
    DWORD ballHasBeenShot = *(DWORD*)(ULONG_PTR(board) + 0x1A4);

    if (ballHasBeenShot) { //if there is no ball the double dereference still works but + 8 holds 0! (yeah i guess sometimes that's not true lol)
        ULONG_PTR first = ULONG_PTR(board) + 0x1A0;
        ULONG_PTR second = **(ULONG_PTR**)(first);
        ULONG_PTR third = *(ULONG_PTR*)(second + 8);
        //*(ULONG_PTR*)(second + 8) = 20;
        //ULONG_PTR ball_address = ((**(ULONG_PTR**)(ULONG_PTR(board) + 0x1A0)) + 8);

        return (void*)third;
    }
    return nullptr;
}