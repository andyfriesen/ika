
#include <cassert>
#include "SDL/SDL.h"
#include "mouse.h"

Mouse::Mouse()
    : _xAxis(X)
    , _yAxis(Y)
    , _wheel(WHEEL)
    , _left(1)
    , _right(2)
    , _middle(3)
{
}

void Mouse::Update()
{
    // No-op
}

InputControl* Mouse::GetControl(const std::string& name)
{
    // hurk
    if (name == "X")            return &_xAxis;
    else if (name == "Y")       return &_yAxis;
    else if (name == "WHEEL")   return &_wheel;
    else if (name == "LEFT")    return &_left;
    else if (name == "RIGHT")   return &_right;
    else if (name == "MIDDLE")  return &_middle;
    else                        return 0;
}

MouseAxisControl* Mouse::GetAxis(MouseAxis axis)
{
    switch (axis)
    {
    case X: return &_xAxis;
    case Y: return &_yAxis;
    case WHEEL: return &_wheel;
    default:    return 0;
    }
}

MouseButtonControl* Mouse::GetButton(uint button)
{
    switch (button)
    {
    case 0: return &_left;
    case 1: return &_right;
    case 2: return &_middle;
    default: return 0;
    }
}

MouseAxisControl::MouseAxisControl(uint axis)
    : _axis(axis)
{}

float MouseAxisControl::Position()
{
    int x, y;
    int b = SDL_GetMouseState(&x, &y);
    
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

float MouseButtonControl::Position()
{
    int b = SDL_GetMouseState(0, 0);
    return b & SDL_BUTTON(_index) ? 1.0f : 0.0f;
}