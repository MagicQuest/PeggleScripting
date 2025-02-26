#pragma once

#include "d2d1.h"
#include <vector>

namespace Controls
{
	namespace Helper {
		struct Coords {
			float x;
			float y;
			float width;
			float height;
			inline Coords(float _x, float _y, float _width, float _height) : x(_x), y(_y), width(_width), height(_height) {};
			//inline Coords(Base* base) : x(base->x), y(base->y), width(base->width), height(base->height) {};
		};

		static const HCURSOR cur[12] = {
			LoadCursor(NULL, IDC_ARROW),
			LoadCursor(NULL, IDC_SIZENS),
			LoadCursor(NULL, IDC_SIZEWE),
			LoadCursor(NULL, IDC_SIZEWE),
			LoadCursor(NULL, IDC_SIZENS),
			LoadCursor(NULL, IDC_HAND),
			LoadCursor(NULL, IDC_SIZEALL),
			LoadCursor(NULL, IDC_HELP),
			LoadCursor(NULL, IDC_HAND),
			LoadCursor(NULL, IDC_HAND),
			LoadCursor(NULL, IDC_IBEAM),
			LoadCursor(NULL, (LPCWSTR)32652),
		};

		struct Hittest {
			int type;
			void* data;
			static const int DEFAULT = 0;
			static const int TOP = 1;
			static const int LEFT = 2;
			static const int RIGHT = 3;
			static const int BOTTOM = 4;
			static const int BUTTON = 5;
			static const int MOVE = 6;
			static const int CONTEXTMENU = 7;
			static const int DROP = 8;
			static const int DRAG = 9;
			static const int TEXT = 10;
			static const int WHEEL = 11;
			Hittest() : type(-1), data(NULL) {};
			inline Hittest(int _type, void* _data) : type(_type), data(_data) {};
		};

		bool withinBounds(const Coords& coords, const POINT& pair);

		bool withinBounds(Coords* coords, const POINT& pair);
	}

	class Base {
	public:
		float x;
		float y;
		float width;
		float height;
		bool alwaysHittest = false;
		std::vector<Base*> children;
		std::vector<bool> always_check; //erm the way i did this was weird on the js side...
		Base(float _x, float _y, float _width, float _height, std::vector<Base*> children);
		virtual ~Base();
		virtual Helper::Hittest hittest(POINT mouse) = 0;
		void appendChild(Base* control);
		//void removeChild(int index);
		virtual void mouseDown(POINT mouse);
		virtual void mouseUp(POINT mouse);
		virtual void windowResized(LONG newwidth, LONG newheight);
	};

	class Parent : public Base {
	public:
		Helper::Hittest defaultHT;

		Parent(float _x, float _y, float _width, float _height, std::vector<Base*> children, Helper::Hittest _defaultHittest);
		virtual ~Parent();
		virtual void redraw() {};
		virtual Helper::Hittest hittest(POINT mouse);
	};


	class GenericControl : public Base {
	public:
		GenericControl(float _x, float _y, float _width, float _height);
		void resize(LONG width, LONG height);
		//virtual ~GenericControl();
		//virtual void mouseDown(POINT mouse) {};
		//virtual void mouseUp(POINT mouse) {};
		//virtual Helper::Hittest hittest(POINT mouse) {};
	};

	typedef void (*ButtonCallback) (Base* parent);

	class GenericButton : public GenericControl {
	public:
		bool alwaysHittest = true;
		ButtonCallback callback;
		Base* parent;
		GenericButton(float _x, float _y, float _width, float _height, ButtonCallback _callback, Base* _parent);
		virtual void buttonDown(POINT mouse) {};
	};

	class Bitmap {
	public:
		float x;
		float y;
		float width;
		float height;
		float opacity;
		float rotation;
		ID2D1Bitmap* img;
		Bitmap(float _x, float _y, float _width, float _height, const wchar_t* path, float opacity);
		~Bitmap();
		void transform(float _x, float _y, float _width, float _height);
		void resize(LONG newwidth, LONG newheight);
		virtual void redraw();
	};

	class GenericTiledBitmap : public Bitmap {
	public:
		Helper::Coords* modes;
		int i;
		GenericTiledBitmap(float _x, float _y, float _width, float _height, const wchar_t* path, float opacity, Helper::Coords* _modes, int _i);
		~GenericTiledBitmap();
		void redraw();
	};

	class TiledBitmapButton : public GenericButton {
	public:
		bool visible = true;
		bool down = false;
		GenericTiledBitmap* bitmap;
		//ButtonCallback callback;
		//Base* parent;

		TiledBitmapButton(GenericTiledBitmap* _bitmap, ButtonCallback _callback, Base* _parent);
		~TiledBitmapButton();
		void transform(float _x, float _y, float newwidth, float newheight);
		void redraw();
		void buttonDown(POINT mouse);
		void resize(LONG newwidth, LONG newheight);
		void mouseDown(POINT mouse);
		void mouseUp(POINT mouse);
		Helper::Hittest hittest(POINT mouse);
	};
};

