#pragma once

extern HMODULE g_thismodule;
extern void* g_peggle;

//v8helper* virtualeight;

extern HMODULE extended_binary;
extern wchar_t g_filename[MAX_PATH];

typedef v8::Local<v8::ObjectTemplate>(*JBSInit)(v8::Isolate*, const wchar_t*, int);

extern JBSInit daddygibmemyglobals;

extern DWORD g_eax;
extern DWORD g_ecx;
extern DWORD g_edx;
//extern DWORD g_ballCount;

//extern void* g_logicmgr; //RTTI says that this random important struct is called the LogicMgr
//extern void* g_board;
//extern void* g_ball;

//HANDLE g_watcherthread;
//extern HANDLE notifyEvent;

//typedef void(__thiscall *activateAbilitySignature)(void*, void*, DWORD, DWORD);
//activateAbilitySignature g_peggle_internal_activate_ability;
extern void* g_peggle_internal_activate_ability;

extern DWORD g_returnval;