#include "pch.h"
#include "Peggle.h"
#include "dllmain.h"
#include "v8helper2.h"

bool Peggle::init = false;

//board is 592 bytes long
void* Peggle::board = nullptr;
//logicMgr is 924 bytes long (damn)
void* Peggle::logicMgr = nullptr;
//ball is 400 bytes long
//void* Peggle::ball = nullptr;

typedef void* (__cdecl* opnfunc)(DWORD size); //__cdecl is implied and im just adding it here because i can

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

//this object is used in a lot of random places but has no vtable so who knows what its real name is
struct EmbeddedText {
    //0
    DWORD unknown; //lowkey idk if this means anything
    //4
    char text[16]; //including null terminator
    //20 (0x14)
    DWORD textLength;
    //24 (0x18)
    DWORD maxLength; //if max length is bigger than 0x10, the string will be allocated on the heap and text will actually point to that new memory

    EmbeddedText() {
        //__debugbreak();
        text[0] = '\0';
        textLength = 0;
        maxLength = 0xF; //default i assume
    }
};

/*
struct RefCount {
    DWORD refCount;
};

struct EditValObj : public RefCount {
    DWORD unknown1;
    DWORD unknown2;
    EditValObj() {
        this->refCount = 0; //not sure why EditValObj is the one to set refCount to 0 lol
    }
};

struct PegInfo : public EditValObj { //0x28 bytes long
    DWORD type = 1;
    BYTE hit = 0;
    BYTE _0x15_ = 1; //idk what these 3 bytes are for lol
    BYTE _0x16_ = 0;
    BYTE _0x17_ = 0;
    DWORD timeSinceHit = 0;
    DWORD hitCount = 0;
    DWORD animationPercent = 0;
    DWORD animationId = 0;

    //everything past this comment is unofficial lol
    PegInfo() {};
    PegInfo(DWORD type, DWORD animationId) { 
        this->type = type;
        this->animationId = animationId;
    }
    virtual ~PegInfo() {}; //this thing has like 3 virtual methods and im assuming the destructor is one of them (i need at least one virtual function so that the vtable takes up the first 4 bytes (like it normally does))
};

//228 bytes long
struct PhysObj : public EditValObj {
    DWORD type = 0; //0x10
    //[[
    DWORD d_0x14_; //wait these might be the bounds
    DWORD d_0x18_;
    DWORD d_0x1C_;
    DWORD d_0x20_;
    //]]
    BYTE collision = 1; //0x24
    BYTE visible = 1; //0x25
    BYTE enableMover = 1; //0x26
    BYTE b_0x27_ = 0; //0x27
    BYTE b_0x28_ = 0; //0x28
    BYTE b_0x29_ = 0; //0x29
    BYTE b_0x2A_ = 0; //0x2A
    BYTE b_0x2B_ = 0; //0x2B
    BYTE b_0x2C_ = 0; //0x2C
    BYTE b_0x2D_ = 0; //0x2D
    BYTE b_0x2E_ = 0; //0x2E
    BYTE b_0x2F_ = 0; //0x2F
    BYTE b_0x30_ = 0; //0x30
    //[[
    BYTE b_0x31_;
    BYTE b_0x32_;
    BYTE b_0x33_;
    //]]
    float f_0x34_ = 0.0; //0x34
    float f_0x38_ = 0.0; //0x38
    float f_0x3C_ = 0.0; //0x3C
    DWORD d_0x40_ = 0; //0x40
    DWORD d_0x44_ = 0x00FFFFFF; //0x44 (color?)
    void* mover = nullptr; //0x48
    DWORD time = 0; //0x4C
    DWORD d_0x50_ = 0; //0x50
    DWORD d_0x54_ = 0; //0x54
    DWORD time2 = 0; //0x58
    EmbeddedText textidk; //0x5C (EmbeddedText is 28 bytes!)
    EmbeddedText textidk2; //0x78 (EmbeddedText is 28 bytes!)
    EmbeddedText textidk3; //0x94 (EmbeddedText is 28 bytes!)
    DWORD d_0xB0_ = 0; //0xB0
    DWORD d_0xB4_ = 0; //0xB4
    float  f0xB8_ = 0.0; //0xB8
    float f_0xBC_ = 0.0; //0xBC
    float f_0xC0_ = 0.0; //0xC0
    BYTE b_0xC4_ = 0; //0xC4
    //[[
    BYTE b_0xC5_;
    BYTE b_0xC6_;
    BYTE b_0xC7_;
    //]]
    DWORD d_0xC8_ = 0; //0xC8
    DWORD d_0xCC_ = 0; //0xCC
    void* peginfo = nullptr; //0xD0
    DWORD d_0xD4_ = 0; //0xD4
    DWORD d_0xD8_ = 0; //0xD8
    DWORD d_0xDC_ = 0; //0xDC
    BYTE b_0xE0_ = 0; //0xE0
    BYTE b_0xE1_ = 0; //0xE1
    //[[
    BYTE b_0xE2_;
    BYTE b_0xE3_;
    //]]

    virtual void loadProperties(void* reader) {};
};

//328 bytes long (i know nothing about this one lol)
struct Poly : public PhysObj {
    //DWORD type = 5;

    //[[
    DWORD _0xE4_;
    //]]
    DWORD _0xE8_ = 0;
    DWORD _0xEC_ = 0;
    DWORD _0xF0_ = 0;
    //[[
    DWORD _0xF4_; //idk
    //]]
    DWORD _0xF8_ = 0;
    DWORD _0xFC_ = 0;
    DWORD _0x100_ = 0;
    //[[
    DWORD _0x104_;
    //]]
    DWORD _0x108_ = 0;
    DWORD _0x10C_ = 0;
    DWORD _0x110_ = 0;
    float _0x114_ = 0.0;
    float _0x118 = 0.0;
    float _0x11C_ = 0.0; //fi
    float _0x120_ = 0.0; //fi
    float _0x124_ = 1.0; //fi
    float _0x128_ = 0.0; //fi
    BYTE _0x12C_ = 0;
    BYTE _0x12D = 1;
    BYTE _0x12E = 1;
    BYTE _0x12F_ = 0; //fi
    BYTE _0x130_ = 0; //fi
    //...
    DWORD _0x134_ = 1; //fi
    DWORD _0x138_ = 0; //fi
    DWORD _0x13C_ = 0; //fi
    DWORD _0x140_ = 0; //fi
    BYTE _0x144_ = 1; //fi
    BYTE _0x145_ = 0; //fi
    BYTE _0x146_ = 0; //fi
    //...
};*/

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
                                                     ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     info.GetReturnValue().Set(Number::New(isolate, *(DWORD*)(base + offset)));                                         \
                                                 }),                                                                                                                    \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     DWORD* prop = (DWORD*)(base + offset);                                                                              \
                                                     *prop = IntegerFI(info[0]);                                                                                         \
                                                 })                                                                                                                     \
                                             );

#define setIntPointerProperty(OT, name, offset) OT->SetAccessorProperty(LITERAL(name),                                                                                     \
                                                    FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                        Isolate* isolate = info.GetIsolate();                                                                              \
                                                        ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());  \
                                                        info.GetReturnValue().Set(Number::New(isolate, *(int*)(base + offset)));                                           \
                                                    }),                                                                                                                    \
                                                    FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                        Isolate* isolate = info.GetIsolate();                                                                              \
                                                        ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());  \
                                                        int* prop = (int*)(base + offset);                                                                                 \
                                                        *prop = IntegerFI(info[0]);                                                                                        \
                                                    })                                                                                                                     \
                                                );

#define setBytePointerProperty(OT, name, offset) OT->SetAccessorProperty(LITERAL(name),                                                                                     \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     info.GetReturnValue().Set(Number::New(isolate, *(BYTE*)(base + offset)));                                         \
                                                 }),                                                                                                                    \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     BYTE* prop = (BYTE*)(base + offset);                                                                              \
                                                     *prop = IntegerFI(info[0]);                                                                                         \
                                                 })                                                                                                                     \
                                             );

#define setFloatPointerProperty(OT, name, offset) OT->SetAccessorProperty(LITERAL(name),                                                                                     \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
                                                     info.GetReturnValue().Set(Number::New(isolate, *(float*)(base + offset)));                                         \
                                                 }),                                                                                                                    \
                                                 FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {                                           \
                                                     Isolate* isolate = info.GetIsolate();                                                                              \
                                                     ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());      \
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
#define IntProperty(OT, name, offset) setIntPointerProperty(OT, name, offset)
#define ByteProperty(OT, name, offset) setBytePointerProperty(OT, name, offset)
#define FloatProperty(OT, name, offset) setFloatPointerProperty(OT, name, offset)

#define ConstProperty(OT, constname) OT->Set(isolate, #constname, Number::New(isolate, constname))

#define GlobalVariableFloatProperty(OT, name, offset) global->SetAccessorProperty(LITERAL(name), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) { \
                                                          using namespace v8;                                                                                                              \
                                                          Isolate* isolate = info.GetIsolate();                                                                                            \
                                                          info.GetReturnValue().Set(Number::New(isolate, *(float*)(ULONG_PTR(g_peggle) + offset)));                                        \
                                                      }), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {                                             \
                                                          using namespace v8;                                                                                                              \
                                                          Isolate* isolate = info.GetIsolate();                                                                                            \
                                                          *(float*)(ULONG_PTR(g_peggle) + offset) = FloatFI(info[0]);                                                                      \
                                                      }));

#define GlobalVariableProperty(OT, name, offset) global->SetAccessorProperty(LITERAL(name), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) { \
                                                          using namespace v8;                                                                                                              \
                                                          Isolate* isolate = info.GetIsolate();                                                                                            \
                                                          info.GetReturnValue().Set(Integer::New(isolate, *(DWORD*)(ULONG_PTR(g_peggle) + offset)));                                        \
                                                      }), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {                                             \
                                                          using namespace v8;                                                                                                              \
                                                          Isolate* isolate = info.GetIsolate();                                                                                            \
                                                          *(DWORD*)(ULONG_PTR(g_peggle) + offset) = IntegerFI(info[0]);                                                                      \
                                                      }));

#define GlobalVariableByteProperty(OT, name, offset) global->SetAccessorProperty(LITERAL(name), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) { \
                                                          using namespace v8;                                                                                                              \
                                                          Isolate* isolate = info.GetIsolate();                                                                                            \
                                                          info.GetReturnValue().Set(Integer::New(isolate, *(BYTE*)(ULONG_PTR(g_peggle) + offset)));                                        \
                                                      }), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {                                             \
                                                          using namespace v8;                                                                                                              \
                                                          Isolate* isolate = info.GetIsolate();                                                                                            \
                                                          *(BYTE*)(ULONG_PTR(g_peggle) + offset) = IntegerFI(info[0]);                                                                      \
                                                      }));

#define __UNTESTED __debugbreak()

#define ABILITY_SUPERGUIDE 1
#define ABILITY_FLIPPERS 2
//no count
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

//wait are these a bit field (yeah im not sure)
#define MOVEMENT_NONE 0
#define MOVEMENT_VERTICAL 1
#define MOVEMENT_HORIZONTAL 2
#define MOVEMENT_VERTICAL_HORIZONTAL 3
#define MOVEMENT_INFINITY 4
//uhh i forgot hold on
#define MOVEMENT_FIGURE_EIGHT 5
#define MOVEMENT_HORIZONTAL_SEMICIRCLE 6
#define MOVEMENT_VERTICAL_SEMICIRCLE 7
#define MOVEMENT_ROTATE_AT_ORIGIN 8
#define MOVEMENT_SMOOTH_ROTATE_AT_ORIGIN 9
#define MOVEMENT_VERTICAL_SNAP 11
#define MOVEMENT_HORIZONTAL_SNAP 12
#define MOVEMENT_ROTATE_AROUND_ORIGIN 13

#define LOGIC_EVENT_DEFAULT 1
#define LOGIC_EVENT_START_TURN 2
//if you set the current event to end turn without shooting the ball it will bring up the free ball coin
#define LOGIC_EVENT_END_TURN 3
#define LOGIC_EVENT_GRANT_FREE_BALL 4
#define LOGIC_EVENT_END_GAME 5
//uhh i thought this opened the character select menu but it seems to show the last pane thing that was on screen (including the tips that the masters give you)
#define LOGIC_EVENT_OPEN_PANE 8
//#define LOGIC_EVENT_CHARACTER_SELECT 9

//i found these in the steamapps/common/Peggle Deluxe/main.pak file (unzipped with quickbms 7x7m)
//specifically in the properties/resources.xml file within the pak
#define SOUND_MORNING 0
#define SOUND_PEGHIT_LOW 1
#define SOUND_PEGHIT 2
#define SOUND_PEGHIT2 3
#define SOUND_PEGHIT3 4
#define SOUND_EXTRABALL 5
#define SOUND_EXTRABALL2 6
#define SOUND_EXTRABALL3 7
#define SOUND_MISS 8
#define SOUND_COINSPIN 9
#define SOUND_COINSPIN_NO 10
#define SOUND_FREEBALL 11
#define SOUND_PENALTY 12
#define SOUND_LEVELWON 13
#define SOUND_LEVELDONE 14
#define SOUND_LEVELLOST 15
#define SOUND_POWERUP 16
#define SOUND_AAH 17
#define SOUND_MULTIBALL 18
#define SOUND_PEGPOP 19
#define SOUND_EXPLODE 20
#define SOUND_BUTTON1 21
#define SOUND_BUTTON2 22
#define SOUND_MOUSEOVER 23
#define SOUND_DING 24
#define SOUND_POINTBOOST 25
#define SOUND_TING 26
#define SOUND_AWARD 27
#define SOUND_APPLAUSE 28
#define SOUND_SKILLSHOT 29
#define SOUND_SPOOKYBALL2 30
#define SOUND_SPOOKYBALL3 31
#define SOUND_SPOOKYBALL4 32
#define SOUND_BUBBLES 33
#define SOUND_FLIP 34
#define SOUND_BUCKETHIT 35
#define SOUND_CANNONCOCK 36
#define SOUND_CANNONSHOT 37
#define SOUND_SCORECOUNTER 38
#define SOUND_FLIPPERUP 39
#define SOUND_FLIPPERDOWN 40
#define SOUND_FLIPPERBOUNCE 41
#define SOUND_RAINBOW 42
#define SOUND_FIREWORKS1 43
#define SOUND_FIREWORKS2 44
#define SOUND_RICHOCHET 45
#define SOUND_CYMBAL 46
#define SOUND_SIGH 47
#define SOUND_WOOSH 48
#define SOUND_TEXT_WOOSH 49
#define SOUND_WHISTLE 50
#define SOUND_FEVERHIT 51
#define SOUND_FIREBALLLOOP 52
#define SOUND_FIREBALLSHOT 53
#define SOUND_FIREBALLBOUNCE 54
#define SOUND_POWERUP_GUIDE 55
#define SOUND_POWERUP_MULTIBALL 56
#define SOUND_POWERUP_PYRAMID 57
#define SOUND_POWERUP_SPACEBLAST 58
#define SOUND_POWERUP_FLIPPERS 59
#define SOUND_POWERUP_SPOOKYBALL 60
#define SOUND_POWERUP_FLOWERPOWER 61
#define SOUND_POWERUP_LUCKYSPIN 62
#define SOUND_POWERUP_FIREBALL 63
#define SOUND_POWERUP_ZEN 64
#define SOUND_GONG 65
#define SOUND_SLOWMO 66
#define SOUND_TYPING 67
#define SOUND_TONELO 68
#define SOUND_TONE 69
#define SOUND_TONEHI 70
#define SOUND_TONESUPERHI 71
#define SOUND_STAMP 72
#define SOUND_ADD_BALL 73
#define SOUND_DIALOG_MOVE 74
#define SOUND_FIREWORKPOP 75

#define PHYS_HOLE 1
#define PHYS_BALL 2
#define PHYS_PEG 3
#define PHYS_LINE 4
#define PHYS_POLY 5
#define PHYS_BRICK 5

#define PEG_ANIMATION_SPECIAL 0
#define PEG_ANIMATION_HIT 1
#define PEG_ANIMATION_

//DWORD peggle_getAbilityOffset(DWORD ability) {
//    return ((2 * ability) * 4) + 0x1D4;
//}

void generic_set_dword(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    DWORD* idk = (DWORD*)(base + IntegerFI(info[0]));
    *idk = IntegerFI(info[1]);
}

void generic_get_dword(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    info.GetReturnValue().Set(Number::New(isolate, *(DWORD*)(base + IntegerFI(info[0]))));
}

void generic_set_float(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    float* idk = (float*)(base + IntegerFI(info[0]));
    *idk = FloatFI(info[1]);
}

void generic_get_float(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    info.GetReturnValue().Set(Number::New(isolate, *(float*)(base + IntegerFI(info[0]))));
}

/*void peggle_get_board(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using namespace v8;
    Isolate* isolate = info.GetIsolate();
    //there's a function that actually gets the board for you in the peggle binary but also g_logicmgr holds a pointer to a Board object at 0x104 bytes in

}*/

constexpr DWORD c_getAbilityOffset(DWORD ability) {
    return ((2 * ability) * 4) + 0x1D4;
}

const char* getObjectName(void* ptr) {
    //print("getObjectName " << ptr);
    ULONG_PTR* vtable = *(ULONG_PTR**)ptr; //wtf? g_eax? was that intentional because how has this been working
    if (vtable) {
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
        //char* name = orttip->pTypeDescriptor->name;
        //print(name);
        //__debugbreak();
        return orttip->pTypeDescriptor->name;
    }
    else {
        return "idk";
    }
}

namespace Templates {
    using namespace v8;

    Local<Object> getBallImpl(Isolate* isolate, void* ptr);
    Local<Object> getPolyImpl(Isolate* isolate, void* ptr);
    Local<Object> getBrickImpl(Isolate* isolate, void* ptr);
    Local<Object> getPegInfoImpl(Isolate* isolate, void* ptr);
    Local<Object> getFloatingTextImpl(Isolate* isolate, void* ptr);
    Local<Object> getHoleImpl(Isolate* isolate, void* ptr);
    Local<Object> getMoverImpl(Isolate* isolate, void* ptr);
    Local<Object> getEmbeddedTextImpl(Isolate* isolate, void* ptr);
    Local<Object> getLineImpl(Isolate* isolate, void* ptr);
    v8::Local<v8::Object> GetImplFromPtr(v8::Isolate* isolate, void* ptr);

    v8::Local<v8::ObjectTemplate> GenericPtrObject(v8::Isolate* isolate, void* ptr) {
        using namespace v8;
        Local<ObjectTemplate> ptr_object = ObjectTemplate::New(isolate);
        ptr_object->Set(isolate, "set_dword", FunctionTemplate::New(isolate, generic_set_dword));
        ptr_object->Set(isolate, "set_float", FunctionTemplate::New(isolate, generic_set_float));
        ptr_object->Set(isolate, "get_dword", FunctionTemplate::New(isolate, generic_get_dword));
        ptr_object->Set(isolate, "get_float", FunctionTemplate::New(isolate, generic_get_float));
        if (ptr != nullptr) {
            ptr_object->Set(isolate, "location", Number::New(isolate, (ULONG_PTR)ptr));
            //print("setting name " << ptr);
            ptr_object->Set(isolate, "name", String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        }
        //ptr_object->SetAccessorProperty(LITERAL("location"), Local<FunctionTemplate>(), Local<FunctionTemplate>(), PropertyAttribute::ReadOnly);
        //ptr_object->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        //    Isolate* isolate = info.GetIsolate();
        //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_logicmgr));
        //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);
        return ptr_object;
    }

    v8::Local<v8::ObjectTemplate> GenericPhysObject(v8::Isolate* isolate, void* ptr) {
        using namespace v8;
        //print("GenericPtrObject");
        Local<ObjectTemplate> ptr_object = GenericPtrObject(isolate, ptr); //possible issue?
        //print("AfterGPO");
        //Property(ptr_object, "threadStuff", 0x4); //prolly don't mess with this one
        Property(ptr_object, "refCount", 0x4); //#true
        Property(ptr_object, "type", 0x10); //just a theory but seems to be true
        ByteProperty(ptr_object, "collision", 0x24);
        ByteProperty(ptr_object, "visible", 0x25);
        ByteProperty(ptr_object, "enableMover", 0x26); //THE LEVEL TWISTED SISTERS HAS PEGS THAT HAVE DISABLED MOVERS! BOTH BUNCHES OF PEGS CAN SPIN!
        Property(ptr_object, "time", 0x4C);
        //sometimes PegInfo is null so we gotta check first
        ptr_object->SetAccessorProperty(LITERAL("pegInfo"), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR pegInfoPtr = *(ULONG_PTR*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0xD0);
            //peginfoptr is the value at +0xD0
            //print(pegInfoPtr);
            if (pegInfoPtr) {
                info.GetReturnValue().Set(Templates::getPegInfoImpl(isolate, (void*)pegInfoPtr));
            }
        }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR* pegInfoPtr = (ULONG_PTR*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0xD0);
            if (info[0]->IsNumber()) {
                *pegInfoPtr = IntegerFI(info[0]);
            }
            else if (info[0]->IsObject()) {
                *pegInfoPtr = IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            }
        }));
        ptr_object->Set(isolate, "setPegType", FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* thisObj = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            Peggle::setPegType(thisObj, IntegerFI(info[0]));
        }));
        ptr_object->SetAccessorProperty(LITERAL("mover"), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR moverPtr = *(ULONG_PTR*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0x48);
            //peginfoptr is the value at +0xD0
            //print(pegInfoPtr);
            if (moverPtr) {
                info.GetReturnValue().Set(Templates::getMoverImpl(isolate, (void*)moverPtr));
            }
        }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR* moverPtr = (ULONG_PTR*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0x48);
            if (info[0]->IsNumber()) {
                *moverPtr = IntegerFI(info[0]);
            }
            else if (info[0]->IsObject()) {
                *moverPtr = IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            }
        }));
        ptr_object->Set(isolate, "setVelocity", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* thisptr = (void*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()));
            Peggle::setPhysVelocity(thisptr, FloatFI(info[0]), FloatFI(info[1]));
        }));
        ptr_object->Set(isolate, "setPosition", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* thisptr = (void*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()));
            Peggle::setPhysPosition(thisptr, FloatFI(info[0]), FloatFI(info[1]));
        }));
        ptr_object->Set(isolate, "getPosition", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            Local<Context> context = isolate->GetCurrentContext();
            void* physObj = (void*)IntegerFI(info.This()->Get(context, LITERAL("location")).ToLocalChecked());
            float position[2]; Peggle::getPhysPosition(physObj, position);
            //__debugbreak();
            //print(position[0]);
            //print(position[1]);
            Local<Object> jsPos = Object::New(isolate);
            jsPos->Set(context, LITERAL("x"), Number::New(isolate, position[0]));
            jsPos->Set(context, LITERAL("y"), Number::New(isolate, position[1]));
            info.GetReturnValue().Set(jsPos);
        }));
        ptr_object->Set(isolate, "getX", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* physObj = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            info.GetReturnValue().Set(Number::New(isolate, Peggle::getPhysX(physObj)));
        }));
        ptr_object->Set(isolate, "getY", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* physObj = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            info.GetReturnValue().Set(Number::New(isolate, Peggle::getPhysY(physObj)));
        }));
        ptr_object->Set(isolate, "setActive", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* physObj = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            Peggle::setPhysActive(physObj, IntegerFI(info[0]));
        }));
        //PhysObj+5C is an embeddedText object
        //PhysObj+78 is an embeddedText object
        //PhysObj+94 is an embeddedText object
        ptr_object->SetAccessorProperty(LITERAL("imgname"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR physObj = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            EmbeddedText* imgname = (EmbeddedText*)(physObj + 0x94);
            const char* text = nullptr;
            //__debugbreak();
            if (imgname->maxLength < 0x10) {
                text = (const char*) &imgname->text;
            }
            else {
                text = (const char*) imgname->text;
            }
            info.GetReturnValue().Set(String::NewFromUtf8(isolate, text).ToLocalChecked());
            //info.GetReturnValue().Set(Templates::getEmbeddedTextImpl(isolate, imgname));
        }), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR physObj = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            EmbeddedText* imgname = (EmbeddedText*)(physObj + 0x94);
            Local<String> jsstr = info[0].As<String>();
            char* stackstr = v8helper2::allocateStackStr(jsstr);
            Peggle::setEmbeddedText((void*)imgname, stackstr, jsstr->Length());
        }));
        ptr_object->Set(isolate, "loadImageIfValidPath", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* physObj = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            info.GetReturnValue().Set(Peggle::loadImgIfImgName(physObj));
        }));
        //ptr_object->SetAccessorProperty(LITERAL("location"), Local<FunctionTemplate>(), Local<FunctionTemplate>(), PropertyAttribute::ReadOnly);
        //ptr_object->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        //    Isolate* isolate = info.GetIsolate();
        //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_logicmgr));
        //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);
        return ptr_object;
    }

    v8::Local<v8::ObjectTemplate> PolyObject(v8::Isolate* isolate, void* ptr) {
        using namespace v8;
        //print("GenericPtrObject");
        Local<ObjectTemplate> poly_object = GenericPhysObject(isolate, ptr);
        FloatProperty(poly_object, "x", 0x114); //hmm when the hell did i find this, it could be right (offset 00081740 GetY(?))
        FloatProperty(poly_object, "y", 0x118);
        FloatProperty(poly_object, "rotation", 0x11C); //speculation

        //i THINK that's what this function does?
        //in radians! (if it works?)
        poly_object->Set(isolate, "setRotation", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* poly = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            Peggle::setPolyRotation(poly, FloatFI(info[0]));
        }));
        
        return poly_object;
    }

    Local<ObjectTemplate> createEmbeddedTextTemplate(v8::Isolate* isolate);

    Local<ObjectTemplate> _Ball;
    Local<ObjectTemplate> _Poly;
    Local<ObjectTemplate> _Brick;
    Local<ObjectTemplate> _PegInfo;
    Local<ObjectTemplate> _FloatingText;
    Local<ObjectTemplate> _Hole;
    Local<ObjectTemplate> _Mover;
    Local<ObjectTemplate> _EmbeddedText;
    Local<ObjectTemplate> _Line;
    void Init(v8::Isolate* isolate) {
        //i think ball and brick have some class in common where both Ball and Brick share a pointer to a PegInfo object at 0xD0
        _Ball = GenericPhysObject(isolate, nullptr);
        ByteProperty(_Ball, "multiball", 0xE8); //idk what it does exactly
        FloatProperty(_Ball, "x", 0xEC); //YES!
        FloatProperty(_Ball, "y", 0xF0); //this is the real ball location!
        //maybe 0xF4 is last psition? idk 
        //0xF4 (ball X), 0xF8 (ball Y), 0x104 (ball X), and 0x108 (ball Y) are floats and related to its position in some way
        //also 10C holds x position and 110 holds y posituion?! what is happening
        FloatProperty(_Ball, "velX", 0xFC); //with me randomly guessing i got it
        FloatProperty(_Ball, "velY", 0x100);
        FloatProperty(_Ball, "drawX", 0x134); //honestly i don't know yet lol
        FloatProperty(_Ball, "drawY", 0x138); //yeah idk it seems like it might be where it draws it at?
        FloatProperty(_Ball, "radius", 0x13C); //WHY THE FUCK IS THERE A RADIUS PROPERTY AND WHY DOES IT ACTUALLY WORK?!
        ByteProperty(_Ball, "anchored", 0x140); //haha like roblox :)
        ByteProperty(_Ball, "fireball", 0x161);
        FloatProperty(_Ball, "idk", 0x170); //not sure.

        _Ball->Set(isolate, "destroy", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* base = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            //probably don't try to access this memory anymore if you pass true
            Peggle::destroyBall(base, IntegerFI(info[0]));
        }));

        _Ball->Set(isolate, "method", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* base = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            void* other = nullptr;
            if (info[0]->IsNumber()) {
                other = (void*)IntegerFI(info[0]);
            }
            else if (info[0]->IsObject()) {
                other = (void*)IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            }
            info.GetReturnValue().Set(Integer::New(isolate, Peggle::randomBallMethod(base, other)));
        }));

        _Poly = PolyObject(isolate, nullptr);

        _Brick = PolyObject(isolate, nullptr); //brick extends poly!
        //Property(_Brick, "threadStuff", 0x4); //prolly don't mess with this one
        //FloatProperty(_Brick, "x", 0xEC); //honestly we're gonna assume the x and y are provided by PhysObject (do not assume that i just crashed my shit)
        //FloatProperty(_Brick, "y", 0xF0);
        //Brick+0xEC actually seemed to be a pointer to an array of floats so i might try changing that lol
        //Brick+0xF0 also seems to point to that same area but its a slightly different location so imma just start changing shit

        //FloatProperty(_Brick, "maybeX", 0x114); //hmm when the hell did i find this, it could be right (offset 00081740 GetY(?))
        //FloatProperty(_Brick, "maybeY", 0x118);

        _PegInfo = GenericPtrObject(isolate, nullptr);
        Property(_PegInfo, "refCount", 0x4);
        Property(_PegInfo, "type", 0x10);
        //Property(_PegInfo, "hit", 0x14); //boolean
        ByteProperty(_PegInfo, "hit", 0x14);
        Property(_PegInfo, "timeSinceHit", 0x18); //for some reason there's 2 (oh that was the animation percent)
        Property(_PegInfo, "hitCount", 0x1C); //hmm

        //what a terrible name but this might be the best way to put it
        //it's the length of time for which this peg was the last one the ball hit
        //hmm it could also be the animation frame but the peg sparkle effects have a different amount of frames so wouldn't that mean the ones with more frames appear to be faster? 
        //it actually might be my earlier guesses might've been wrong!
        //Property(_PegInfo, "lengthOfTimeThisPegWasLastHit", 0x20); //for some reason there's 2 (oh wait) (ah something special about this one is that it doesn't go above 100 because im assuming something happens when you do (the pegs prolly disappear (wrong!)))
        
        Property(_PegInfo, "animationPercent", 0x20); //confirmed! if you set this value to anything between 1-100 it will play the peg hit animation
        Property(_PegInfo, "animationId", 0x24); //possibly confirmed
        _PegInfo->Set(isolate, "setHit", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* peginfo = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            Peggle::setHit(peginfo, IntegerFI(info[0]));
        }));
        _PegInfo->Set(isolate, "playPegAnimation", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* peginfo = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            Peggle::playPegAnimation(peginfo, IntegerFI(info[0]));
        }));
        _PegInfo->Set(isolate, "lightUpPeg", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = info.GetIsolate();
            void* peginfo = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            Peggle::lightUpPeg(peginfo, IntegerFI(info[0]));
        }));

        _FloatingText = GenericPtrObject(isolate, nullptr);
        //uhhh in the showHoveringScoreOverPeg function they set FloatingText+5C to 0x2D (45 idk what that is for)
        //FloatingText+0x60 is 0x00FFFF00 (color?)
        //FloatingText+0x68 is 0x64 (lifeTime)
        //FloatingText+0x78 is later set to A (like the number 10?!) (yeah still don't know)
        //FloatingText+0x7C is 1 (idk)
        
        //nevermind im pretty sure if i change this it might crash lol
        //they use FloatingText+0x5C with another function that returns some graphics related object (if 0x2D is passed, it returns an ImageFont object)
        //Property(_FloatingText, "changethis", 0x5C); //idk yet
        
        Property(_FloatingText, "id", 0x58);

        //damn that shit actually was a coincidence -> nothing happened when i changed it :(
        //hmmm nah this gotta be color
        //i knew it
        //ok idk why it didn't work when i changed it last time but this actually is the color (maybe you just have to change it the moment you make the floating text but idk)
        Property(_FloatingText, "color", 0x60); //honestly this is just a theory but most FloatingText objects are yellow and this value is normally set to 0xFFFF00 (coincidence? i think not.)
        
        Property(_FloatingText, "lifeTime", 0x68);
        Property(_FloatingText, "elapsedTime", 0x6C);
        Property(_FloatingText, "number", 0x78); //maybe? (prolly not :( )
        Property(_FloatingText, "something", 0x7C); //when you set this higher than 3, the text just disappears (idk what it does still)
        //if the followingObj property is not nullptr, it will call member functions on the object to get its x and y positions and then uses them to move the FloatingText
        _FloatingText->SetAccessorProperty(LITERAL("followingObj"), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR followingObjPtr = *(ULONG_PTR*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0xB4);
            //peginfoptr is the value at +0xD0
            //print(pegInfoPtr);
            if (followingObjPtr) {
                info.GetReturnValue().Set(Templates::GetImplFromPtr(isolate, (void*)followingObjPtr));
            }
        }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR* followingObjPtr = (ULONG_PTR*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0xB4);
            if (info[0]->IsNumber()) {
                *followingObjPtr = IntegerFI(info[0]);
            }
            else if (info[0]->IsObject()) {
                *followingObjPtr = IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            }
        }));

        FloatProperty(_FloatingText, "lastX", 0xB8); //#true
        FloatProperty(_FloatingText, "lastY", 0xBC); //#true
        FloatProperty(_FloatingText, "x", 0x80); //i mean im pretty sure #true
        FloatProperty(_FloatingText, "y", 0x84); //#true
        //0x88 and 0x8C could offsets or velocity?
        FloatProperty(_FloatingText, "velX", 0x88); //#true
        FloatProperty(_FloatingText, "velY", 0x8C); //#true
        FloatProperty(_FloatingText, "idk", 0x90); //lowkey the only thing i know is it's a float
        FloatProperty(_FloatingText, "rotation", 0x94); //rotation?
        FloatProperty(_FloatingText, "velRotation", 0x98); //maybe rotation velocity
        //FloatProperty(_FloatingText, "something", 0x8C); //is like -0.1
        _FloatingText->SetAccessorProperty(LITERAL("text"), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            //uhh idk how to get the text yet lol it gets kinda deep in the graphics side
            //oh wait maybe it's not as crazy as i thoguht
            EmbeddedText* embedded = *(EmbeddedText**)(base+0xC);
            const char* text = nullptr;
            //__debugbreak();
            if (embedded->maxLength < 0x10) {
                text = (const char*) &embedded->text;
            }
            else {
                text = (const char*) embedded->text;
            }
            info.GetReturnValue().Set(String::NewFromUtf8(isolate, text).ToLocalChecked());
        }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            EmbeddedText textObj = EmbeddedText();
            Local<String> jsstr = info[0].As<String>();
            char* stackstr = v8helper2::allocateStackStr(jsstr);
            Peggle::setEmbeddedText((void*)&textObj, stackstr, jsstr->Length());
            Peggle::setFloatingTextText((void*)base, (void*)&textObj);
        }));

        _Hole = GenericPhysObject(isolate, nullptr); //assuming generic phys lol
        //FloatProperty(_Hole, "velX", 0x14); //not sure (this CANNOT be right)
        //FloatProperty(_Hole, "velY", 0x18);
        //Property(_Hole, "time", 0x4C); //also not sure (i've moved the time property into the GenericPhysObject itself because i think they all have it)
        
        _Mover = GenericPtrObject(isolate, nullptr);
        Property(_Mover, "refCount", 0x4);
        IntProperty(_Mover, "type", 0x10); //confirmed
        IntProperty(_Mover, "offset", 0x14); //another offset?
        //Property(_Mover, "somethingtime", 0x18); //hmm idk how i thought this might be related to time this seems like amplitude
        IntProperty(_Mover, "amplitude", 0x18); //confirmed!
        Property(_Mover, "speedDivisor", 0x1C); //confirmed! related to the speed of the movement
        FloatProperty(_Mover, "timingOffset", 0x20); //confirmed
        FloatProperty(_Mover, "optionalVerticalAmplitude", 0x28); //optional
        FloatProperty(_Mover, "x", 0x54); //oh yeah confirmed (but nothing happens when you change these values as their calculated every frame)
        FloatProperty(_Mover, "y", 0x58); //oh yeah confirmed (but nothing happens when you change these values as their calculated every frame)
        FloatProperty(_Mover, "originX", 0x5C); //oh yeah confirmed
        FloatProperty(_Mover, "originY", 0x60); //confirmed

        _EmbeddedText = createEmbeddedTextTemplate(isolate);

        _Line = GenericPhysObject(isolate, nullptr);
        FloatProperty(_Line, "drawLeft", 0xEC);
        FloatProperty(_Line, "drawRight", 0xF0);
        FloatProperty(_Line, "drawTop", 0xF4);
        FloatProperty(_Line, "drawBottom", 0xF8);
    }

    Local<ObjectTemplate> createEmbeddedTextTemplate(Isolate* isolate) {
        Local<ObjectTemplate> templa = GenericPtrObject(isolate, nullptr);
        Property(templa, "textLength", 0x14);
        Property(templa, "maxLength", 0x18); //assuming lol

        templa->SetAccessorProperty(LITERAL("text"),
            FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
                //getter
                Isolate* isolate = info.GetIsolate();
                ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
                Local<String> str;
                if (String::NewFromUtf8(isolate, (const char*)(base+0x4)).ToLocal(&str)) {
                    info.GetReturnValue().Set(str);
                }
                else {
                    print("something went wrong trying to read the string");
                }
            }),
            FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
                //setter
                Isolate* isolate = info.GetIsolate();
                ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
                //hold on there's an actual function for this!
                //im just gonna allocate the string length right here using ALLOCA!!!
                Local<String> jsstr = info[0].As<String>();

                //char* stackstr = (char*)_alloca(jsstr->Length()+1); //this is awesome (don't let the haters tell you otherwise bruh)
                //strcpy(stackstr, CStringFI(jsstr)); //almost didn't allocate space for the null terminator i would've had the worst bug ever

                //don't worry my boy i've made it an inline function
                char* stackstr = v8helper2::allocateStackStr(jsstr);

                Peggle::setEmbeddedText((void*)base, stackstr, jsstr->Length());
                //char* text = (char*)(base + 0x4);
                //memcpy(text, CStringFI(info[0]), str->Length());
                //strcpy(text, CStringFI(str)); //also copies null terminator
            })
        );
        return templa;
    }

    Local<Object> getBallImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _Ball->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        return object;
    }

    Local<Object> getPolyImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _Poly->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        return object;
    }

    Local<Object> getBrickImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _Brick->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        return object;
    }

    Local<Object> getPegInfoImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _PegInfo->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        return object;
    }

    Local<Object> getFloatingTextImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _FloatingText->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        return object;
    }

    Local<Object> getHoleImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _Hole->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        return object;
    }
    
    Local<Object> getMoverImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _Mover->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        return object;
    }

    Local<Object> getEmbeddedTextImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _EmbeddedText->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        //object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked()); //no vtable
        return object;
    }

    Local<Object> getLineImpl(Isolate* isolate, void* ptr) {
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> object = _Line->NewInstance(context).ToLocalChecked();
        object->Set(context, LITERAL("location"), Number::New(isolate, (LONG_PTR)ptr));
        object->Set(context, LITERAL("name"), String::NewFromUtf8(isolate, getObjectName(ptr)).ToLocalChecked());
        return object;
    }

    v8::Local<v8::Object> GetImplFromPtr(v8::Isolate* isolate, void* ptr) {
        using namespace v8;

        if (ptr == nullptr) {
            __debugbreak(); //uh oh something weird is happening!
        }

        const char* name = getObjectName(ptr);
        //print(name << " GetMimplfromptr");

        Local<Object> obj;
        if (strcmp(".?AVBall@Sexy@@", name) == 0) {
            obj = getBallImpl(isolate, ptr);
        }
        else if (strcmp(".?AVPoly@Sexy@@", name) == 0) {
            obj = getPolyImpl(isolate, ptr);
        }
        else if (strcmp(".?AVBrick@Sexy@@", name) == 0) {
            obj = getBrickImpl(isolate, ptr);
        }
        else if (strcmp(".?AVPegInfo@Sexy@@", name) == 0) {
            obj = getPegInfoImpl(isolate, ptr);
        }
        else if (strcmp(".?AVFloatingText@Sexy@@", name) == 0) {
            obj = getFloatingTextImpl(isolate, ptr);
        }
        else if (strcmp(".?AVHole@Sexy@@", name) == 0) {
            obj = getHoleImpl(isolate, ptr);
        }
        else if (strcmp(".?AVMover@Sexy@@", name) == 0) {
            obj = getMoverImpl(isolate, ptr);
        }
        else if (strcmp(".?AVLine@Sexy@@", name) == 0) {
            obj = getLineImpl(isolate, ptr);
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
            if (info[1]->BooleanValue(isolate)) { //if the second parameter is true then we'll tell the main thread to wait longer
                v8helper2::tellWaitingThreadsToWaitLonger();
            }
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
            if (info[1]->BooleanValue(isolate)) { //if the second parameter is true then we'll tell the main thread to wait longer
                v8helper2::tellWaitingThreadsToWaitLonger();
            }
            //print("running peg hit");
            //nah i gotta check the types of these bruh
            Local<Object> ball1 = Templates::GetImplFromPtr(isolate, (void*)g_eax); //Templates::getBallImpl(isolate, (void*)g_eax);
            Local<Object> ball2 = Templates::GetImplFromPtr(isolate, (void*)g_ecx);

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
            if (info[1]->BooleanValue(isolate)) { //if the second parameter is true then we'll tell the main thread to wait longer
                v8helper2::tellWaitingThreadsToWaitLonger();
            }
            Local<Value> args[] = { Integer::New(isolate, g_eax), Integer::New(isolate, g_ecx), Integer::New(isolate, g_edx) };
            //print("running on keydow " << sizeof(args) << " " << ARRAYSIZE(args)); //what the hell is sizeof doing (oops it returns the size in bytes not elements!)
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

    print("init globals");

    board = GetBoard();
    logicMgr = GetLogicMgrFromBoard(board);
    //g_ball = TryGetBallFromBoard(g_board);

    Templates::Init(isolate);

    //print("tempalkte inites");

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

    Property(LogicMgr, "currentEvent", 0x4); //what the hell? changing this value forces seemingly random things to happen (1 does nothing, 2 automatically skips to 3)
    Property(LogicMgr, "eventTime", 0x8); //idk what the purpose of this one is
    Property(LogicMgr, "specialsomething", 0x20); //hmm this doesn't seem to change alot
    //ByteProperty(LogicMgr, "fastforward", 0x59); //i thinkj? (wait no this was for board lol)
    Property(LogicMgr, "relatedX", 0xA0);
    Property(LogicMgr, "relatedY", 0xA4);
    FloatProperty(LogicMgr, "gunRotation", 0xE0); //in radians
    Property(LogicMgr, "nudgeTimeout", 0xE8); //this number becomes non-zero after you nudge the gun with the arrows or scrolling (while this value is non-zero it disables your ability to move the gun until it decrements itself to 0)
    ByteProperty(LogicMgr, "shootBall", 0xEC); //booleans
    ByteProperty(LogicMgr, "playSoundAndShootBall", 0xED);
    Property(LogicMgr, "mouseX", 0x98); //hmm these are correct but it will randomly decide to discard these with a random const 0xFFFFD8F0
    Property(LogicMgr, "mouseY", 0x9C);
    Property(LogicMgr, "multipliedPointsThisTurn", 0x108); //YO LET'S GOT THIS HAS TO BE RIGHT!
    Property(LogicMgr, "originalPointsThisTurn", 0x10C); //also special in some weay
    Property(LogicMgr, "orangeMultiplier", 0x114); //assuming
    Property(LogicMgr, "pegsHit", 0x120);
    Property(LogicMgr, "turnOffset", 0x128);
    Property(LogicMgr, "gunReloadAnimationFrame", 0x138); //i assume

    ////Property(LogicMgr, "specialsomething2", 0x170); //lmao for some reason this is a pointer to the purple peg
    LogicMgr->SetAccessorProperty(LITERAL("purplePeg"), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
        Isolate* isolate = info.GetIsolate();
        ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        ULONG_PTR purplePegPtr = *(ULONG_PTR*)(base + 0x170);
        if (purplePegPtr) {
            info.GetReturnValue().Set(Templates::GetImplFromPtr(isolate, (void*)purplePegPtr)); //what a handy function
        }
    }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) { //just in case?
        Isolate* isolate = info.GetIsolate();
        ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        ULONG_PTR* purplePegPtr = (ULONG_PTR*)(base + 0x170);
        if (info[0]->IsNumber()) {
            *purplePegPtr = IntegerFI(info[0]);
        }
        else if (info[0]->IsObject()) {
            *purplePegPtr = IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        }
    }));
    Property(LogicMgr, "score", 0x174);
    Property(LogicMgr, "balls", 0x17C);

    Property(LogicMgr, "currentMaster", 0x184); //confirmed but it only changes which character shows up in the gun and not the ability (and it only takes effect after a turn)

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
    Property(LogicMgr, "freeBallsGivenByPoints", 0x258); //oops what was it? (fixed)

    Property(LogicMgr, "idkyet", 0xF0);
    ByteProperty(LogicMgr, "replay", 0xF5);
    ByteProperty(LogicMgr, "won", 0xF6);
    ByteProperty(LogicMgr, "checkshit", 0xFB); //idk yet lol

    FloatProperty(LogicMgr, "pegX", 0x330); //not sure what for yet but thats what these seem to be
    FloatProperty(LogicMgr, "pegY", 0x334);
    //0x338 and 0x33C also store peg x and y at offset 0x00067B4A

    Property(LogicMgr, "pegsLeft", 0x348)
    Property(LogicMgr, "pegsLeftToPop", 0x354); //changed when the turn is over (idk about this one)
    //oh these are actually the length of the orange peg and blue peg arrays at the address-4
    Property(LogicMgr, "orangePegsLeft", 0x360); //yo this might be the amount of orange pegs left...
    Property(LogicMgr, "bluePegsLeft", 0x36C);
    //Property(LogicMgr, "uniquePegsHit", 0x12C); //amount of pegs hit or SOMETHING?
    //Property(LogicMgr, "uniquePegsHit", 0x354); //um im pretty sure they both hold the exact same value (but technically this might be a different object) (nah they ain' tthe same)
    Property(LogicMgr, "pegsHitThisTurn", 0x12C);

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

    ConstProperty(global, MOVEMENT_NONE);
    ConstProperty(global, MOVEMENT_VERTICAL);
    ConstProperty(global, MOVEMENT_HORIZONTAL);
    ConstProperty(global, MOVEMENT_VERTICAL_HORIZONTAL);
    ConstProperty(global, MOVEMENT_INFINITY);
    ConstProperty(global, MOVEMENT_FIGURE_EIGHT);
    ConstProperty(global, MOVEMENT_HORIZONTAL_SEMICIRCLE);
    ConstProperty(global, MOVEMENT_VERTICAL_SEMICIRCLE);
    ConstProperty(global, MOVEMENT_ROTATE_AT_ORIGIN);
    ConstProperty(global, MOVEMENT_SMOOTH_ROTATE_AT_ORIGIN);
    ConstProperty(global, MOVEMENT_VERTICAL_SNAP);
    ConstProperty(global, MOVEMENT_HORIZONTAL_SNAP);
    ConstProperty(global, MOVEMENT_ROTATE_AROUND_ORIGIN);

    ConstProperty(global, LOGIC_EVENT_DEFAULT);
    ConstProperty(global, LOGIC_EVENT_START_TURN);
    ConstProperty(global, LOGIC_EVENT_END_TURN);
    ConstProperty(global, LOGIC_EVENT_GRANT_FREE_BALL);
    ConstProperty(global, LOGIC_EVENT_END_GAME);
    //...
    ConstProperty(global, LOGIC_EVENT_OPEN_PANE);

    ConstProperty(global, SOUND_MORNING);
    ConstProperty(global, SOUND_PEGHIT_LOW);
    ConstProperty(global, SOUND_PEGHIT);
    ConstProperty(global, SOUND_PEGHIT2);
    ConstProperty(global, SOUND_PEGHIT3);
    ConstProperty(global, SOUND_EXTRABALL);
    ConstProperty(global, SOUND_EXTRABALL2);
    ConstProperty(global, SOUND_EXTRABALL3);
    ConstProperty(global, SOUND_MISS);
    ConstProperty(global, SOUND_COINSPIN);
    ConstProperty(global, SOUND_COINSPIN_NO);
    ConstProperty(global, SOUND_FREEBALL);
    ConstProperty(global, SOUND_PENALTY);
    ConstProperty(global, SOUND_LEVELWON);
    ConstProperty(global, SOUND_LEVELDONE);
    ConstProperty(global, SOUND_LEVELLOST);
    ConstProperty(global, SOUND_POWERUP);
    ConstProperty(global, SOUND_AAH);
    ConstProperty(global, SOUND_MULTIBALL);
    ConstProperty(global, SOUND_PEGPOP);
    ConstProperty(global, SOUND_EXPLODE);
    ConstProperty(global, SOUND_BUTTON1);
    ConstProperty(global, SOUND_BUTTON2);
    ConstProperty(global, SOUND_MOUSEOVER);
    ConstProperty(global, SOUND_DING);
    ConstProperty(global, SOUND_POINTBOOST);
    ConstProperty(global, SOUND_TING);
    ConstProperty(global, SOUND_AWARD);
    ConstProperty(global, SOUND_APPLAUSE);
    ConstProperty(global, SOUND_SKILLSHOT);
    ConstProperty(global, SOUND_SPOOKYBALL2);
    ConstProperty(global, SOUND_SPOOKYBALL3);
    ConstProperty(global, SOUND_SPOOKYBALL4);
    ConstProperty(global, SOUND_BUBBLES);
    ConstProperty(global, SOUND_FLIP);
    ConstProperty(global, SOUND_BUCKETHIT);
    ConstProperty(global, SOUND_CANNONCOCK);
    ConstProperty(global, SOUND_CANNONSHOT);
    ConstProperty(global, SOUND_SCORECOUNTER);
    ConstProperty(global, SOUND_FLIPPERUP);
    ConstProperty(global, SOUND_FLIPPERDOWN);
    ConstProperty(global, SOUND_FLIPPERBOUNCE);
    ConstProperty(global, SOUND_RAINBOW);
    ConstProperty(global, SOUND_FIREWORKS1);
    ConstProperty(global, SOUND_FIREWORKS2);
    ConstProperty(global, SOUND_RICHOCHET);
    ConstProperty(global, SOUND_CYMBAL);
    ConstProperty(global, SOUND_SIGH);
    ConstProperty(global, SOUND_WOOSH);
    ConstProperty(global, SOUND_TEXT_WOOSH);
    ConstProperty(global, SOUND_WHISTLE);
    ConstProperty(global, SOUND_FEVERHIT);
    ConstProperty(global, SOUND_FIREBALLLOOP);
    ConstProperty(global, SOUND_FIREBALLSHOT);
    ConstProperty(global, SOUND_FIREBALLBOUNCE);
    ConstProperty(global, SOUND_POWERUP_GUIDE);
    ConstProperty(global, SOUND_POWERUP_MULTIBALL);
    ConstProperty(global, SOUND_POWERUP_PYRAMID);
    ConstProperty(global, SOUND_POWERUP_SPACEBLAST);
    ConstProperty(global, SOUND_POWERUP_FLIPPERS);
    ConstProperty(global, SOUND_POWERUP_SPOOKYBALL);
    ConstProperty(global, SOUND_POWERUP_FLOWERPOWER);
    ConstProperty(global, SOUND_POWERUP_LUCKYSPIN);
    ConstProperty(global, SOUND_POWERUP_FIREBALL);
    ConstProperty(global, SOUND_POWERUP_ZEN);
    ConstProperty(global, SOUND_GONG);
    ConstProperty(global, SOUND_SLOWMO);
    ConstProperty(global, SOUND_TYPING);
    ConstProperty(global, SOUND_TONELO);
    ConstProperty(global, SOUND_TONE);
    ConstProperty(global, SOUND_TONEHI);
    ConstProperty(global, SOUND_TONESUPERHI);
    ConstProperty(global, SOUND_STAMP);
    ConstProperty(global, SOUND_ADD_BALL);
    ConstProperty(global, SOUND_DIALOG_MOVE);
    ConstProperty(global, SOUND_FIREWORKPOP);

    ConstProperty(global, PHYS_HOLE);
    ConstProperty(global, PHYS_BALL);
    ConstProperty(global, PHYS_PEG);
    ConstProperty(global, PHYS_LINE);
    ConstProperty(global, PHYS_POLY);
    ConstProperty(global, PHYS_BRICK);

    //surprisingly yes it actually makes a copy!
    //Local<ObjectTemplate> isthisacopy = ObjectTemplate::New(isolate);
    //isthisacopy->Set(isolate, "my boy", LITERAL("2 sigma hon funk"));
    //isthisacopy->Set(isolate, "bussin", Boolean::New(isolate, true));
    //
    //LogicMgr->Set(isolate, "help1", isthisacopy);
    //LogicMgr->Set(isolate, "help2", isthisacopy);

    /*Local<ObjectTemplate> yellowTextObj = Templates::getEmbeddedTextImpl(isolate, (void*)((ULONG_PTR)logicMgr + 0x7C)); //passing nullptr here because getObjectName will fail because this object has no 
    LogicMgr->Set(isolate, "yellowTextObj", yellowTextObj);
    //Property(LogicMgr, "textLength", 0x90);

    Local<ObjectTemplate> idkTextObj = Templates::getEmbeddedTextImpl(isolate, (void*)((ULONG_PTR)logicMgr + 0x18C));
    LogicMgr->Set(isolate, "idkTextObj", idkTextObj);*/

    Local<ObjectTemplate> usernameTextObj = Templates::createEmbeddedTextTemplate(isolate);
    usernameTextObj->Set(isolate, "location", Integer::New(isolate, ULONG_PTR(logicMgr) + 0x18C));

    LogicMgr->Set(isolate, "usernameTextObj", usernameTextObj);

    //Local<ObjectTemplate> yellowTextObj = Templates::_EmbeddedText; //hopefully a copy? (it wasn't)
    Local<ObjectTemplate> yellowTextObj = Templates::createEmbeddedTextTemplate(isolate);
    yellowTextObj->Set(isolate, "location", Integer::New(isolate, ULONG_PTR(logicMgr) + 0x7C));

    LogicMgr->Set(isolate, "yellowTextObj", yellowTextObj); //copy!

    /*Local<ObjectTemplate> yellowTextObj = Templates::GenericPtrObject(isolate, nullptr);
    yellowTextObj->Set(isolate, "location", Number::New(isolate, ULONG_PTR(logicMgr) + 0x7C));
    Property(yellowTextObj, "textLength", 0x14);
    Property(yellowTextObj, "maxLength", 0x18); //assuming lol

    yellowTextObj->SetAccessorProperty(LITERAL("text"),
        FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            //getter
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            Local<String> str;
            if (String::NewFromUtf8(isolate, (const char*)(base+0x4)).ToLocal(&str)) {
                info.GetReturnValue().Set(str);
            }
            else {
                print("something went wrong reading peggle.text");
            }
        }),
        FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {
            //setter
            Isolate* isolate = info.GetIsolate();
            ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
            //hold on there's an actual function for this!
            //im just gonna allocate the string length right here using ALLOCA!!!
            Local<String> jsstr = info[0].As<String>();

            //char* stackstr = (char*)_alloca(jsstr->Length()+1); //this is awesome (don't let the haters tell you otherwise bruh)
            //strcpy(stackstr, CStringFI(jsstr)); //almost didn't allocate space for the null terminator i would've had the worst bug ever

            //don't worry my boy i've made it an inline function
            char* stackstr = v8helper2::allocateStackStr(jsstr);

            Peggle::setEmbeddedText((void*)base, stackstr, jsstr->Length());
            //char* text = (char*)(base + 0x4);
            //memcpy(text, CStringFI(info[0]), str->Length());
            //strcpy(text, CStringFI(str)); //also copies null terminator
        })
    );
    LogicMgr->Set(isolate, "yellowTextObj", yellowTextObj);*/
    
    //LogicMgr->SetAccessorProperty(LITERAL("location"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    info.GetReturnValue().Set(Number::New(isolate, (ULONG_PTR)g_logicmgr));
    //}), Local<FunctionTemplate>(), v8::PropertyAttribute::ReadOnly);
    //
    //LogicMgr->Set(isolate, "set_dword", FunctionTemplate::New(isolate, generic_set_dword));
    //LogicMgr->Set(isolate, "get_dword", FunctionTemplate::New(isolate, generic_get_dword));
    LogicMgr->Set(isolate, "activateAbility", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();

        //lowkey you should be passing your own parameters because idk what this even points to
        //DWORD unknown1 = 0x005F19B4;
        void* ball = TryGetBallFromBoard(Peggle::board);

        void* physObj2 = ball; //i wonder what that would do

        if (info[0]->IsNumber()) {
            ball = (void*)IntegerFI(info[0]);
        }
        else if (info[0]->IsObject()) {
            ball = (void*)IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        }
        if (info[1]->IsNumber()) {
            physObj2 = (void*)IntegerFI(info[1]);
        }
        else if (info[1]->IsObject()) {
            physObj2 = (void*)IntegerFI(info[1].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        }

        DWORD ability = 0; //IntegerFI(info[2]);
        if (info[2]->BooleanValue(isolate)) {
            ability = IntegerFI(info[2]);
        }


        BOOL isAbilitySpecified = FALSE;

        if (info[3]->BooleanValue(isolate)) {
            isAbilitySpecified = IntegerFI(info[3]);
        }

        Peggle::activateAbility(ball, physObj2, ability, isAbilitySpecified);
    }));
    //LogicMgr->Set(isolate, "click", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    Peggle::clickOnBoard(IntegerFI(info[0]), IntegerFI(info[1]), IntegerFI(info[2]));
    //}));
    LogicMgr->Set(isolate, "addBalls", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Peggle::addBalls(IntegerFI(info[0]), IntegerFI(info[1]), IntegerFI(info[2]));
    }));
    LogicMgr->Set(isolate, "spawnFloatingText", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Local<String> str = info[0].As<String>();
        float x = FloatFI(info[1]);
        float y = FloatFI(info[2]);
        //print(x);
        //print(y);
        EmbeddedText textObj = EmbeddedText();

        //just in case, im copying the string onto the stack just like the other time i use set embedded text
        //char* stackstr = (char*)_alloca(str->Length() + 1); //this is awesome (don't let the haters tell you otherwise bruh)
        //strcpy(stackstr, CStringFI(str)); //almost didn't allocate space for the null terminator i would've had the worst bug ever
        
        char* stackstr = v8helper2::allocateStackStr(str);

        //print(stackstr);
        //__debugbreak();
        Peggle::setEmbeddedText((void*) &textObj, stackstr, str->Length());
        //__debugbreak();
        DWORD graphicsType = 0x2D; //not totally sure what this param is but thi is my assumption
        if (info[3]->BooleanValue(isolate)) {
            graphicsType = IntegerFI(info[3]);
        }
        void* floatingText = Peggle::spawnFloatingText((void*)&textObj, &x, &y, graphicsType);
        info.GetReturnValue().Set(Templates::getFloatingTextImpl(isolate, floatingText));
    }));

    //im gonna make the very quick assumption that all the peg loops do this and so i'll implement them as such
    LogicMgr->Set(isolate, "enumOrangePegs", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> func = info[0].As<Function>();

        //i see with the eyes wide open (well kinda)
        //`__debugbreak();
        struct {
            DWORD esp_s8 = (ULONG_PTR(logicMgr) + 0x358);
            DWORD ebp_s4 = **(ULONG_PTR**)(ULONG_PTR(logicMgr) + 0x35C);
        } stack; //i have to make this struct so that these variables are in the right places because for some reason during the loop they call this weird function to find the next peg (i mean it's kinda like that)

        //constexpr DWORD offset = 0x00006300;
        //ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;
        //__debugbreak();

        DWORD ecx = *(DWORD*)(ULONG_PTR(logicMgr) + 0x35C);

        //i just realized that most of the weird arrays have a length property right after them!
        //DWORD length = *(DWORD*)(ULONG_PTR(logicMgr) + 0x360); //nah nah this is cheating i gotta do it right
        //while (stack.ebp_s4 != stack.ecx) {
        //for (DWORD i = 0; i < length; i++) {
        while(ecx != stack.ebp_s4) {
            //print(i);
            void* peg = *(void**)(stack.ebp_s4 + 0xC); //how did i forget to dereference the (void**)
            Local<Value> obj = Templates::GetImplFromPtr(isolate, peg);
            if (obj.IsEmpty()) {
                //well it's one of the other physobjs i haven't implemented yet lol just slap a generic on there
                obj = Templates::GenericPtrObject(isolate, peg)->NewInstance(context).ToLocalChecked();
            }
            TryCatch errhandler(isolate);
            MaybeLocal<Value> result = func->Call(context, context->Global(), 1, &obj); //why do i gotta cast it myself it should be able to see that right? i might be doing it wrong
            CHECKEXCEPTIONS(errhandler);
            if (!result.IsEmpty()) {
                if (result.ToLocalChecked()->BooleanValue(isolate)) {
                    break;
                }
            }

            //anyways here's the reason im using the stack local struct
            //this function changes the memory at ecx+4 (which would be my ebp_s4 variable)
            //__asm {
            //    //honestly i can't tell if this function is thiscall or fastcall
            //    lea ecx, stack
            //    call funcaddr
            //}
            //aw damn you aren't allowed to use __asm in a lambda
            Peggle::doSpecialThingForTheLoop(&stack); //this function lowkey confusing me bruh why did they do it like this

            //stack.ebp_s4 = *(ULONG_PTR*)(stack.ebp_s4 + 8);
        }
        //}
    }));

    LogicMgr->Set(isolate, "enumBluePegs", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> func = info[0].As<Function>();

        struct {
            DWORD esp_s8 = (ULONG_PTR(logicMgr) + 0x364);
            DWORD ebp_s4 = **(ULONG_PTR**)(ULONG_PTR(logicMgr) + 0x368);
        } stack; //i have to make this struct so that these variables are in the right places because for some reason during the loop they call this weird function to find the next peg (i mean it's kinda like that)

        DWORD ecx = *(DWORD*)(ULONG_PTR(logicMgr) + 0x368);

        while(ecx != stack.ebp_s4) {
            void* peg = *(void**)(stack.ebp_s4 + 0xC); //how did i forget to dereference the (void**)
            Local<Value> obj = Templates::GetImplFromPtr(isolate, peg);
            if (obj.IsEmpty()) {
                //well it's one of the other physobjs i haven't implemented yet lol just slap a generic on there
                obj = Templates::GenericPtrObject(isolate, peg)->NewInstance(context).ToLocalChecked();
            }
            TryCatch errhandler(isolate);
            MaybeLocal<Value> result = func->Call(context, context->Global(), 1, &obj); //why do i gotta cast it myself it should be able to see that right? i might be doing it wrong
            CHECKEXCEPTIONS(errhandler);
            if (!result.IsEmpty()) {
                if (result.ToLocalChecked()->BooleanValue(isolate)) {
                    break;
                }
            }

            //anyways here's the reason im using the stack local struct
            //this function changes the memory at ecx+4 (which would be my ebp_s4 variable)
            //__asm {
            //    //honestly i can't tell if this function is thiscall or fastcall
            //    lea ecx, stack
            //    call funcaddr
            //}
            //aw damn you aren't allowed to use __asm in a lambda
            Peggle::doSpecialThingForTheLoop(&stack); //this function lowkey confusing me bruh why did they do it like this

            //stack.ebp_s4 = *(ULONG_PTR*)(stack.ebp_s4 + 8);
        }
        //}
    }));

    //wait this one might work the same way the other arrays do
    LogicMgr->Set(isolate, "enumPegs", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> func = info[0].As<Function>();

        //__debugbreak();
        DWORD pegsOnBoard = *(DWORD*)(ULONG_PTR(logicMgr) + 0x348);
        if (pegsOnBoard) {
            ULONG_PTR ebp_s8 = **(ULONG_PTR**)(ULONG_PTR(logicMgr) + 0x344);

            ULONG_PTR eax = *(ULONG_PTR*)(ULONG_PTR(logicMgr) + 0x344);
            //ULONG_PTR ecx = *(ULONG_PTR*)(eax);
            //__debugbreak();
            while (eax != ebp_s8) {
                void* peg = *(void**)(ebp_s8 + 8);

                Local<Value> obj = Templates::GetImplFromPtr(isolate, peg);
                if (obj.IsEmpty()) {
                    //well it's one of the other physobjs i haven't implemented yet lol just slap a generic on there
                    obj = Templates::GenericPtrObject(isolate, peg)->NewInstance(context).ToLocalChecked();
                }
                TryCatch errhandler(isolate);
                MaybeLocal<Value> result = func->Call(context, context->Global(), 1, &obj); //why do i gotta cast it myself i should be able to see that right? i might be doing it wrong
                CHECKEXCEPTIONS(errhandler);
                if (!result.IsEmpty()) {
                    if (result.ToLocalChecked()->BooleanValue(isolate)) {
                        break;
                    }
                }

                ebp_s8 = *(ULONG_PTR*)(ebp_s8);
                //ecx = *(ULONG_PTR*)(esi);
            }
        }
    }));
    //LogicMgr->SetAccessorProperty(LITERAL("ball"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    //DWORD ballsOnBoard = *(DWORD*)(ULONG_PTR(logicMgr) + 0x35C);
    //
    //    //if (ballsOnBoard) { //if there is no ball the double dereference still works but + 8 holds 0! (yeah i guess sometimes that's not true lol)
    //        ULONG_PTR first = *(DWORD*)(ULONG_PTR(logicMgr) + 0x35C);
    //        if (first) {
    //            ULONG_PTR second = *(ULONG_PTR*)(first);
    //            ULONG_PTR third = *(ULONG_PTR*)(second + 0xC);
    //            //*(ULONG_PTR*)(second + 8) = 20;
    //            //ULONG_PTR ball_address = ((**(ULONG_PTR**)(ULONG_PTR(board) + 0x1A0)) + 8);
    //
    //            if (third) {
    //                info.GetReturnValue().Set(Templates::GetImplFromPtr(isolate, (void*)third));
    //            }
    //        }
    //    //}
    //    //if (ball) {
    //        //Local<Object> Ball = GetBallObjectImpl(isolate);
    //        //print("post get ball impl");
    //        //Ball->Set(isolate->GetCurrentContext(), LITERAL("location"), Number::New(isolate, (ULONG_PTR)ball));
    //        //print("ogmegdg");
    //        ////info.GetReturnValue().Set(GetBallObjectImpl(isolate, ball));
    //        //info.GetReturnValue().Set(Ball);
    //        //print("et");
    //        //info.GetReturnValue().Set(Templates::getBallImpl(isolate, ball));
    //    //}
    //}), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    ULONG_PTR first = ULONG_PTR(board) + 0x1A0;
    //    ULONG_PTR second = **(ULONG_PTR**)(first);
    //
    //    //__debugbreak();
    //    if (info[0]->IsNumber()) {
    //        *(ULONG_PTR*)(second + 8) = IntegerFI(info[0]);
    //    }
    //    else if (info[0]->IsObject()) {
    //        *(ULONG_PTR*)(second + 8) = IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
    //    }
    //})); //lmao i accidently left that shit read only

    //peggle->Set(isolate, "get_board", FunctionTemplate::New(isolate, peggle_get_board));

    /*peggle->SetAccessorProperty(LITERAL("windowTitle"),
        FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<Value>& info) {
            Isolate* isolate = info.GetIsolate();
            info.GetReturnValue().Set(String::NewFromTwoByte(isolate, (const uint16_t*)GetWindowText()))
        }), FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& info) {

        })
    );*/

    Local<ObjectTemplate> Board = Templates::GenericPtrObject(isolate, board);
    //Board->Set(isolate, "createFloatingText", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    void* floatingText = Peggle::createFloatingText((void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()));
    //    if (floatingText) {
    //        info.GetReturnValue().Set(Templates::getFloatingTextImpl(isolate, floatingText));
    //    }
    //}));
    Board->Set(isolate, "enumPhysObjs", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> func = info[0].As<Function>();
        //__debugbreak();
        //unfortunately this loop is really weird and complicated so hopefully i can get it right on the first try
        ULONG_PTR base = (info.This()->Get(context, LITERAL("location")).ToLocalChecked())->IntegerValue(context).FromJust();
        ULONG_PTR esi = (**(ULONG_PTR**)(base + 0x194)); // + 0x190; //haha no way im stepping through it and the compiler actually chose to store this in the esi register (aw nevermind they tricked me and actually used the edi register instead)
        ULONG_PTR ebx = base + 0x190;
        //cmp for error checking that i can't be bothered to do
        ULONG_PTR ecx = *(ULONG_PTR*)(ebx + 4);
    
        print(base << " " << esi << " " << ebx << " " << ecx);
    
        //some how this all worked out on the first try

        while (esi != ecx) {
            //__debugbreak();
            //print("yerp " << esi << " " << ebx << " " << ecx);
            ecx = *(ULONG_PTR*)(ebx + 4); //wait do i have to do this? ebx isn't what's changing and nothing is changing base+0x194
            void* physObj = *(void**)(esi + 8);
            //check type of physObj
            Local<Value> obj = Templates::GetImplFromPtr(isolate, physObj);
            if (obj.IsEmpty()) {
                //well it's one of the other physobjs i haven't implemented yet lol just slap a generic on there
                obj = Templates::GenericPtrObject(isolate, physObj)->NewInstance(context).ToLocalChecked();
            }
            TryCatch errhandler(isolate);
            MaybeLocal<Value> result = func->Call(context, context->Global(), 1, &obj); //why do i gotta cast it myself i should be able to see that right? i might be doing it wrong
            CHECKEXCEPTIONS(errhandler);
            if (!result.IsEmpty()) {
                if (result.ToLocalChecked()->BooleanValue(isolate)) {
                    break;
                }
            }
            esi = *(ULONG_PTR*)(esi);
        }
    }));
    Board->Set(isolate, "click", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Peggle::clickOnBoard(IntegerFI(info[0]), IntegerFI(info[1]), IntegerFI(info[2]));
    }));
    Board->Set(isolate, "setSlowMotion", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Peggle::setSlowMotion(IntegerFI(info[0]), IntegerFI(info[1]));
    }));
    Board->Set(isolate, "reloadGun", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        void* ball = Peggle::reloadGun();
        if (ball) {
            info.GetReturnValue().Set(Templates::getBallImpl(isolate, ball));
        }
    }));
    ByteProperty(Board, "mouseOver", 0x59); //i thinkj? (idk)
    ByteProperty(Board, "slowmotion", 0xC1);
    ByteProperty(Board, "frozen", 0xC2); //not the same as paused lol (Board sets this byte when it's waiting for the next phys loop because of slow motion)
    Property(Board, "slowMotionSpeed", 0xD0); //closer it is to 0, the slower the game runs
    //Property(Board, "ballHasBeenShot", 0x1A4); //this is actually the amount of balls on the board actually!
    Property(Board, "ballsOnBoard", 0x1A4);
    Property(Board, "viewX", 0x1A8);
    Property(Board, "viewY", 0x1AC);
    Property(Board, "paused", 0x1B8); //prolly  
    Property(Board, "pauseMenuVisibility", 0x1BC);
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

    //wait a second this is actually an array of balls if you do it right
    Board->SetAccessorProperty(LITERAL("ball"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
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

        //__debugbreak();
        if (info[0]->IsNumber()) {
            *(ULONG_PTR*)(second + 8) = IntegerFI(info[0]);
        }
        else if (info[0]->IsObject()) {
            *(ULONG_PTR*)(second + 8) = IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        }
    })); //lmao i accidently left that shit read only

    Board->Set(isolate, "enumBalls", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> func = info[0].As<Function>();

        //__debugbreak();
        DWORD ballsOnBoard = *(DWORD*)(ULONG_PTR(board) + 0x1A4);
        if (ballsOnBoard) {
            ULONG_PTR ebp_s8 = **(ULONG_PTR**)(ULONG_PTR(board) + 0x1A0);

            ULONG_PTR eax = *(ULONG_PTR*)(ULONG_PTR(board) + 0x1A0);
            //ULONG_PTR ecx = *(ULONG_PTR*)(eax);
            //__debugbreak();
            while (eax != ebp_s8) {
                void* ball = *(void**)(ebp_s8 + 8);

                Local<Value> obj = Templates::GetImplFromPtr(isolate, ball);
                if (obj.IsEmpty()) {
                    //well it's one of the other physobjs i haven't implemented yet lol just slap a generic on there
                    obj = Templates::GenericPtrObject(isolate, ball)->NewInstance(context).ToLocalChecked();
                }
                TryCatch errhandler(isolate);
                MaybeLocal<Value> result = func->Call(context, context->Global(), 1, &obj); //why do i gotta cast it myself i should be able to see that right? i might be doing it wrong
                CHECKEXCEPTIONS(errhandler);
                if (!result.IsEmpty()) {
                    if (result.ToLocalChecked()->BooleanValue(isolate)) {
                        break;
                    }
                }

                ebp_s8 = *(ULONG_PTR*)(ebp_s8);
                //ecx = *(ULONG_PTR*)(esi);
            }
        }
    }));

    Local<ObjectTemplate> FloatingTextMgr = Templates::GenericPtrObject(isolate, *(void**)(ULONG_PTR(board)+0x148));
    //FloatingTextMgr->Set(isolate, "createFloatingText", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    void* floatingText = Peggle::createFloatingText((void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()));
    //    if (floatingText) {
    //        info.GetReturnValue().Set(Templates::getFloatingTextImpl(isolate, floatingText));
    //    }
    //}));
    FloatingTextMgr->Set(isolate, "enumFloatingTextObjs", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> func = info[0].As<Function>();
        //__debugbreak();
        //unfortunately this loop is really weird and complicated so hopefully i can get it right on the first try
        ULONG_PTR base = (info.This()->Get(context, LITERAL("location")).ToLocalChecked())->IntegerValue(context).FromJust();
        ULONG_PTR eax = *(ULONG_PTR*)(base + 0xC); //eax not used again lol
        ULONG_PTR esi = *(ULONG_PTR*)(eax);
        ULONG_PTR edi = base + 8;

        ULONG_PTR ebx = *(ULONG_PTR*)(edi + 4);
        
        while (esi != ebx) {
            //__debugbreak();
            void* floatingText = (void*)(esi + 8); //oh wait i wasn't supposed to dereference it lol
            //check type of physObj
            Local<Value> obj = Templates::GetImplFromPtr(isolate, floatingText);
            if (obj.IsEmpty()) {
                //well it's one of the other physobjs i haven't implemented yet lol just slap a generic on there
                obj = Templates::GenericPtrObject(isolate, floatingText)->NewInstance(context).ToLocalChecked();
            }
            TryCatch errhandler(isolate);
            MaybeLocal<Value> result = func->Call(context, context->Global(), 1, &obj); //why do i gotta cast it myself i should be able to see that right? i might be doing it wrong
            CHECKEXCEPTIONS(errhandler);
            if (!result.IsEmpty()) {
                if (result.ToLocalChecked()->BooleanValue(isolate)) {
                    break;
                }
            }
            esi = *(ULONG_PTR*)(esi);
        }
    }));
    Local<ObjectTemplate> WidgetManager = Templates::GenericPtrObject(isolate, *(void**)(ULONG_PTR(board) + 0x10));
    Property(WidgetManager, "mouseX", 0xE0);
    Property(WidgetManager, "mouseY", 0xE4);

    //uh the same value seems the be at 0xE8 too
    //left button is 0b001
    //right button is 0b010
    //middle button might be 0b100 but idk yet lo
    Property(WidgetManager, "mouseButtons", 0xEC); //bit field

    WidgetManager->Set(isolate, "isKeyDown", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        //this isn't an actual function in peggle but what im doing here is taken from offset 0x0013E866, specifically the part where it moves a byte (1) into WidgetManager+keyCode+F4
        
        DWORD eax = IntegerFI(info[0]); //keyCode

        if (info[0]->IsString()) {
            eax = toupper(*(char*)CStringFI(info[0]));
        }
        else if (info[0]->IsNumber()) {
            eax = IntegerFI(info[0]);
        }

        //they also do this check too
        if (eax < 0xFE) {
            info.GetReturnValue().Set(Integer::New(isolate, *(BYTE*)(eax + base + 0xF4)));
        }
    }));

    ByteProperty(WidgetManager, "arrowLeft", 0x119);
    ByteProperty(WidgetManager, "arrowUp", 0x11A);
    ByteProperty(WidgetManager, "arrowRight", 0x11B);
    ByteProperty(WidgetManager, "arrowDown", 0x11C);

    Local<ObjectTemplate> Gun = Templates::GenericPtrObject(isolate, *(void**)(ULONG_PTR(board)+0xBC));
    FloatProperty(Gun, "lastRotation", 0x150); //controlled by LogicMgr
    FloatProperty(Gun, "ballX", 0x158); //idk that's what it's set to when you shoot it
    FloatProperty(Gun, "ballY", 0x15C);
    //FloatProperty(Gun, "rotation", 0x188); //uhhhh there's two? (changing this one didn't do anything)
    FloatProperty(Gun, "offsetX", 0x164); //some kind of offset or like a bounding box
    FloatProperty(Gun, "offsetY", 0x168);
    FloatProperty(Gun, "idkBruh", 0x16C);
    FloatProperty(Gun, "offsetBallRadius", 0x170);

    //im adding this function just in case you want to create your own ball and set it as the Gun's ball (the Board function creates the ball for you and does this too)
    Gun->Set(isolate, "setNewBall", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        void* base = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        void* ball = nullptr;
        if (info[0]->IsNumber()) {
            ball = (void*)IntegerFI(info[0]);
        }
        else if (info[0]->IsObject()) {
            ball = (void*)IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        }
        Peggle::setGunNewBall(base, ball);
    }));
    Gun->SetAccessorProperty(LITERAL("ball"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        ULONG_PTR base = IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        void* ball = *(void**)(base + 0x1C0);
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
        ULONG_PTR* ballPtr = (ULONG_PTR*)(IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0x1C0);
        if (info[0]->IsNumber()) {
            *ballPtr = IntegerFI(info[0]);
        }
        else if (info[0]->IsObject()) {
            *ballPtr = IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        }
    }));

    Local<ObjectTemplate> SoundMgr = Templates::GenericPtrObject(isolate, *(void**)(ULONG_PTR(board) + 0x158));
    SoundMgr->Set(isolate, "playSoundSimple", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        void* base = (void*)IntegerFI(info.This()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked());
        Peggle::playSoundSimple(base, IntegerFI(info[0]), IntegerFI(info[1]));
    }));

    global->Set(isolate, "SoundMgr", SoundMgr);
    global->Set(isolate, "Gun", Gun);
    global->Set(isolate, "WidgetManager", WidgetManager);
    global->Set(isolate, "FloatingTextMgr", FloatingTextMgr);
    global->Set(isolate, "Board", Board);
    global->Set(isolate, "LogicMgr", LogicMgr);
    //print("done!e");
    //global->Set(isolate, "peggle", peggle);

    global->Set(isolate, "createBall", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        void* ball = Peggle::createBall(IntegerFI(info[0]), IntegerFI(info[1]), FloatFI(info[2]), FloatFI(info[3]), FloatFI(info[4]), FloatFI(info[5]));
        if (ball) {
            info.GetReturnValue().Set(Templates::getBallImpl(isolate, ball));
        }
    }));
    global->Set(isolate, "createPegInfo", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();

        constexpr DWORD opnOffset = 0x0019C159;
        opnfunc operator_new = (opnfunc)((ULONG_PTR)g_peggle + opnOffset);
        
        //uhh ok nevermind lol not only did i forget to initialize refCount but i also (for some reason) didn't realize that my vtable would be different from their vtable so im just gonna create one like they do (my PegInfo struct is still right tho (besides the virtual methods lol))
        void* alloc_peginfo = Peggle::PegInfoConstructor();
        if (alloc_peginfo) {
            //__debugbreak();
            *(DWORD*)(ULONG_PTR(alloc_peginfo) + 0x10) = IntegerFI(info[1]); //peg type
            *(DWORD*)(ULONG_PTR(alloc_peginfo) + 0x24) = IntegerFI(info[2]); //peg animation id
            DWORD* address = nullptr;
            if (info[0]->IsNumber()) {
                address = (DWORD*)IntegerFI(info[0]);
            }
            else if (info[0]->IsObject()) {
                address = (DWORD*)(IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked()) + 0xD0);
            }
            if (address) {
                //__debugbreak();
                InterlockedIncrement((DWORD*)(ULONG_PTR(alloc_peginfo) + 4));
                *address = (DWORD)alloc_peginfo;
            }
            info.GetReturnValue().Set(Templates::getPegInfoImpl(isolate, alloc_peginfo));
        }
        /*
        //uhh lowkey since i've made an overloaded PegInfo constructor i don't know if i can call it in asm
        //so we're gonna be cheap and just create it on the stack and copy it lol
        __debugbreak();
        PegInfo stack_peginfo = PegInfo(IntegerFI(info[1]), IntegerFI(info[2]));
        __debugbreak();
        DWORD* address = nullptr;
        if (info[0]->IsNumber()) {
            address = (DWORD*)IntegerFI(info[0]);
        }
        else if (info[0]->IsObject()) {
            address = (DWORD*)(IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked())+0xD0);
        }
        __debugbreak();
        PegInfo* alloc_peginfo = (PegInfo*)operator_new(sizeof(PegInfo));
        if (address) { //they actually have a function for this purpose in asm but i can't be bothered to call it because you can't use __asm blocks in lambda functions (there's another way to call it but i just don't wanna ok)
            InterlockedIncrement((DWORD*)&stack_peginfo.refCount);
            *address = (DWORD)alloc_peginfo;
        }
        memcpy(alloc_peginfo, &stack_peginfo, sizeof(PegInfo));*/
    }));
    global->Set(isolate, "createMover", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        void* mover = Peggle::MoverConstructor();
        if (mover) {
            //__debugbreak();
            *(float*)((ULONG_PTR)mover + 0x5C) = FloatFI(info[1]); //originX
            *(float*)((ULONG_PTR)mover + 0x60) = FloatFI(info[2]); //originY
            DWORD* address = nullptr;
            if (info[0]->IsNumber()) {
                address = (DWORD*)IntegerFI(info[0]);
            }
            else if (info[0]->IsObject()) {
                address = (DWORD*)(IntegerFI(info[0].As<Object>()->Get(isolate->GetCurrentContext(), LITERAL("location")).ToLocalChecked())+0x48);
            }
            //__debugbreak();
            if (address) {
                InterlockedIncrement((DWORD*)(ULONG_PTR(mover) + 4));
                *address = (DWORD)mover;
            }
            info.GetReturnValue().Set(Templates::getMoverImpl(isolate, mover));
        }
    }));

    global->SetAccessorProperty(LITERAL("canFastForwardDuringTurns"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        BYTE first = *(BYTE*)(ULONG_PTR(g_peggle) + 0x00010F83);
        info.GetReturnValue().Set(first != 0x83); //when first is 0x83 fast forwarding is not allowed because that's the first byte of the original instruction
    }), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        bool doit = info[0]->BooleanValue(isolate);
        BYTE* lebytes;
        if (doit) {
            BYTE sixByteNop[] = {
                0x66, 0x0f, 0x1f, 0x44, 0x00, 0x00, //nop word ptr [eax+eax*1+0x0]
            };
            lebytes = sixByteNop;
        }
        else {
            BYTE actualInstructions[] = {
                0x83, 0x79, 0x04, 0x02, //cmp dword ptr [ecx+4], 2
                0x74, 0x16,             //je 410F9F
            };
            lebytes = actualInstructions;
        }
        memcpy((void*)(ULONG_PTR(g_peggle) + 0x00010F83), lebytes, 6); //wow this memcpy was compiled into 3 mov instructions
        FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
    }));

    global->SetAccessorProperty(LITERAL("canShootBallDuringTurns"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        BYTE first = *(BYTE*)(ULONG_PTR(g_peggle) + 0x0007299B);
        info.GetReturnValue().Set(first != 0x83); //when first is 0x83 you can't shoot the ball during a turn because that's the first byte of the original instruction (a cmp to check if the LogicMgr's current event is 1)
    }), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        bool doit = info[0]->BooleanValue(isolate);
        BYTE* lebytes;
        if (doit) {
            BYTE fiveByteNop[] = {
                0x0f, 0x1f, 0x44, 0x00, 0x00, //nop dword ptr [eax+eax*1+0x0]
            };
            lebytes = fiveByteNop;
        }
        else {
            BYTE actualInstructions[] = {
                0x83, 0xF8, 0x01, //cmp eax, 1
                0x75, 0x26,       //jne +0x26
            };
            lebytes = actualInstructions;
        }
        memcpy((void*)(ULONG_PTR(g_peggle) + 0x0007299B), lebytes, 5);
        FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
    }));

     global->SetAccessorProperty(LITERAL("canShootPegs"), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        BYTE first = *(BYTE*)(ULONG_PTR(g_peggle) + 0x0004DA31);
        info.GetReturnValue().Set(first != 0x39); //im honestly not 100% sure what the function this instruction is in does but im pretty sure it lets you shoot a peg instead of a ball (if you set Gun.ball = a peg)
    }), FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        bool doit = info[0]->BooleanValue(isolate);
        BYTE* lebytes;
        BYTE* lebytes2;
        if (doit) {
            BYTE fiveByteNop[] = {
                0x0f, 0x1f, 0x44, 0x00, 0x00, //nop dword ptr [eax+eax*1+0x0]
            };
            BYTE tenByteNop[] = {
                0x66, 0x66, 0x0f, 0x1f, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, //data16 nop word ptr [eax+eax+1*0x0] (somehow a 10 byte nop)
            };
            lebytes = fiveByteNop;
            lebytes2 = tenByteNop;
        }
        else {
            BYTE actualInstructions[] = {
                0x39, 0x47, 0x10, //cmp dword ptr [edi+10], eax
                0x75, 0x62,       //jne +0x62
            };
            BYTE actualInstructions2[] = {
                0x83, 0x7F, 0x10, 0x02,             //cmp dword ptr [edi+10], 2
                0x0F, 0x85, 0xE5, 0x00, 0x00, 0x00, //jne +0xE5
            };
            lebytes = actualInstructions;
            lebytes2 = actualInstructions2;
        }
        memcpy((void*)(ULONG_PTR(g_peggle) + 0x0004DA31), lebytes, 5);
        memcpy((void*)(ULONG_PTR(g_peggle) + 0x00021143), lebytes2, 10);
        FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
    }));

    //global->Set(isolate, "allowFastForwardingDuringTurn", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
    //    Isolate* isolate = info.GetIsolate();
    //    bool doit = info[0]->BooleanValue(isolate);
    //    __debugbreak();
    //    BYTE* lebytes;
    //    if (doit) {
    //        BYTE fourByteNop[] = {
    //            0x0f,0x1f,0x40,0x00
    //        };
    //        lebytes = fourByteNop;
    //    }
    //    else {
    //        BYTE actualInstruction[] = {
    //            0x83, 0x79, 0x04, 0x02, //cmp dword ptr [ecx+4], 2
    //        };
    //        lebytes = actualInstruction;
    //    }
    //    memcpy((void*)(ULONG_PTR(g_peggle) + 0x00010F83), lebytes, 4);
    //    FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
    //}));

    //after changing this one i should probably call FlushInstructionCache but usually you don't have to (plus then i'd have to make a new macro)
    GlobalVariableProperty(global, "defaultTextColor", 0x00069F08); //lmao the default floating text color is lowkey hardcoded in so we're changing the actual hardcoded value in the spawnFloatingText/showHoveringTextOverPeg function
    //global variable property to control the speed of fastforwarding (but since it's hard coded it can only go from 0-7f because it's a signed byte push (0x6a op code))
    GlobalVariableByteProperty(global, "fastForwardSpeed", 0x00010F97);
    
    GlobalVariableFloatProperty(global, "defaultBallRadius", 0x249D78);
    GlobalVariableFloatProperty(global, "gravity", 0x249D80);

    global->Set(isolate, "castPtrToObj", FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
        Isolate* isolate = info.GetIsolate();
        void* ptr = (void*)IntegerFI(info[0]);
        if (ptr) {
            info.GetReturnValue().Set(Templates::GetImplFromPtr(isolate, ptr));
        }
    }));

    //create ball 0x00080DE0
    //set position
    //set velocity
    //interlocked increment
    //board after shoot 0x00022A90
    //interlocked decrement

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
    DWORD ballsOnBoard = *(DWORD*)(ULONG_PTR(board) + 0x1A4);

    if (ballsOnBoard) { //if there is no ball the double dereference still works but + 8 holds 0! (yeah i guess sometimes that's not true lol)
        ULONG_PTR first = ULONG_PTR(board) + 0x1A0;
        ULONG_PTR second = **(ULONG_PTR**)(first);
        ULONG_PTR third = *(ULONG_PTR*)(second + 8);
        //*(ULONG_PTR*)(second + 8) = 20;
        //ULONG_PTR ball_address = ((**(ULONG_PTR**)(ULONG_PTR(board) + 0x1A0)) + 8);

        return (void*)third;
    }
    return nullptr;
}

void Peggle::activateAbility(void* ball, void* physObj2, DWORD ability, BOOL isAbilitySpecified) {
    //uhh we need to call this function
    //uh oh how the fuck do i call a member function myself
    //well since we've got the option i might have to __asm this shit!

    //constexpr DWORD offset = 0x0006EDF0;

    //i think there are 2 ways to call this function
    //if you know which ability you want, you pass it
    //if you don't, set ability to 0
    //the problem is i don't really know what you pass as the first 2 arguments
    //if you don't know the ability (meaning it's implied), the first 2 arguments seem to point to 2 different Ball objects (which is weird because why are there 2)

    //ok for some reason the first 2 parameters point to the same address so lets allocate that on le stack right here
    //yeah im not gonna lie idk if these were ever right? they still worked though so imma use them as default params
    //DWORD unknown1 = 0x005F19B4; //idk
    ////DWORD unknown2 = 0x005F1FDC; //for some reason this one was different! (when i used warren and got uhh i forgot what exactly it gave me thhis)
    //DWORD unknown2 = unknown1;

    //yeah so i asked chat and he said asm was the goat so...
    //hold on i just randomly thought about it and i think you could still call a method without asm!
    //void*(__fastcall mango)(DWORD register_ecx, DWORD register_edx_unused, DWORD stack_param2, DWORD stack_param3, ...);
    //using fastcall you could pass the object as the very first parameter
    //DWORD unknown1 = IntegerFI(info[0]);

    //DWORD unknown2;// = IntegerFI(info[1]);

    constexpr DWORD offset = 0x0006EDF0;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    ////bruh this actually worked but didn't at the same time
    //__asm {
    //    //int 3
    //    push isAbilitySpecified
    //    push ability
    //    //lea eax, unknown2
    //    //push eax
    //    //lea eax, unknown1
    //    //push eax
    //    push unknown2
    //    push unknown1
    //    mov ecx, Peggle::logicMgr
    //    call peggle_internal_activate_ability
    //}
    ////g_peggle_internal_activate_ability(nullptr, nullptr, IntegerFI(info[0]), 1); //idk im assuming this shit...


    __asm {
        //int 3
        push isAbilitySpecified
        push ability
        push physObj2
        push ball
        mov ecx, Peggle::logicMgr
        call funcaddr
    }
}

void Peggle::clickOnBoard(DWORD mouseX, DWORD mouseY, DWORD mouseButton = 1) { //ohhhh right click is -1 and middle mouse is 3
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

void Peggle::addBalls(DWORD amount, DWORD unknown, DWORD b_unknown2) { //b_unknown2 is some kinda boolean or something
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

void Peggle::setHit(void* pegInfo, DWORD hit) {
    constexpr DWORD offset = 0x00076650;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov eax, funcaddr
        push hit
        mov ecx, pegInfo
        call eax
    }
}

void Peggle::setPegType(void* BrickOrBall, DWORD ptype) { //im assuming i know what this function does lol (fortunately if the object doesn't have peginfo it will safely return)
    constexpr DWORD offset = 0x00079870;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    //"type" is reserved in __asm blocks so i named the parameter ptype lol
    __asm {
        mov eax, funcaddr
        push ptype
        mov ecx, BrickOrBall
        call eax
    }
}

void* Peggle::createFloatingText(void* floatingTextMgr) { //uhhh every time i use this function it crashes lol hold on imma figure out how it works in a minute (yeah ok don't use this function use the spawn one)
    __UNTESTED;
    constexpr DWORD offset = 0x00069D20;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov eax, funcaddr
        mov ecx, floatingTextMgr
        call eax
    }
}

void* Peggle::spawnFloatingText(void* embeddedText, float* x, float* y, DWORD $idk) {
    constexpr DWORD offset = 0x00069EB0;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov eax, funcaddr
        mov ecx, logicMgr
        push $idk

        //uhhh im gonna use the fpu because i forgot how to use the xmm stuff

        //dummy pushes for fpu
        push ecx

        //wait apparently (as seen by some of my other functions) you can just push the float directly? (well im taking a pointer to the float here so i'd have to change that lol)
        mov edx, y

        fld dword ptr [edx]
        fstp dword ptr [esp]

        push ecx

        mov edx, x
        fld dword ptr [edx]
        fstp dword ptr [esp]

        push embeddedText
        call eax
    }
}

void Peggle::setFloatingTextText(void* floatingText, void* embeddedText) {
    //__UNTESTED;
    constexpr DWORD offset = 0x00066610;
    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov eax, funcaddr
        mov ecx, floatingText
        push embeddedText
        call eax
    }
}

void Peggle::setEmbeddedText(void* textObject, const char* text, size_t textLength) {
    constexpr DWORD offset = 0x0000CEA0;
    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    //"length" is reserved in __asm blocks so i had to use textLength lol
    __asm {
        mov eax, funcaddr
        mov ecx, textObject
        push textLength
        push text
        call eax
    }
}

void Peggle::setPhysVelocity(void* physObj, float vx, float vy) {
    __asm {
        //hmm in modern times floating point arguments are passed through xmm registers so how do i put those onto stack (actually it's probably easy)
        mov ecx, physObj
        mov eax, [ecx]
        mov edx, [eax+0x4C]

        //sub esp, 8
        //mov dword ptr [esp+4], vx
        //mov dword ptr [esp], vy
        push vy
        push vx
        call edx
    }
}

void Peggle::setPhysPosition(void* physObj, float x, float y) {
    __asm {
        //hmm in modern times floating point arguments are passed through xmm registers so how do i put those onto stack (actually it's probably easy)
        mov ecx, physObj
        mov eax, [ecx]
        mov edx, [eax + 0x44]

        //sub esp, 8
        //mov dword ptr [esp+4], vx
        //mov dword ptr [esp], vy
        push y
        push x
        call edx
    }
}

void Peggle::getPhysPosition(void* physObj, OUT float* arr) {
    __asm {
        mov ecx, physObj
        mov eax, [ecx]
        mov edx, [eax+0x74]
        //lol i crashed it i forgot to push the arr
        push arr
        call edx
    }
}

//calls a member function that (im assuming) all phys objects have (Ball/Brick/Hole)
//lol chatgpt told me that if i slapped all of these on here it would force it to use the fpu (but it didn't lol)
//extern "C" float __stdcall Peggle::getPhysX(void* physObj) {
float Peggle::getPhysX(void* physObj) {
    __asm {
        mov ecx, physObj
        //lmao for some reason i tried to do mov eax, [physObj] and it lowkey ignored the brackets
        mov eax, [ecx]
        mov edx, [eax+0x78]
        call edx

        //well dang now i gotta figure out how to use the xmm registers again because i forgot
        //dummy push for fpu
        push ecx
        fstp dword ptr [esp]

        movss xmm0, dword ptr [esp]

        //sadly we have to add 4 since we pushed that earlier
        add esp, 4
    }
}

//extern "C" float __stdcall Peggle::getPhysY(void* physObj) {
float Peggle::getPhysY(void* physObj) {
    __asm {
        mov ecx, physObj
        //lmao for some reason i tried to do mov eax, [physObj] and it lowkey ignored the brackets
        mov eax, [ecx]
        mov edx, [eax+0x7C]
        call edx

        //well dang now i gotta figure out how to use the xmm registers again because i forgot
        //dummy push for fpu
        push ecx
        fstp dword ptr [esp]

        movss xmm0, dword ptr [esp]

        //sadly we have to add 4 since we pushed that earlier
        add esp, 4
    }
}

//this function sets the physObj+0x24 and physObj+0x25 properties to the second parameter's value
//physObj+0x24 controls if this object has collision
//physObj+0x25 controls visibility
void Peggle::setPhysActive(void* physObj, bool active) {
    constexpr DWORD offset = 0x000769F0;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, physObj
        push active
        call funcaddr
    }
}

bool Peggle::loadImgIfImgName(void* physObj) {
    constexpr DWORD offset = 0x0007F830;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, physObj
        call funcaddr
    }
}

void Peggle::playPegAnimation(void* pegInfo, DWORD animationId) {
    constexpr DWORD offset = 0x00076670;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov eax, funcaddr
        mov ecx, pegInfo
        push animationId
        call eax
    }
}

void Peggle::lightUpPeg(void* pegInfo, DWORD timeLength) {
    constexpr DWORD offset = 0x00076690;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, pegInfo
        push timeLength
        call funcaddr
    }
}


void Peggle::playSoundSimple(void* soundMgr, DWORD id, DWORD idk) {
    constexpr DWORD offset = 0x0005AF70;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;
    
    __asm {
        mov ecx, soundMgr
        push idk
        push id
        call funcaddr
    }
}

void Peggle::doSpecialThingForTheLoop(OUT void* stack) {
    constexpr DWORD offset = 0x00006300;
    
    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, stack
        call funcaddr
    }
}

//most of the stuff i do in this function were taken from the multiball powerup function
//oh ok i get it now: the first param isn't anchored but actually isPeg
//isPeg determines whether or not this ball will have its phys type set to PHYS_PEG and be anchored
//if pushOntoBoard == false, the ball will not be pushed into the physObj and ball arrays and the only time you might need this is if you want this ball to be owned by the Gun (like with demo.js' reloadGun function)
void* Peggle::createBall(bool isPeg, bool pushOntoBoard, float x, float y, float vx, float vy) {
    constexpr DWORD offset = 0x00080DE0;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    //void* ball = malloc(0x190); //they used the new operator but i don't actually have the ball class lol (hmm perhaps the fact they used new and im using malloc makes it crash whenever it tries to free the memory... INVESTIGATING!)
    //alright my hypothesis for why it keeps crashing is this: they use the new operator and use some weird free function (that x64dbg and relyze doesn't recognize)

    //let's use the same "new" function that they do (im feeling fancy so i'll call it like a regular function)
    constexpr DWORD opnOffset = 0x0019C159;
    opnfunc operator_new = (opnfunc)((ULONG_PTR)g_peggle + opnOffset);
    void* ball = operator_new(0x190);

    if (ball) {
        constexpr DWORD bfaoffset = 0x00022A90; //offset for a board function that i THINK will add the ball to the Board+1A0 array (it does but only if the ball has a phys type of 2 (PHYS_BALL))

        ULONG_PTR boardfuncaddr = ULONG_PTR(g_peggle) + bfaoffset;

        //uh oh inline asm can't call overloaded functions
        //how do i can interlocked increment (i guess we'll just use InterlockedAdd lol)

        __asm {
            mov ecx, ball
            push isPeg
            call funcaddr
            //eax = the ball
            
            push esi
            //esi = ball
            mov esi, eax
            //no test because idk i assume it might work
            //int 3

            //honestly idk why they do this but it might be important...
            lea ecx, [esi+4]
            //lol i gotta use InterlockedAdd instead of InterlockedIncrement because InterlockedIncrement is overloaded :(
            push 1
            push ecx
            call InterlockedAdd
            //oh no! it's __cdecl! (i thought it was __stdcall)
            add esp, 8
            //call InterlockedIncrement

            //calling ball setPosition method :eyes:
            mov ecx, esi
            mov edx, [ecx]
            mov edx, [edx+0x44]
            push y
            push x
            call edx

            //calling ball setVelocity method
            mov ecx, esi
            mov edx, [ecx]
            mov edx, [edx+0x4C]
            push vy
            push vx
            call edx

            //if you pass false for pushOntoBoard we skip that board method
            mov al, pushOntoBoard
            test al, al
            je Afterpush

            //this function adds the ball to the phys obj array and the ball array
            //ok yeah im pretty sure im right, it also adds it to the physObj array at Board+194
            mov ecx, board
            push esi
            call boardfuncaddr

            Afterpush:

            lea ecx, [esi+4]
            push -1
            push ecx
            call InterlockedAdd
            add esp, 8
            //call InterlockedDecrement

            //cl = pushOntoBoard
            mov cl, pushOntoBoard
            //dl = 1
            mov dl, 1
            //dl = 1 - pushOntoBoard
            sub dl, cl
            //al = al + (!pushOntoBoard)
            add al, dl
            //basically, all that to say if you pass false as pushOntoBoard we add 1 to eax, skipping the int3
            test eax, eax
            jg End

            //idk when this would happen but they check for it in the multiball function
            //oh wait i think they destroy the ball if it has 0 references (Ball+4 is refCount)
            int 3

            End:
            pop esi
        }
    }
    else {
        print("some how my new void* 0x190 failed");
    }

    return ball;
}

void Peggle::destroyBall(void* ball, bool freeMemory) {
    constexpr DWORD offset = 0x0007F130;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        //actually i don't need funcaddr and offset because the pointer to this function is located as the first element in the vtable
        //mov ecx, ball
        //mov eax, [ecx]
        //mov eax, [eax]
        //push freeMemory
        //call eax

        mov ecx, ball
        push freeMemory
        call funcaddr
    }
}

//im using BOOL instead of bool because this function actually sets ALL of eax to the return value (which usually it only sets al)
//it actually doesn't matter though and im sure that if i used bool it would work just the same lol
BOOL Peggle::randomBallMethod(void* ball, void* other) {
    constexpr DWORD offset = 0x00077A00;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, ball
        push other
        //this function checks SOMETHING but i don't know what...
        call funcaddr
    }
}

void* Peggle::MoverConstructor() {
    constexpr DWORD opnOffset = 0x0019C159;
    opnfunc operator_new = (opnfunc)((ULONG_PTR)g_peggle + opnOffset);

    void* mover = operator_new(104); //a mover is 104 bytes long

    constexpr DWORD offset = 0x0007B430; //oops lowkey i had an extra 4 in there but doctor runtime has solved the case

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, mover
        call funcaddr
    }
}

void* Peggle::PegInfoConstructor() {
    constexpr DWORD opnOffset = 0x0019C159;
    opnfunc operator_new = (opnfunc)((ULONG_PTR)g_peggle + opnOffset);

    void* peginfo = operator_new(40); //PegInfo is 40 bytes long

    constexpr DWORD offset = 0x000765F0; //oops lowkey i had an extra 4 in there but doctor runtime has solved the case

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, peginfo
        call funcaddr
    }
}

void Peggle::setPolyRotation(void* poly, float rotation) {
    constexpr DWORD offset = 0x0007A010;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, poly
        push rotation
        call funcaddr
    }
}

void Peggle::setSlowMotion(bool slowmotion, DWORD speed) {
    constexpr DWORD offset = 0x000026F0;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, board
        push speed
        push slowmotion
        call funcaddr
    }
}

//void Peggle::pushObjToPhysArray(void* physobj) {
    
//}

void Peggle::setGunNewBall(void* Gun, void* ball) {
    constexpr DWORD offset = 0x000844C0;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        mov ecx, Gun
        push ball
        call funcaddr
    }
}

void* Peggle::reloadGun() {
    constexpr DWORD offset = 0x000090D0;

    ULONG_PTR funcaddr = ULONG_PTR(g_peggle) + offset;

    __asm {
        //for later
        push esi
        mov esi, board

        mov ecx, esi
        call funcaddr

        //this function doesn't actually return the ball so we'll just do it here
        //Board has a pointer to Gun at 0xBC
        mov eax, [esi+0xBC]
        //Gun (eax) has a pointer to the ball at 0x1C0
        mov eax, [eax+0x1C0]

        //non-volatile
        pop esi
    }
}