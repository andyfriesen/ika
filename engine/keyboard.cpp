
#include "SDL/SDL.h"
#include "keyboard.h"

namespace
{
    // Pulled from the SDL header, and munged a bit with regexps and
    // so forth.
    const struct
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
    const uint numKeys = sizeof(keys) / sizeof(keys[0]);
}

Keyboard::Keyboard()
{
    for (uint i = 0; i < numKeys; i++)
        _nameToKeySym[keys[i].name] = keys[i].code;
}

Keyboard::~Keyboard()
{
    for (KeyMap::iterator
        iter = _keys.begin();
        iter != _keys.end();
        iter++)
    {
        KeyControl* c = iter->second;
        delete c;
    }
}

void Keyboard::Unpress()
{
    for (KeyMap::iterator
        iter = _keys.begin();
        iter != _keys.end();
        iter++)
    {
        iter->second->Unpress();
    }
}

InputControl* Keyboard::GetControl(const std::string& name)
{
    KeySymMap::iterator iter = _nameToKeySym.find(name);
    if (iter == _nameToKeySym.end())
        return 0;

    uint keySym = iter->second;
    KeyMap::iterator iter2 = _keys.find(keySym);
    if (iter2 == _keys.end())
    {
        KeyControl* key = new KeyControl;
        _keys[keySym] = key;
        return key;
    }
    else
        return iter2->second;
}

void Keyboard::KeyDown(uint keyCode)
{
    KeyMap::iterator iter = _keys.find(keyCode);
    if (iter != _keys.end())
        iter->second->KeyDown();

    if (keyCode < 256)
        _keyQueue.push(keyCode);
}

void Keyboard::KeyUp(uint keyCode)
{
    KeyMap::iterator iter = _keys.find(keyCode);
    if (iter != _keys.end())
        iter->second->KeyUp();
}

bool Keyboard::WasKeyPressed() const
{
    return _keyQueue.size() != 0;
}

char Keyboard::GetKey()
{
    if (_keyQueue.size())
    {
        char c = _keyQueue.front();
        _keyQueue.pop();
        return c;
    }
    else
        return 0;
}

void Keyboard::ClearKeyQueue()
{
    while (_keyQueue.size())
        _keyQueue.pop();
}

KeyControl::KeyControl()
    : _pressed(false)
{}

void KeyControl::KeyDown()
{
    _pressed = true;
    if (onPress)
        the< ::Input>()->QueueEvent(&onPress);
}

void KeyControl::KeyUp()
{
    _pressed = false;
    if (onUnpress)
        the< ::Input>()->QueueEvent(&onUnpress);
}

float KeyControl::Position()
{
    return _pressed ? 1.0f : 0.0f;
}