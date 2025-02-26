#pragma once

#include <windows.h>
#include "Controls.h"

class Ball : public Controls::Bitmap {
public:
	float intendedX;
	float intendedY;
	D2D1_POINT_2F velocity; //lol using the d2d one because it's alr defined nothing special about it
	bool bounced;

	Ball(float _x, float _y, float _width, float _height, const wchar_t* path, float opacity);
	void tick(float ratioX, float ratioY);
	void bounce();
};

struct Slice {
	float x;
	float y;
	float width;
	float height;
	float kerningWidth = 0;
	float kerningHeight = 0;
};

class PeggleUI : public Controls::Parent
{
public:
	std::vector<Controls::Bitmap*> backgroundimgs;
	int loading; //loading is 0-25
	ID2D1Bitmap* loadingLights;
	Controls::Bitmap* sunBmp;
	//Controls::Bitmap* sunFaceBmp;
	Controls::Bitmap* shadowBmp;
	Controls::Bitmap* peggleBmp;
	Controls::TiledBitmapButton* hookButton;
	HANDLE peggleProcess;
	
	HWND hwnd;
	bool intro = false;
	Ball* postIntroBall = nullptr;
	unsigned long long timing;
	unsigned long long introLoadingTiming;
	unsigned long long bounceTiming;

	float ratioX;
	float ratioY;

	const Slice loadingSlices[5] = {
		{0, 0, 18, 49},
		{18, 0, 18, 49, 12},
		{36, 0, 18, 49, 14}, //251 -> 13
		{54, 0, 18, 49, 13}, //238 -> 14
		{72, 0, 18, 49, 13, 1}, //224
	};

	const int loadingIndices[25] = {
		4, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0,
	};

	PeggleUI(HWND _window, float width, float height);
	~PeggleUI();

	void startLoading();
	void setLoading(int v);
	void stopLoading();

	void hookErr(const wchar_t* msg, void* addressToWriteDllPath); //, void* pathptr);
	void hook();

	void resize(LONG newwidth, LONG newheight);
	void redraw();
	void windowResized(LONG newwidth, LONG newheight);

	void onTimer();
	void beginIntro();
};