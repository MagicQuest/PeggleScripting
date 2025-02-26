#include "Controls.h"
#include "main.h"
#include "goodmacrosfordirect2dandwichelper.h"

using namespace Controls;

bool Helper::withinBounds(const Coords& coords, const POINT& pair) {
    return pair.x > coords.x && pair.y > coords.y && pair.x < coords.x + coords.width && pair.y < coords.y + coords.height;
}

bool Helper::withinBounds(Coords* coords, const POINT& pair) {
    return pair.x > coords->x && pair.y > coords->y && pair.x < coords->x + coords->width && pair.y < coords->y + coords->height;
}

#pragma region Base

Base::Base(float _x, float _y, float _width, float _height, std::vector<Base*> children) {
    this->x = _x;
    this->y = _y;
    this->width = _width;
    this->height = _height;
    if (!children.empty()) {
        for (Base* control : children) {
            this->appendChild(control);
        }
    }
    //this->children.swap(children);
    //this->children = children;
}

Base::~Base() {
    //delete[] this->children;
    //for (const GenericControl& control : this->children) {
        
    //}
    print("Base destructor!");
    //this->children.clear(); //does that call the destructor or what lol
    for (Base* control : this->children) {
        delete control;
    }
}

void Base::appendChild(Base* control) {
    if (control->alwaysHittest) {
        this->alwaysHittest = true;
        //this->always_check.push_back(this->children.size());
    }
    //this->always_check.reserve(this->children.size());
    //this->always_check[this->children.size()] = control->alwaysHittest; //does this make the array bigger if it's too small for this index?
    this->always_check.push_back(control->alwaysHittest);
    this->children.push_back(control);
}

//void Base::removeChild(int index) { //luckily we never call this function so we'll never know if i wrote it wrong or right :)
//    this->children.erase(this->children.begin() + index); //hopefully calls destructor
//    size_t indexToErase = -1;
//    for (size_t i = 0; i < this->always_check.size(); i++) {
//        if (this->always_check[i] == index) {
//            indexToErase = i;
//        }
//        else if (indexToErase != -1u) {
//            this->always_check[i] = this->always_check[i] - 1;
//        }
//    }
//    if (indexToErase == -1u) {
//        print("oops that's wrong!");
//        __debugbreak();
//    }
//
//    this->always_check.erase(this->always_check.begin() + indexToErase);
//}

void Base::mouseDown(POINT mouse) {
    for (Base* control : children) {
        control->mouseDown(mouse);
    }
}

void Base::mouseUp(POINT mouse) {
    for (Base* control : children) {
        control->mouseUp(mouse);
    }
}

void Base::windowResized(LONG newwidth, LONG newheight) {
    for (Base* control : children) {
        control->windowResized(newwidth, newheight);
    }
}

#pragma endregion

#pragma region Parent

Parent::Parent(float _x, float _y, float _width, float _height, std::vector<Base*> children, Helper::Hittest _defaultHittest) : Base(_x, _y, _width, _height, children) {
    this->defaultHT = _defaultHittest;
}

Parent::~Parent() {
    print("parent desteructor (carry on)");
}

Helper::Hittest Parent::hittest(POINT mouse) {
    using namespace Helper;
    if (this->alwaysHittest || withinBounds(Coords(this->x, this->y, this->width, this->height), mouse)) {
        Hittest ht{-1, nullptr};
        
        for (size_t i = 0; i < this->children.size(); i++) {
            Base* control = this->children[i];
            control->hittest(mouse);
            bool ac = this->always_check[i];
            if (withinBounds(Coords(control->x, control->y, control->width, control->height), mouse)) {
                if (ac) {
                    this->always_check[i] = false;
                }
                ht = control->hittest(mouse);
                if (ht.type != -1) {
                    break;
                }
            }
            else if (ac) {
                this->always_check[i] = true;
            }
        }

        for (size_t i = 0; i < this->always_check.size(); i++) {
            if (!this->always_check[i]) {
                continue;
            }
            Base* control = this->children[i];
            Hittest subht = control->hittest(mouse);
            if (ht.type == -1 && subht.type != -1) {
                ht = subht;
            }
        }

        if (ht.type != -1) {
            return ht;
        }

        return this->defaultHT;
    }
    return Hittest(-1, NULL);
}

#pragma endregion

#pragma region GenericControl

GenericControl::GenericControl(float _x, float _y, float _width, float _height) : Base(_x, _y, _width, _height, std::vector<Base*>()) {

}

void GenericControl::resize(LONG width, LONG height) {
    this->width = (float)width;
    this->height = (float)height;
}

#pragma endregion

#pragma region GenericButton

GenericButton::GenericButton(float _x, float _y, float _width, float _height, ButtonCallback _callback, Base* _parent) : GenericControl(_x, _y, _width, _height), callback(_callback), parent(_parent) {
    
}

#pragma endregion

#pragma region Bitmap

Bitmap::Bitmap(float _x, float _y, float _width, float _height, const wchar_t* path, float _opacity) : opacity(_opacity)
{
    this->transform(_x, _y, _width, _height);

    IWICFormatConverter* wicConverter = wic->LoadBitmapFromFilename(path, GUID_WICPixelFormat32bppPBGRA, 0);

    RetIfFailed(d2d->renderTarget->CreateBitmapFromWicBitmap(wicConverter, NULL, &this->img), "failed d2d->renderTarget->CreateBitmapFromWicBitmap");
    wicConverter->Release();

    this->rotation = 0;
}

Bitmap::~Bitmap() {
    print("~releasing Bitmap " << this->img->Release());
}

void Bitmap::transform(float _x, float _y, float _width, float _height) {
    this->x = _x;
    this->y = _y;
    this->width = _width;
    this->height = _height;
}

void Bitmap::resize(LONG newwidth, LONG newheight) {
    this->width = (float)newwidth;
    this->height = (float)newheight;
}

void Bitmap::redraw() {
    //const prev = d2d.GetTransform();
    D2D1_MATRIX_3X2_F prev;
    d2d->renderTarget->GetTransform(&prev);

    //const transform = Matrix3x2F.Multiply(Matrix3x2F.Translation(prev._31+this.x, prev._32+this.y), Matrix3x2F.Rotation(this.rotation, prev._31+this.x+this.width/2, prev._32+this.y+this.height/2));
    D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Translation(prev._31 + this->x, prev._32 + this->y) * D2D1::Matrix3x2F::Rotation(this->rotation, D2D1::Point2F(prev._31 + this->x + this->width / 2, prev._32 + this->y + this->height / 2));
    
    //d2d.SetTransform(transform);
    d2d->renderTarget->SetTransform(transform);
    
    //d2d.DrawBitmap(this.img, 0, 0, this.width, this.height, this.opacity);
    d2d->renderTarget->DrawBitmap(this->img, D2D1::RectF(0, 0, this->width, this->height), this->opacity);

    //d2d.SetTransform(prev);
    d2d->renderTarget->SetTransform(prev);
}

#pragma endregion

#pragma region GenericTiledBitmap

GenericTiledBitmap::GenericTiledBitmap(float _x, float _y, float _width, float _height, const wchar_t* path, float opacity, Helper::Coords* _modes, int _i) : Bitmap(_x, _y, _width, _height, path, opacity)
{
    this->modes = _modes;
    this->i = _i;
}

GenericTiledBitmap::~GenericTiledBitmap() {
    delete[] this->modes;
}

void GenericTiledBitmap::redraw() {
    Helper::Coords coord = this->modes[this->i];
    d2d->renderTarget->DrawBitmap(this->img, D2D1::RectF(this->x, this->y, this->x + this->width, this->y + this->height), this->opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(coord.x, coord.y, coord.x + coord.width, coord.y + coord.height));
}

#pragma endregion

#pragma region TiledBitmapButton

TiledBitmapButton::TiledBitmapButton(/*const GenericTiledBitmap&*/GenericTiledBitmap* _bitmap, ButtonCallback _callback, Base* _parent) : GenericButton(_bitmap->x, _bitmap->y, _bitmap->width, _bitmap->height, _callback, _parent) {
    this->bitmap = _bitmap;
    //this->bitmap = new GenericTiledBitmap(_bitmap);
    //this->callback = _callback;
    //this->parent = _parent;
}

TiledBitmapButton::~TiledBitmapButton() {
    delete this->bitmap;
}

void TiledBitmapButton::transform(float _x, float _y, float newwidth, float newheight) {
    this->x = _x;
    this->y = _y;
    __super::resize(newwidth, newheight); //i wonder what the __super keyword actually does
    this->bitmap->transform(_x, _y, newwidth, newheight);
}

void TiledBitmapButton::redraw() {
    if (this->visible) {
        this->bitmap->redraw();
    }
}

void TiledBitmapButton::buttonDown(POINT mouse) {
    //this->callback();
}

void TiledBitmapButton::resize(LONG newwidth, LONG newheight) {
    __super::resize(newwidth, newheight);
    this->down = false;
}

void TiledBitmapButton::mouseDown(POINT mouse) {
    using namespace Helper;
    //if (Helper::withinBounds((Helper::Coords*)(this+sizeof(ULONG_PTR)), mouse)) { //WINK!!! (honestly i think this could work but you have to make sure you don't include the vtable lmao (wait TiledBitmapButton has no virtual methods so... doesn't that mean it doesn't have one? (jarvis invesitgae)))

    //}
    if (withinBounds(Coords(this->x, this->y, this->width, this->height), mouse)) { //the ole constructor shit worked (nevermind :( )
        this->bitmap->x += 1;
        this->bitmap->y += 1;
        this->down = true;
        PlaySoundSpecial(std::string(__dirname) + "/psgui/button.mp3", "buttonclick", NULL);
    }
}

void TiledBitmapButton::mouseUp(POINT mouse) {
    using namespace Helper;
    if (withinBounds(Coords(this->x, this->y, this->width, this->height), mouse) && this->down) {
        this->bitmap->x -= 1;
        this->bitmap->y -= 1;
        this->down = false;
        this->callback(this->parent);
    }
}

Helper::Hittest TiledBitmapButton::hittest(POINT mouse) {
    using namespace Helper;

    this->bitmap->i = 0;

    if (this->visible) {
        if (withinBounds(Coords(this->x, this->y, this->width, this->height), mouse)) {
            this->bitmap->i = 1;
            return Hittest(Hittest::BUTTON, this);
        }
        else if(this->down) {
            this->down = false;
            this->bitmap->x -= 1;
            this->bitmap->y -= 1;
        }
    }

    return Hittest(-1, nullptr);
}

#pragma endregion
