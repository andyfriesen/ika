#include <cassert>
#include "joystick.h"
#include "SDL/SDL.h"
#include "common/log.h"

// Joystick values read to be less than this are trimmed to 0.
static const int EPSILON = 258;

Joystick::Joystick(uint index)
: _joystick(SDL_JoystickOpen(index)) {
    assert(_joystick);

    _axes.resize(SDL_JoystickNumAxes(_joystick));
    _reverseAxes.resize(_axes.size());
    _buttons.resize(SDL_JoystickNumButtons(_joystick));
}

Joystick::~Joystick() {
    assert(_joystick && SDL_JoystickOpened(SDL_JoystickIndex(_joystick)));

    for (uint i = 0; i < _axes.size(); i++) {
        delete _axes[i];
        delete _reverseAxes[i];
    }

    for (uint i = 0; i < _buttons.size(); i++) {
        delete _buttons[i];
    }
        
    SDL_JoystickClose(_joystick);
}

void Joystick::Unpress() {
    for (uint i = 0; i < _axes.size(); i++) {
        _axes[i]->Unpress();
        _reverseAxes[i]->Unpress();
    }

    for (uint i = 0; i < _buttons.size(); i++) {
        _buttons[i]->Unpress();
    }
}

InputControl* Joystick::GetControl(const std::string& name) {
    if (name[0] == 'A') {
        uint axis = atoi(name.substr(1).c_str());
        return GetAxis(axis);

    } else if (name[0] == 'R') {
        uint axis = atoi(name.substr(1).c_str());
        return GetReverseAxis(axis);

    } else if (name[0] == 'B') {
        uint button = atoi(name.substr(1).c_str());
        return GetButton(button);

    } else {
        return 0;
    }
}

uint Joystick::GetNumAxes() const {
    return _axes.size();
}

uint Joystick::GetNumButtons() const {
    return _buttons.size();
}

AxisControl* Joystick::GetAxis(uint index) {
    if (index >= _axes.size()) {
        return 0;
    }

    if (_axes[index] == 0) {
        _axes[index] = new AxisControl(_joystick, index);
    }

    return _axes[index];
}

ReverseAxisControl* Joystick::GetReverseAxis(uint index) {
    if (index >= _axes.size()) {
        return 0;
    }

    if (_reverseAxes[index] == 0) {
        _reverseAxes[index] = new ReverseAxisControl(_joystick, index);
    }

    return _reverseAxes[index];
}

ButtonControl* Joystick::GetButton(uint index) {
    if (index >= _buttons.size()) {
        return 0;
    }
    
    if (_buttons[index] == 0) {
        _buttons[index] = new ButtonControl(_joystick, index);
    }
    
    return _buttons[index];
}

AxisControl::AxisControl(_SDL_Joystick* j, uint index)
: _joystick(j)
, _index(index) {
    assert(_joystick);
}

bool AxisControl::GetPressed() {
    // Axes are considered "pressed" if they are now moved beyond the 0.5 point, but were not before

    float delta = PeekDelta();
    float pos = Position();

    bool value = (
        (pos > 0.5f) &&
        (pos - delta <= 0.5f)
    );

    return value;
}

float AxisControl::GetPosition() {
    int i = SDL_JoystickGetAxis(_joystick, _index);
    
    // Kill nearly-centered values; joysticks aren't always very precise.
    if (abs(i) < EPSILON) {
        i = 0;
    }

    // Cheap hack to get normalization
    if (i == -32768) {
        i++;
    }

    return float(i) / 32767;
}

ReverseAxisControl::ReverseAxisControl(_SDL_Joystick* j, uint index)
: AxisControl(j, index) {
}

float ReverseAxisControl::GetPosition() {
    return -AxisControl::GetPosition();
}

ButtonControl::ButtonControl(_SDL_Joystick* j, uint index)
: _joystick(j)
, _index(index) {
    assert(_joystick);
}

float ButtonControl::GetPosition() {
    return float(SDL_JoystickGetButton(_joystick, _index));
}
