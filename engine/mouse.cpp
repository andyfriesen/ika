
#include <cassert>
#include "SDL/SDL.h"
#include "mouse.h"

Mouse::Mouse()
    : _xAxis(new InputControl)
    , _yAxis(new InputControl)
    , _wheel(new InputControl)
    , _left(new InputControl)
    , _right(new InputControl)
    , _middle(new InputControl)
{
}

void Mouse::Unpress() {
    _xAxis->Unpress();
    _yAxis->Unpress();
    _wheel->Unpress();
    _left->Unpress();
    _right->Unpress();
    _middle->Unpress();
}

InputControl* Mouse::GetControl(const std::string& name) {
     //hurk
    if (name == "X")            return _xAxis.get();
    else if (name == "Y")       return _yAxis.get();
    else if (name == "WHEEL")   return _wheel.get();
    else if (name == "LEFT")    return _left.get();
    else if (name == "RIGHT")   return _right.get();
    else if (name == "MIDDLE")  return _middle.get();
    else                        return 0;
}

InputControl* Mouse::GetAxis(MouseAxis axis) {
    switch (axis) {
        case X: return _xAxis.get();
        case Y: return _yAxis.get();
        case WHEEL: return _wheel.get();
        default:    return 0;
    }
}

InputControl* Mouse::GetButton(uint button) {
    switch (button) {
        case 1: return _left.get();
        case 2: return _right.get();
        case 3: return _middle.get();
        default: {
            return 0;
            //throw std::runtime_error(va("Asked for invalid mouse button %i", button));
        }
    }
}
