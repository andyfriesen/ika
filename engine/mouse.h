
#ifndef MOUSE_H
#define MOUSE_H

#include "input.h"
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

private:
    ScopedPtr<MouseAxisControl> _xAxis;
    ScopedPtr<MouseAxisControl> _yAxis;
    ScopedPtr<MouseAxisControl> _wheel;
    ScopedPtr<MouseButtonControl> _left;
    ScopedPtr<MouseButtonControl> _right;
    ScopedPtr<MouseButtonControl> _middle;
};

struct MouseAxisControl : InputControl {
    MouseAxisControl(uint axis);

protected:
    virtual float GetPosition();

private:
    uint _axis;
};

struct MouseButtonControl : InputControl {
    MouseButtonControl(uint index);

protected:
    virtual float GetPosition();

private:
    uint _index;
};

#endif
