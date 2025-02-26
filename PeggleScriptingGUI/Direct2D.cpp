#include "Direct2D.h"
#include <string>
#include <iostream>
#include "goodmacrosfordirect2dandwichelper.h"
#include <windowsx.h>
#include "main.h"
//#include <typeinfo>

//#define err(shit, cstring) MessageBoxA(NULL, std::to_string(shit).c_str(), cstring, MB_OK | MB_ICONERROR);

//template <typename T>
bool Direct2D::Init(HWND window, int type) {
	/*HRESULT shit = */SusIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory), "Graphics Init Create Factory");
	//if (shit != S_OK) {
	//	err(shit, "Graphics Init Create Factory");
	//	return false;
	//}

	RECT rect{ 0 };// GetClientRect(window, &rect);
	//I DIDN'T INITIALIZE RECT AND IT WAS FULL OF GARBAGE
	if (window == NULL) {
		rect.right = GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	else {
		GetClientRect(window, &rect);
	}

	//if constexpr (std::is_same_v<ID2D1RenderTarget, T>) {
	if(type == 0) {
		/*shit = */SusIfFailed(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(window, D2D1::SizeU(rect.right, rect.bottom)), (ID2D1HwndRenderTarget**)&renderTarget), "Graphics Init Create HWND Render Target");

		//if (shit != S_OK) {
		//	err(shit, "Graphics Init Create DC Render Target");
		//	err(GetLastError(), "Graphics Init DC Create Render Target");
		//	return false;
		//}
	}else if(type == 1) {
	//else if constexpr (std::is_same_v<ID2D1DCRenderTarget, T>) {
		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED), //having this set to ignore seriously made using CreateBitmap->CopyFromBitmap(WICBitmap) stop working (lemme explain)
			//i used fnBmp = CreateBitmapFromFilename("...")
			//then made another bitmap with cBitmap = CreateBitmap(...)
			//i wanted to copy the fnBmp to the cBitmap so
			//i used cBitmap.CopyFromBitmap(..., fnBmp, ...)
			//and it failed because cBitmap had a different pixel format and here was the source of that difference
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);
		/*shit = */SusIfFailed(factory->CreateDCRenderTarget(&props, (ID2D1DCRenderTarget**)&renderTarget), "Graphics Init Create DC Render Target");

		//if (shit != S_OK) {
		//	err(shit, "Graphics Init Create DC Render Target");
		//	err(GetLastError(), "Graphics Init DC Create Render Target");
		//	return false;
		//}

		//HDC dc = GetDC(window);
		tempDC = GetDC(window);

		HRESULT shit = ((ID2D1DCRenderTarget*)renderTarget)->BindDC(tempDC, &rect); //yo i forgot to release it (no way this is the reason why layeredwindow.js stopped working! i released the dc and EndDraw failed!)

		//ReleaseDC(window, dc); //yeah nah you can't release it after binding it you gotta keep that shit

		//if (shit != S_OK) {
			//err(shit, "Graphics Bind DC");
			SusIfFailed(shit, "Graphics Bind DC");
			//return false;
		//}
	}

	/*shit = */SusIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), (IUnknown**)&textfactory), "Graphics Init Direct Write Instantiation");
	//if (shit != S_OK) {
	//	err(shit, "Graphics Init Direct Write Instantiation"); //big ass word for no reason
	//	return false;
	//}

	/*shit = */ContIfFailed(renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &clearBrush), "Graphics Init Clear Brush (this isn't actually required so you can continue as normal (you cannot d2d.Clear with opacity anymore) )"); //idk if this is required because im pretty sure you can already clear with opacity?
	//if (shit != S_OK) {
	//	MessageBoxA(NULL, "Graphics Init Clear Brush", "this isn't actually required so you can continue as normal (you cannot d2d.Clear with opacity anymore)", MB_OK | MB_ICONINFORMATION);
	//}

	//shit = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);
	
	//if (shit != S_OK) {
	//    MessageBoxA(NULL, "yo shit FUCKED UP co create instance WIC image", "yeah we failed to create the wic factory (for loading bitmaps/pictures)", MB_OK | MB_ICONERROR);
	//    //return;
	//}

	/*shit = */ContIfFailed(factory->CreateDrawingStateBlock(D2D1::DrawingStateDescription(), &drawingStateBlock), "HELP create drawing state block did NOT work (cannot Save/Restore DrawingState)");//D2D1::DrawingStateDescription();
	
	//if (shit != S_OK) {
	//	MessageBoxA(NULL, "HELP create drawing state block did NOT work (cannot Save/Restore DrawingState)", "uhhhh we need some HELP!", MB_OK | MB_ICONERROR);
	//	//return false;
	//}

	this->type = type;
	this->window = window;

	return true;
	//err(21, typeid(T).name());
}

void Direct2D::EndDraw(bool unused) {
	RetPrintIfFailed(this->renderTarget->EndDraw(), "Direct2D EndDraw failed?");
}

void Direct2D::paint() {
	this->renderTarget->BeginDraw();

	for (Controls::Parent* pane : panes) {
		this->renderTarget->SaveDrawingState(this->drawingStateBlock);

		float x = pane->x;
		float y = pane->y;
		//if (!pane->sticky) {

		//}
		this->renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(x, y));
		pane->redraw();
		this->renderTarget->RestoreDrawingState(this->drawingStateBlock);
	}

	this->EndDraw(NULL);

	//throwawayObjects
}

LRESULT Direct2D::windowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	LRESULT returnval = 0;

	if (msg == WM_CREATE) {
		this->paint();
	}
	else if (msg == WM_PAINT) {
		//this->dirty = true;
	}
	else if (msg == WM_SIZE) {
		LONG wid = LOWORD(lp);
		LONG hei = HIWORD(lp);
		this->Resize(wid, hei);
		for (Controls::Parent* pane : panes) {
			pane->windowResized(wid, hei);
		}
		//this->dirty = true;
	}
	else if (msg == WM_SETCURSOR) {
		POINT mouse; GetCursorPos(&mouse);
		ScreenToClient(hwnd, &mouse);

		//Controls::Helper::Hittest hit{-1, NULL};

		//print(this->panes.size());

		//yo im actually not gonna lie im never using size_t for loops again
		//for some reason every SINGLE time i use it there's some situation where i need it to be negative
		for (int i = this->panes.size() - 1; i >= 0; i--) {
			Controls::Parent* pane = this->panes[i];
			//print(pane << " " << i << " how?");
			POINT mca = POINT(mouse);
			//if (!pane->sticky) {

			//}
			POINT clientmouse = POINT{mca.x-(LONG)pane->x, mca.y-(LONG)pane->y};

			Controls::Helper::Hittest ht = pane->hittest(clientmouse);

			if (ht.type != -1) {
				this->hit.type = ht.type;
				this->hit.data = ht.data;
				break;
			}
		}

		if (this->hit.type != -1) {
			//print(this->hit.type << " " << (this->hit.type != -1));
			SetCursor(Controls::Helper::cur[this->hit.type]);
			//this->dirty = true;
			returnval = 1;
		}
	}
	else if (msg == WM_LBUTTONDOWN) {
		SetCapture(hwnd);
		POINT mouse = POINT{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};

		if (this->hit.type != -1) {
			switch (this->hit.type) {
				case Controls::Helper::Hittest::BUTTON:
					((Controls::GenericButton*)this->hit.data)->buttonDown(mouse);
					break;

				case Controls::Helper::Hittest::MOVE:
					break;

				case Controls::Helper::Hittest::CONTEXTMENU:
				case Controls::Helper::Hittest::WHEEL:
					break;

				case Controls::Helper::Hittest::DRAG:
					break;

				case Controls::Helper::Hittest::DROP:
					break;

				case Controls::Helper::Hittest::TEXT:
					break;

				default:
					break;
			}
		}

		for (Controls::Parent* pane : panes) {
			if (Controls::Helper::withinBounds(Controls::Helper::Coords(pane->x, pane->y, pane->width, pane->height), mouse)) {
				POINT mca = POINT(mouse);
				//if (!pane->sticky) {

				//}
				pane->mouseDown(mca);
			}
		}

		//Editable.editing
	}
	else if (msg == WM_RBUTTONDOWN) {
		POINT mouse = POINT{ GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };

		//Editable.editing

		if (this->hit.type == this->hit.BUTTON) {
			//
		}
		else if (this->hit.type == this->hit.CONTEXTMENU) {
			//
		}

		//for (Controls::Parent* pane : panes) {
		//	if (Controls::Helper::withinBounds(pane, mouse)) {
		//		POINT mca = POINT(mouse);
		//		//if (!pane->sticky) {
		//
		//		//}
		//		pane->rightMouseDown(mca);
		//	}
		//}
	}
	else if (msg == WM_LBUTTONUP) {
		ReleaseCapture();
		//Draggable.mouseUp();

		POINT mouse = POINT{ GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };

		//activeButton

		for (Controls::Parent* pane : panes) {
			if (Controls::Helper::withinBounds(Controls::Helper::Coords(pane->x, pane->y, pane->width, pane->height), mouse)) {
				POINT mca = POINT(mouse);
				//if (!pane->sticky) {

				//}
				pane->mouseUp(mca);
			}
		}
	}
	else if (msg == WM_COMMAND) {
		if (this->hit.type == hit.CONTEXTMENU) {
			//
			this->dirty = true;
		}
	}
	else if (msg == WM_KEYDOWN) {
		//
	}
	else if (msg == WM_CHAR) {
		//
	}
	else if (msg == WM_MOUSEWHEEL) {
		//
	}
	else if (msg == WM_DESTROY) {
		//delete this;
	}

	if (this->dirty) {
		this->paint();
		this->dirty = false;
	}

	return returnval;
}