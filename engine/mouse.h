
#ifndef MOUSE_H
#define MOUSE_H

#include "input.h"

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

    virtual void Update();
    virtual InputControl* GetControl(const std::string& name);

    MouseAxisControl* GetAxis(MouseAxis axis);
    MouseButtonControl* GetButton(uint button);

private:
    MouseAxisControl _xAxis;
    MouseAxisControl _yAxis;
    MouseAxisControl _wheel;
    MouseButtonControl _left;
    MouseButtonControl _right;
    MouseButtonControl _middle;
};

#endif