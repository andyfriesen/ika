#include <cassert>
#include <SDL/SDL.h>

#include "input.h"

std::map<const char*, int> Input::_keymap;
bool Input::_keymapinitted = false;

namespace
{
    struct
    {
        const char* name;
        SDLKey code;
    } keys[] =
    {
        { "A", SDLK_a },
        { "Enter", SDLK_RETURN },
    };
}

class KeyControl : public Input::Control
{
    bool _pressed;  // true if the key was pressed lately
    bool _down;     // true if the key is down right now
    int _code;

public:
    KeyControl(Input* p, int code)
        : Control(p)
        , _pressed(false)
        , _down(false)
        , _code(code)
    {}

    ~KeyControl()
    {
        UnregisterControl(_code);
    }

    void Press()
    {
        _pressed = true;
        _down = true;
    }

    void Unpress()
    {
        _down = false;
    }

    virtual bool Pressed()
    {
        bool a = _pressed;
        _pressed = false;
        return _pressed;
    }

    virtual float Position()    {   return _down ? 1.0f : 0.0f; }
    virtual float Delta()       {   return _down ? 1.0f : 0.0f; }
};

/*class CompositeControl : public Control
{
    // NYI
};*/

void Input::Control::AddRef()
{
    _refcount++;
}

void Input::Control::Release()
{
    _refcount--;
    if (_refcount == 0)
        delete this;
}

void Input::Control::UnregisterControl(const std::string& name)
{
    _parent->Unregister(name);
}

void Input::Control::UnregisterControl(int k)
{
    _parent->Unregister(k);
}

Input::Input()
{
    if (!_keymapinitted)
    {
        const int len = sizeof(keys) / sizeof(keys[0]);
        for (int i = 0; i < len; i++)
            _keymap[keys[i].name] = keys[i].code;

        _keymapinitted = true;
    }

    // hurk.  Gay.
    _up     = (*this)["UpKey"];     _up->AddRef();
    _down   = (*this)["DownKey"];   _down->AddRef();
    _left   = (*this)["LeftKey"];   _left->AddRef();
    _right  = (*this)["RightKey"];  _right->AddRef();
    _enter  = (*this)["EnterKey"];  _enter->AddRef();
    _cancel = (*this)["Escape"];    _cancel->AddRef();
}

Input::~Input()
{
    _up->Release();
    _down->Release();
    _left->Release();
    _right->Release();
    _enter->Release();
    _cancel->Release();
    assert(_controls.size() == 0);
}

void Input::Unregister(int keycode)
{
    assert(_keys.count(keycode));

    _keys.erase(keycode);
}

void Input::Unregister(const std::string& name)
{
    assert(_controls.count(name));

    _controls.erase(name);
}

void Input::KeyDown(int key)
{
    KeyControl* c = _keys[key];
    if (c)
    {
        c->Press();

        if (c->onPress)
            _hookqueue.push(c->onPress);
    }
}

void Input::KeyUp(int key)
{
    KeyControl* c = _keys[key];
    if (c)
    {
        c->Unpress();

        if (c->onUnpress)
            _hookqueue.push(c->onUnpress);
    }
}

Input::Control* Input::GetControl(const std::string& name)
{
    int i = _keymap[name.c_str()];
    if (i)
    {
        KeyControl* c = new KeyControl(this, i);
        _keys[i] = c;
        c->AddRef();
        return c;
    }

    return 0;
}

void* Input::GetNextControlEvent()
{
    if (_hookqueue.empty())
        return 0;

    void* p = _hookqueue.front();
    _hookqueue.pop();

    return p;
}

void Input::ClearEventQueue()
{
    while (_hookqueue.size())
        _hookqueue.pop();
}