#include <cassert>
#include <SDL/SDL.h>

#include "input.h"
#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"
#include "common/log.h"
#include "common/misc.h"

bool InputControl::Pressed()
{
    return Delta() > 0.0f;
}

float InputControl::Delta()
{
    float pos = Position();
    float delta = pos - _oldPos;
    _oldPos = pos;

    return delta;
}

float InputControl::PeekDelta()
{
    return Position() - _oldPos;
}

ScopedPtr<Input> Input::_theInstance = 0;

Input* Input::GetInstance()
{
    if (!_theInstance)
        _theInstance = new Input;
    return _theInstance.get();
}

void Input::Destroy()
{
    if (_theInstance)
        _theInstance = 0;
}

Keyboard* Input::GetKeyboard() const
{
    return _keyboard.get();
}

Mouse* Input::GetMouse() const
{
    return _mouse.get();
}

uint Input::NumJoysticks() const
{
    return SDL_NumJoysticks();
}

Joystick* Input::GetJoystick(uint index)
{
    if (index >= _joysticks.size())
        return 0;

    if (!_joysticks[index])
        _joysticks[index] = new Joystick(index);
    return _joysticks[index];
}

void Input::KeyDown(uint key)
{
    _keyboard->KeyDown(key);
}

void Input::KeyUp(uint key)
{
    _keyboard->KeyUp(key);
}

void Input::JoyAxisMove(uint stick, uint index, uint value)
{
    if (_joysticks[stick])
    {
        AxisControl* axis = _joysticks[stick]->GetAxis(index);
        if (axis->onPress && value != 0)
            QueueEvent(&axis->onPress);
        else if (axis->onUnpress && value == 0)
            QueueEvent(&axis->onUnpress);

        // FIXME: reverse axis events are not raised.
        //ReverseAxisControl* r_axis = _joysticks[stick]->GetReverseAxis(axis);
    }
}

void Input::JoyButtonChange(uint stick, uint index, bool value)
{
    if (_joysticks[stick])
    {
        ButtonControl* button = _joysticks[stick]->GetButton(index);

        if (value && button->onPress)
            QueueEvent(&button->onPress);
        else if (!value && button->onUnpress)
            QueueEvent(&button->onUnpress);
    }
}

void Input::Update()
{
    // All of the current devices update themselves.  Fuckit for now.
    /*_keyboard->Update();
    _mouse->Update();
    for (uint i = 0; i < _joysticks.size(); i++)
    {
        if (_joysticks[i])
            _joysticks[i]->Update();
    }*/
}

void Input::Flush()
{
    _keyboard->ClearKeyQueue();
    _eventQueue = 0;
}

void Input::Unpress()
{
    _keyboard->Unpress();
    _mouse->Unpress();
    for (uint i = 0; i < _joysticks.size(); i++)
    {
        if (_joysticks[i])
            _joysticks[i]->Unpress();
    }
}

void Input::SetStandardControl(InputControl* const& ctrl, InputControl* newControl)
{
    assert(newControl != 0);

    InputControl*& c = const_cast<InputControl*&>(ctrl);
    assert(c != 0);
    c = newControl;
}

ScriptObject* Input::GetQueuedEvent()
{
    ScriptObject* o = _eventQueue;
    _eventQueue = 0;
    return o;
}

void Input::QueueEvent(ScriptObject* script)
{
    _eventQueue = script;
}

Input::Input()
    : _keyboard(new Keyboard)
    , _mouse(new Mouse)
    , up(_up)
    , down(_down)
    , left(_left)
    , right(_right)
    , enter(_enter)
    , cancel(_cancel)
    , _eventQueue(0)
{
    _joysticks.resize(SDL_NumJoysticks());

    _up = _keyboard->GetControl("UP");
    _down = _keyboard->GetControl("DOWN");
    _left = _keyboard->GetControl("LEFT");
    _right = _keyboard->GetControl("RIGHT");
    _enter = _keyboard->GetControl("RETURN");
    _cancel = _keyboard->GetControl("CANCEL");
}

Input::~Input()
{
    for (uint i = 0; i < _joysticks.size(); i++)
        delete _joysticks[i];
}
