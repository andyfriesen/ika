
#include <cassert>
#include "SDL/SDL.h"
#include "mouse.h"

Mouse::Mouse()
    : _xAxis(new MouseAxisControl)
    , _yAxis(new MouseAxisControl)
    , _wheel(new MouseAxisControl)
    , _left(new MouseButtonControl)
    , _right(new MouseButtonControl)
    , _middle(new MouseButtonControl)
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

void Mouse::Motion(float x, float y) {
    _xAxis->SetPosition(x);
    _yAxis->SetPosition(y);
}

void Mouse::Clicked(uint button, bool isPressed) {
    // EVIL

    if (isPressed) {
        switch (button) {
            case 0: _left->KeyDown(); break;
            case 1: _right->KeyDown(); break;
            case 2: _middle->KeyDown(); break;
            default: assert(false);
        }
    } else {
        switch (button) {
            case 0: _left->KeyUp(); break;
            case 1: _right->KeyUp(); break;
            case 2: _middle->KeyUp(); break;
            default: assert(false);
        }
    }
}

MouseAxisControl* Mouse::GetAxis(MouseAxis axis) {
    switch (axis) {
    case X: return _xAxis.get();
    case Y: return _yAxis.get();
    case WHEEL: return _wheel.get();
    default:    return 0;
    }
}

MouseButtonControl* Mouse::GetButton(uint button) {
    switch (button) {
        case 1: return _left.get();
        case 2: return _right.get();
        case 3: return _middle.get();
        default: return 0;
    }
}

void MouseAxisControl::SetPosition(float newPos) {
    _pos = newPos;
}

float MouseAxisControl::GetPosition() {
    return _pos;
}
