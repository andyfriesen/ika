/*
 * I didn't want to do this, but the joystick stuff is proving to be complicated in its own right.
 * Enough so that it needs its ows source file.  blech.
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <vector>
#include <string>
#include "input.h"

struct AxisControl;
struct ReverseAxisControl;
struct ButtonControl;
struct _SDL_Joystick;

struct Joystick : InputDevice {
    Joystick(uint index);
    ~Joystick();

    virtual void Update() {}
    virtual void Unpress();
    virtual InputControl* GetControl(const std::string& name);

    uint GetNumAxes() const;
    uint GetNumButtons() const;
    InputControl* GetAxis(uint index);
    InputControl* GetReverseAxis(uint index);

    InputControl* GetButton(uint index);

private:
    struct _SDL_Joystick* _joystick;

    std::vector<InputControl*> _axes;
    std::vector<InputControl*> _reverseAxes;
    std::vector<InputControl*> _buttons;
};

#endif
