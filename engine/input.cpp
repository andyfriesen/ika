#include <cassert>
#include <SDL/SDL.h>

#include "input.h"
#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"
#include "common/log.h"
#include "common/utility.h"

InputControl::InputControl()
    : _curPos(0)
    , _oldPos(0)
{}

bool InputControl::Pressed() {
    return Delta() > 0;
}

float InputControl::Position() {
    return _curPos;
}

float InputControl::Delta() {
    float delta = PeekDelta();
    _oldPos = _curPos;
    return delta;
}

float InputControl::PeekDelta() {
    return _curPos - _oldPos;
}

void InputControl::UpdatePosition(float newPos) {
    _oldPos = _curPos;
    _curPos = newPos;
}

ScopedPtr<Input> Input::_theInstance;

Input* Input::getInstance() {
    if (!_theInstance) {
        _theInstance = new Input;
    }

    return _theInstance.get();
}

void Input::Destroy() {
    if (_theInstance) {
        _theInstance = 0;
    }
}

Keyboard* Input::GetKeyboard() const {
    return _keyboard.get();
}

Mouse* Input::GetMouse() const {
    return _mouse.get();
}

uint Input::NumJoysticks() const {
    return SDL_NumJoysticks();
}

Joystick* Input::GetJoystick(uint index) {
    if (index >= _joysticks.size()) {
        return 0;
    }

    if (!_joysticks[index]) {
        _joysticks[index] = new Joystick(index);
    }

    return _joysticks[index];
}

void Input::KeyDown(uint key) {
    _keyboard->KeyDown(key);

    InputControl* ctrl = _keyboard->GetKey(key);
    if (ctrl != 0) {
        UpdateControl(ctrl, true);
    }
}

void Input::KeyUp(uint key) {
    _keyboard->KeyUp(key);

    InputControl* ctrl = _keyboard->GetKey(key);
    if (ctrl != 0) {
        UpdateControl(ctrl, false);
    }
}

void Input::JoyAxisMove(uint stick, uint index, int value) {
    if (stick >= _joysticks.size() || !_joysticks[stick]) return;

    const int EPSILON = 258;

    // Kill nearly-centered values; joysticks aren't always very precise.
    if (abs(value) < EPSILON) {
        value = 0;
    }

    // Cheap hack to get normalization
    if (value == -32768) {
        value++;
    }

    float fvalue = float(value) / 32767;

    InputControl* axis = _joysticks[stick]->GetAxis(index);
    InputControl* reverseAxis = _joysticks[stick]->GetReverseAxis(index);

    UpdateControl(axis, fvalue);
    UpdateControl(reverseAxis, -fvalue);
}

void Input::JoyButtonChange(uint stick, uint index, bool value) {
    if (stick >= _joysticks.size() || !_joysticks[stick]) return;

    InputControl* button = _joysticks[stick]->GetButton(index);

    UpdateControl(button, value);
}

void Input::MouseButtonChange(uint index, bool value) {
    InputControl* button = _mouse->GetButton(index);

    UpdateControl(button, value);
}

void Input::MouseMoved(int x, int y) {
    InputControl* mx = _mouse->GetAxis(Mouse::X);
    InputControl* my = _mouse->GetAxis(Mouse::Y);

    UpdateControl(mx, float(x));
    UpdateControl(my, float(y));
}

void Input::Update() {
    // All of the current devices update themselves.  Fuckit for now.
    /*_keyboard->Update();
    _mouse->Update();
    for (uint i = 0; i < _joysticks.size(); i++)
    {
        if (_joysticks[i])
            _joysticks[i]->Update();
    }*/
}

void Input::Flush() {
    _keyboard->ClearKeyQueue();
    _eventQueue = 0;
}

void Input::Unpress() {
    _keyboard->Unpress();
    _mouse->Unpress();
    for (uint i = 0; i < _joysticks.size(); i++) {
        if (_joysticks[i]) {
            _joysticks[i]->Unpress();
        }
    }
}

void Input::SetStandardControl(InputControl* const& ctrl, InputControl* newControl) {
    assert(newControl != 0);

    InputControl*& c = const_cast<InputControl*&>(ctrl);
    assert(c != 0);
    c = newControl;
}

ScriptObject* Input::GetQueuedEvent() {
    ScriptObject* o = _eventQueue;
    _eventQueue = 0;
    return o;
}

void Input::QueueEvent(ScriptObject* script) {
    if (script) {
        _eventQueue = script;
    }
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
, _eventQueue(0) {
    _joysticks.resize(SDL_NumJoysticks());

    _up = _keyboard->GetControl("UP");
    _down = _keyboard->GetControl("DOWN");
    _left = _keyboard->GetControl("LEFT");
    _right = _keyboard->GetControl("RIGHT");
    _enter = _keyboard->GetControl("RETURN");
    _cancel = _keyboard->GetControl("ESCAPE");
}

Input::~Input() {
    for (uint i = 0; i < _joysticks.size(); i++) {
        delete _joysticks[i];
    }
}

void Input::UpdateControl(InputControl* ctrl, float newValue) {
    if (!ctrl) return;

    ctrl->UpdatePosition(newValue);

    float pd = ctrl->PeekDelta();

    if (pd) {
        if (newValue != 0 && ctrl->onPress) {
            QueueEvent(&ctrl->onPress);

        } else if (ctrl->onUnpress) {
            QueueEvent(&ctrl->onUnpress);
        }
    }
}

void Input::UpdateControl(InputControl* ctrl, int newValue) {
    UpdateControl(ctrl, float(newValue));
}

void Input::UpdateControl(InputControl* ctrl, bool newValue) {
    UpdateControl(ctrl, newValue ? 1.0f : 0.0f);
}
