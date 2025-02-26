#pragma once
namespace Peggle {
	extern bool init;
	extern void* logicMgr;
	extern void* board;
	extern void* ball;

	void InitPeggleGlobals(const v8::Local<v8::ObjectTemplate>& global, v8::Isolate* isolate);
	void* GetThunderballApp();
	void* GetBoard();
	void* GetLogicMgrFromBoard(void* board);
	void* TryGetBallFromBoard(void* board);
}

