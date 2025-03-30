#pragma once
namespace Peggle {
	extern bool init;
	extern void* board;
	extern void* logicMgr;
	//extern void* ball;

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
	void* spawnFloatingText(void* embeddedText, float* x, float* y, DWORD $idk);
	void setFloatingTextText(void* floatingText, void* embeddedText);
	void setEmbeddedText(void* textObject, const char* text, size_t textLength);
	void setPhysVelocity(void* physObj, float vx, float vy);
	void getPhysPosition(void* physObj, OUT float* arr);
	void setPhysPosition(void* physObj, float x, float y);
	//extern "C" float __stdcall getPhysX(void* physObj);
	//extern "C" float __stdcall getPhysY(void* physObj);
	float getPhysX(void* physObj);
	float getPhysY(void* physObj);
	void setPhysActive(void* physObj, bool idk);
	bool loadImgIfImgName(void* physObj);
	void playPegAnimation(void* pegInfo, DWORD animationId);
	void lightUpPeg(void* pegInfo, DWORD timeLength);
	void playSoundSimple(void* soundMgr, DWORD id, DWORD idk);

	void doSpecialThingForTheLoop(OUT void* stack);

	void* createBall(bool isPeg, bool notOnBoard, float x, float y, float vx, float vy);
	void destroyBall(void* ball, bool freeMemory);
	BOOL randomBallMethod(void* ball, void* other);
	void* MoverConstructor();
	void* PegInfoConstructor();
	void setPolyRotation(void* poly, float rotation);

	void setSlowMotion(bool slowmotion, DWORD speed);
	//void* createEmbeddedText(OUT void*, DWORD text);

	void setGunNewBall(void* Gun, void* ball);
	void* reloadGun();
}

