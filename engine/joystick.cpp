#include <cassert>
#include "joystick.h"
#include "SDL/SDL.h"
#include "common/log.h"

Joystick::Joystick(uint index)
: _joystick(SDL_JoystickOpen(int(index))) {
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

InputControl* Joystick::GetAxis(uint index) {
    if (index >= _axes.size()) {
        return 0;
    }

    if (_axes[index] == 0) {
        _axes[index] = new InputControl;
    }

    return _axes[index];
}

InputControl* Joystick::GetReverseAxis(uint index) {
    if (index >= _axes.size()) {
        return 0;
    }

    if (_reverseAxes[index] == 0) {
        _reverseAxes[index] = new InputControl;
    }

    return _reverseAxes[index];
}

InputControl* Joystick::GetButton(uint index) {
    if (index >= _buttons.size()) {
        return 0;
    }
    
    if (_buttons[index] == 0) {
        _buttons[index] = new InputControl;
    }
    
    return _buttons[index];
}
