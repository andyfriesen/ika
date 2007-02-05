#pragma once

#include "input.h"
#include "common/utility.h"

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

    InputControl* GetAxis(MouseAxis axis);
    InputControl* GetButton(uint button);

private:
    ScopedPtr<InputControl> _xAxis;
    ScopedPtr<InputControl> _yAxis;
    ScopedPtr<InputControl> _wheel;
    ScopedPtr<InputControl> _left;
    ScopedPtr<InputControl> _right;
    ScopedPtr<InputControl> _middle;
};
