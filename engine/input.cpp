#include <cassert>
#include <SDL/SDL.h>

#include "input.h"
#include "common/log.h"

std::map<std::string, int> Input::_keyMap;
bool Input::_keyMapInitted = false;

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
        {"A", SDLK_a},
        {"B", SDLK_b},
        {"C", SDLK_c},
        {"D", SDLK_d},
        {"E", SDLK_e},
        {"F", SDLK_f},
        {"G", SDLK_g},
        {"H", SDLK_h},
        {"I", SDLK_i},
        {"J", SDLK_j},
        {"K", SDLK_k},
        {"L", SDLK_l},
        {"M", SDLK_m},
        {"N", SDLK_n},
        {"O", SDLK_o},
        {"P", SDLK_p},
        {"Q", SDLK_q},
        {"R", SDLK_r},
        {"S", SDLK_s},
        {"T", SDLK_t},
        {"U", SDLK_u},
        {"V", SDLK_v},
        {"W", SDLK_w},
        {"X", SDLK_x},
        {"Y", SDLK_y},
        {"Z", SDLK_z},
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

public:
    KeyControl(Input* p, int code)
        : Control(p)
        , _pressed(false)
        , _down(false)
    {}

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
        return a;
    }

    virtual float Position()    {   return _down ? 1.0f : 0.0f; }
    virtual float Delta()       {   return Pressed() ? 1.0f : 0.0f; }
};

/*class CompositeControl : public Control
{
    // NYI
};*/

Input::Input()
    : _hookQueue(0)
{
    if (!_keyMapInitted)
    {
        const int len = sizeof(keys) / sizeof(keys[0]);
        for (int i = 0; i < len; i++)
            _keyMap[keys[i].name] = keys[i].code;

        _keyMapInitted = true;
    }

    // hurk.  Gay.
    _up     = GetControl("UP");
    _down   = GetControl("DOWN");
    _left   = GetControl("LEFT");
    _right  = GetControl("RIGHT");
    _enter  = GetControl("RETURN");
    _cancel = GetControl("ESCAPE");
}

Input::~Input()
{
    for (std::map<int, KeyControl*>::iterator i = _keys.begin(); i != _keys.end(); i++)
    {
        KeyControl* kc = i->second;
        delete kc;
    }
}

void Input::KeyDown(int key)
{
    if (key < 255)
        _keyQueue.push(key);

    KeyControl* c = _keys[key];
    if (c)
    {
        c->Press();

        if (_hookQueue == 0)
            _hookQueue = &c->onPress;
    }
}

void Input::KeyUp(int key)
{
    KeyControl* c = _keys[key];
    if (c)
    {
        c->Unpress();

        if (_hookQueue == 0)
            _hookQueue = &c->onUnpress;
    }
}

Input::Control* Input::GetControl(const std::string& name)
{
    int i = _keyMap[name];
    if (i)
    {
        KeyControl* c = _keys[i];
        if (!c)
        {
            c = new KeyControl(this, i);
            _keys[i] = c;
            _controls[name] = c;
        }
        return c;
    }

    return 0;
}

void* Input::GetNextControlEvent()
{
    if (_hookQueue != 0)
    {
        void* p = _hookQueue->get();
        _hookQueue = 0;
        return p;
    }
    else
        return 0;
}

void Input::ClearEventQueue()
{
    _hookQueue = 0;
}

char Input::GetKey()
{
    if (_keyQueue.empty())
        return 0;
    else
    {
        char c = _keyQueue.front();
        _keyQueue.pop();
        return c;
    }
}

void Input::ClearKeyQueue()
{
    while (!_keyQueue.empty())
        _keyQueue.pop();
}

void Input::Unpress(const std::string& name)
{
    GetControl(name)->Pressed();
}

void Input::Unpress(int i)
{
    Control* c = _keys[i];
    if (c)
        c->Pressed();
}

void Input::Unpress()
{
    for (std::map<std::string, Control*>::iterator i = _controls.begin(); i != _controls.end(); i++)
        i->second->Pressed();
}

void Input::Flush()
{
    ClearKeyQueue();
    Unpress();
    ClearEventQueue();
}