
#include <cassert>
#include "SDL/SDL.h"
#include "mouse.h"

Mouse::Mouse()
    : _xAxis(new MouseAxisControl(X))
    , _yAxis(new MouseAxisControl(Y))
    , _wheel(new MouseAxisControl(WHEEL))
    , _left(new MouseButtonControl(1))
    , _right(new MouseButtonControl(2))
    , _middle(new MouseButtonControl(3))
{
}

void Mouse::Unpress()
{
    _xAxis->Unpress();
    _yAxis->Unpress();
    _wheel->Unpress();
    _left->Unpress();
    _right->Unpress();
    _middle->Unpress();
}

InputControl* Mouse::GetControl(const std::string& name)
{
     //hurk
    if (name == "X")            return _xAxis.get();
    else if (name == "Y")       return _yAxis.get();
    else if (name == "WHEEL")   return _wheel.get();
    else if (name == "LEFT")    return _left.get();
    else if (name == "RIGHT")   return _right.get();
    else if (name == "MIDDLE")  return _middle.get();
    else                        return 0;
}

MouseAxisControl* Mouse::GetAxis(MouseAxis axis)
{
    switch (axis)
    {
    case X: return _xAxis.get();
    case Y: return _yAxis.get();
    case WHEEL: return _wheel.get();
    default:    return 0;
    }
}

MouseButtonControl* Mouse::GetButton(uint button)
{
    switch (button)
    {
    case 1: return _left.get();
    case 2: return _right.get();
    case 3: return _middle.get();
    default: return 0;
    }
}

MouseAxisControl::MouseAxisControl(uint axis)
    : _axis(axis)
{}

float MouseAxisControl::GetPosition()
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    
    // gay.
    switch (_axis)
    {
    case Mouse::X: return float(x);
    case Mouse::Y: return float(y);
    case Mouse::WHEEL: return 0; // NYI
    default:
        assert(false);
        return 0; // make the compiler shut up.  Never actually happens.
    }
}

MouseButtonControl::MouseButtonControl(uint index)
    : _index(index)
{}

float MouseButtonControl::GetPosition()
{
    int b = SDL_GetMouseState(0, 0);
    return b & SDL_BUTTON(_index) ? 1.0f : 0.0f;
}
