#include <cassert>
#include <SDL/SDL.h>

#include "input.h"

std::map<std::string, int> Input::_keymap;
bool Input::_keymapinitted = false;

namespace
{
    struct
    {
        const char* name;
        SDLKey code;
    } keys[] =
    {
        {"BACKSPACE", SDLK_BACKSPACE},
        {"TAB", SDLK_TAB},
        {"CLEAR", SDLK_CLEAR},
        {"RETURN", SDLK_RETURN},
        {"PAUSE", SDLK_PAUSE},
        {"ESCAPE", SDLK_ESCAPE},
        {"SPACE", SDLK_SPACE},
        {"EXCLAIM", SDLK_EXCLAIM},
        {"QUOTEDBL", SDLK_QUOTEDBL},
        {"HASH", SDLK_HASH},
        {"DOLLAR", SDLK_DOLLAR},
        {"AMPERSAND", SDLK_AMPERSAND},
        {"QUOTE", SDLK_QUOTE},
        {"LEFTPAREN", SDLK_LEFTPAREN},
        {"RIGHTPAREN", SDLK_RIGHTPAREN},
        {"ASTERISK", SDLK_ASTERISK},
        {"PLUS", SDLK_PLUS},
        {"COMMA", SDLK_COMMA},
        {"MINUS", SDLK_MINUS},
        {"PERIOD", SDLK_PERIOD},
        {"SLASH", SDLK_SLASH},
        {"0", SDLK_0},
        {"1", SDLK_1},
        {"2", SDLK_2},
        {"3", SDLK_3},
        {"4", SDLK_4},
        {"5", SDLK_5},
        {"6", SDLK_6},
        {"7", SDLK_7},
        {"8", SDLK_8},
        {"9", SDLK_9},
        {"COLON", SDLK_COLON},
        {"SEMICOLON", SDLK_SEMICOLON},
        {"LESS", SDLK_LESS},
        {"EQUALS", SDLK_EQUALS},
        {"GREATER", SDLK_GREATER},
        {"QUESTION", SDLK_QUESTION},
        {"AT", SDLK_AT},
        {"LEFTBRACKET", SDLK_LEFTBRACKET},
        {"BACKSLASH", SDLK_BACKSLASH},
        {"RIGHTBRACKET", SDLK_RIGHTBRACKET},
        {"CARET", SDLK_CARET},
        {"UNDERSCORE", SDLK_UNDERSCORE},
        {"BACKQUOTE", SDLK_BACKQUOTE},
        {"a", SDLK_a},
        {"b", SDLK_b},
        {"c", SDLK_c},
        {"d", SDLK_d},
        {"e", SDLK_e},
        {"f", SDLK_f},
        {"g", SDLK_g},
        {"h", SDLK_h},
        {"i", SDLK_i},
        {"j", SDLK_j},
        {"k", SDLK_k},
        {"l", SDLK_l},
        {"m", SDLK_m},
        {"n", SDLK_n},
        {"o", SDLK_o},
        {"p", SDLK_p},
        {"q", SDLK_q},
        {"r", SDLK_r},
        {"s", SDLK_s},
        {"t", SDLK_t},
        {"u", SDLK_u},
        {"v", SDLK_v},
        {"w", SDLK_w},
        {"x", SDLK_x},
        {"y", SDLK_y},
        {"z", SDLK_z},
        {"DELETE", SDLK_DELETE},
        {"KP0", SDLK_KP0},
        {"KP1", SDLK_KP1},
        {"KP2", SDLK_KP2},
        {"KP3", SDLK_KP3},
        {"KP4", SDLK_KP4},
        {"KP5", SDLK_KP5},
        {"KP6", SDLK_KP6},
        {"KP7", SDLK_KP7},
        {"KP8", SDLK_KP8},
        {"KP9", SDLK_KP9},
        {"KP_PERIOD", SDLK_KP_PERIOD},
        {"KP_DIVIDE", SDLK_KP_DIVIDE},
        {"KP_MULTIPLY", SDLK_KP_MULTIPLY},
        {"KP_MINUS", SDLK_KP_MINUS},
        {"KP_PLUS", SDLK_KP_PLUS},
        {"KP_ENTER", SDLK_KP_ENTER},
        {"KP_EQUALS", SDLK_KP_EQUALS},
        {"UP", SDLK_UP},
        {"DOWN", SDLK_DOWN},
        {"RIGHT", SDLK_RIGHT},
        {"LEFT", SDLK_LEFT},
        {"INSERT", SDLK_INSERT},
        {"HOME", SDLK_HOME},
        {"END", SDLK_END},
        {"PAGEUP", SDLK_PAGEUP},
        {"PAGEDOWN", SDLK_PAGEDOWN},
        {"F1", SDLK_F1},
        {"F2", SDLK_F2},
        {"F3", SDLK_F3},
        {"F4", SDLK_F4},
        {"F5", SDLK_F5},
        {"F6", SDLK_F6},
        {"F7", SDLK_F7},
        {"F8", SDLK_F8},
        {"F9", SDLK_F9},
        {"F10", SDLK_F10},
        {"F11", SDLK_F11},
        {"F12", SDLK_F12},
        {"F13", SDLK_F13},
        {"F14", SDLK_F14},
        {"F15", SDLK_F15},
        {"NUMLOCK", SDLK_NUMLOCK},
        {"CAPSLOCK", SDLK_CAPSLOCK},
        {"SCROLLOCK", SDLK_SCROLLOCK},
        {"RSHIFT", SDLK_RSHIFT},
        {"LSHIFT", SDLK_LSHIFT},
        {"RCTRL", SDLK_RCTRL},
        {"LCTRL", SDLK_LCTRL},
        {"RALT", SDLK_RALT},
        {"LALT", SDLK_LALT},
        {"RMETA", SDLK_RMETA},
        {"LMETA", SDLK_LMETA},
        {"LSUPER", SDLK_LSUPER},
        {"RSUPER", SDLK_RSUPER},
        {"MODE", SDLK_MODE},
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
    _up     = GetControl("UP");     _up->AddRef();
    _down   = GetControl("DOWN");   _down->AddRef();
    _left   = GetControl("LEFT");   _left->AddRef();
    _right  = GetControl("RIGHT");  _right->AddRef();
    _enter  = GetControl("RETURN"); _enter->AddRef();
    _cancel = GetControl("ESCAPE"); _cancel->AddRef();
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
    int i = _keymap[name];
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