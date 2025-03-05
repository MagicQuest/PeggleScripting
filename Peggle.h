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
	void activateAbility(void* ball, void* physObj2, DWORD ability, BOOL isAbilitySpecified);
	void clickOnBoard(DWORD mouseX, DWORD mouseY, DWORD mouseButton);
	void addBalls(DWORD amount, DWORD unknown, DWORD b_unknown2);
	void setHit(void* pegInfo, DWORD hit);
	void setPegType(void* BrickOrBall, DWORD ptype);
	void* createFloatingText(void* board);
	void setEmbeddedText(void* textObject, const char* text, size_t textLength);
	extern "C" float __stdcall getPhysX(void* physObj);
	extern "C" float __stdcall getPhysY(void* physObj);
	void playPegAnimation(void* pegInfo, BOOL popped);
}

