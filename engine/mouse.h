
#ifndef MOUSE_H
#define MOUSE_H

#include "input.h"
#include "common/misc.h"

class MouseAxisControl;
class MouseButtonControl;

class Mouse : public InputDevice
{
public:
    enum MouseAxis
    {
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

class MouseAxisControl : public InputControl
{
public:
    MouseAxisControl(uint axis);

    virtual float Position();

private:
    uint _axis;
};

class MouseButtonControl : public InputControl
{
public:
    MouseButtonControl(uint index);

    virtual float Position();

private:
    uint _index;
};

#endif
