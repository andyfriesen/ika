/*
 * I didn't want to do this, but the joystick stuff is proving to be complicated in its own right.
 * Enough so that it needs its ows source file.  blech.
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <vector>
#include <string>
#include "input.h"

class AxisControl;
class ReverseAxisControl;
class ButtonControl;
struct _SDL_Joystick;

class Joystick : public InputDevice
{
public:
    Joystick(uint index);
    ~Joystick();

    virtual void Update() {}
    virtual void Unpress();
    virtual InputControl* GetControl(const std::string& name);

    uint GetNumAxes() const;
    uint GetNumButtons() const;
    AxisControl* GetAxis(uint index);
    ReverseAxisControl* GetReverseAxis(uint index);

    ButtonControl* GetButton(uint index);

private:
    struct _SDL_Joystick* _joystick;

    std::vector<AxisControl*> _axes;
    std::vector<ReverseAxisControl*> _reverseAxes;
    std::vector<ButtonControl*> _buttons;
};

class AxisControl : public InputControl
{
    friend class Joystick;
public:
    virtual bool Pressed();
    virtual float Position();

protected:
    AxisControl(_SDL_Joystick* j, uint index);

private:
    _SDL_Joystick* _joystick;
    uint _index;
};

// Reverse of the PositiveAxisControl.  Duh.
class ReverseAxisControl : public AxisControl
{
    friend class Joystick;

public:
    virtual bool Pressed();
    virtual float Position();

private:
    ReverseAxisControl(_SDL_Joystick* j, uint index);
};

class ButtonControl : public InputControl
{
    friend class Joystick;

public:
    virtual float Position();

private:
    ButtonControl(_SDL_Joystick* j, uint index);

    _SDL_Joystick* _joystick;
    uint _index;
};

#endif
