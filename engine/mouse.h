
#ifndef MOUSE_H
#define MOUSE_H

#include "input.h"
#include "keyboard.h" // EVIL
#include "common/utility.h"

struct MouseAxisControl;
struct MouseButtonControl;

struct Mouse : InputDevice {

    enum MouseAxis {
        X,
        Y,
        WHEEL
    };

    Mouse();

    virtual void Update() {}
    virtual void Unpress();
    virtual InputControl* GetControl(const std::string& name);

    MouseAxisControl* GetAxis(MouseAxis axis);
    MouseButtonControl* GetButton(uint button);

    void Motion(float x, float y);
    void Clicked(uint button, bool isPressed);

private:
    ScopedPtr<MouseAxisControl> _xAxis;
    ScopedPtr<MouseAxisControl> _yAxis;
    ScopedPtr<MouseAxisControl> _wheel;
    ScopedPtr<MouseButtonControl> _left;
    ScopedPtr<MouseButtonControl> _right;
    ScopedPtr<MouseButtonControl> _middle;
};

struct MouseAxisControl : InputControl {
    MouseAxisControl()
        : _pos(0)
    {}

    void SetPosition(float newPos);

protected:
    virtual float GetPosition();

private:
    float _pos;
};

struct MouseButtonControl : KeyControl {
};

#endif
