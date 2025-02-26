#pragma once

#include <d2d1.h>
#include <d2d1_3.h>
#include <wincodec.h>
#include <dwrite_3.h>
#include <vector>
#include "Controls.h"
//#include <iostream>
//#include <v8.h>

//using namespace v8;

//template<typename T>
class Direct2D
{
public:
	//https://stackoverflow.com/questions/37285528/drawing-and-creating-non-rectangular-bitmaps-using-direct2d
	// https://learn.microsoft.com/en-us/windows/win32/direct2d/improving-direct2d-performance
	// https://groups.google.com/g/microsoft.public.win32.programmer.directx.graphics/c/aDiqBVY43Jw
	// https://stackoverflow.com/questions/21768180/direct2d-how-to-save-content-of-id2d1rendertarget-to-an-image-file
// 
	//T* renderTarget;
	ID2D1RenderTarget* renderTarget;
	ID2D1Factory7* factory;
	IDWriteFactory7* textfactory; //lol forgo to capitaliez f
	ID2D1SolidColorBrush* clearBrush;
	ID2D1DrawingStateBlock* drawingStateBlock;
	IWICImagingFactory2* wicFactory;
	int type;
	HWND window;
	HDC tempDC;

	bool dirty;
	std::vector<Controls::Parent*> panes;
	Controls::Helper::Hittest hit{-1, NULL};
	//IDWriteTextFormat* textFormat;
	
	Direct2D() {
		factory = nullptr;
		renderTarget = nullptr;
		textfactory = nullptr;
		clearBrush = nullptr;
		drawingStateBlock = nullptr;
		wicFactory = nullptr;
		tempDC = NULL;
		window = NULL;
		type = 0;
		hit = Controls::Helper::Hittest(-1, NULL);
		//textFormat = nullptr;
	}

	virtual ~Direct2D() {
		//std::cout << "d2d -> " << factory << " " << renderTarget << " " << textfactory << " " << clearBrush << " " << drawingStateBlock << std::endl;
		if (factory != nullptr) {
			factory->Release();
		}
		if (this->type < 2 && renderTarget != nullptr) { //direct2d11 (ID2D1DeviceContext) and up use com ptrs which get mad when you try to release them (lol i only just now tried to fix this)
			renderTarget->Release();
		}
		if (textfactory != nullptr) {
			textfactory->Release();
		}
		if (clearBrush != nullptr) {
			clearBrush->Release();
		}
		if (drawingStateBlock != nullptr) {
			drawingStateBlock->Release();
		}
		//panes.clear();
		for (Controls::Parent* pane : panes) {
			delete pane;
		}
		//std::cout << ("D2D DESTROYED") << std::endl;
		//if (textFormat != nullptr) {
		//	textFormat->Release();
		//}
	}

	virtual bool Init(HWND window, int type);

	virtual int Resize(UINT width, UINT height) {
		if (this->type == 0) {
			ID2D1HwndRenderTarget* renderTarget = (ID2D1HwndRenderTarget*)this->renderTarget;
			//info.GetReturnValue().Set(Number::New(isolate, renderTarget->Resize(D2D1::SizeU(IntegerFI(info[0]), IntegerFI(info[1])))));
			return renderTarget->Resize(D2D1::SizeU(width, height));
		}
		else if(this->type == 1) {
			ID2D1DCRenderTarget* renderTarget = (ID2D1DCRenderTarget*)this->renderTarget;
			//i almost recreated the entire d2d object
			//tempDC = GetDC(this->window);
			RECT r{ 0, 0, width, height};
			//info.GetReturnValue().Set(Number::New(isolate, renderTarget->BindDC(dc, &r)));
			return renderTarget->BindDC(tempDC, &r);
		}
	}

	virtual void EndDraw(bool unused);

	void paint();

	LRESULT windowProc(HWND, UINT, WPARAM, LPARAM);

	//void BeginDraw() {
	//V8FUNC(BeginDraw) {
	//	renderTarget->BeginDraw();
	//}
	//
	//V8FUNC(EndDraw) {
	//	renderTarget->EndDraw();
	//}
};