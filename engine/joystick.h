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
    AxisControl* GetAxis(uint index);
    ReverseAxisControl* GetReverseAxis(uint index);

    ButtonControl* GetButton(uint index);

private:
    struct _SDL_Joystick* _joystick;

    std::vector<AxisControl*> _axes;
    std::vector<ReverseAxisControl*> _reverseAxes;
    std::vector<ButtonControl*> _buttons;
};

struct AxisControl : InputControl {
    friend struct Joystick;

protected:
    AxisControl(_SDL_Joystick* j, uint index);

    virtual bool  GetPressed();
    virtual float GetPosition();

private:
    _SDL_Joystick* _joystick;
    uint _index;
};

// Reverse of the PositiveAxisControl.  Duh.
struct ReverseAxisControl : AxisControl {
    friend struct Joystick;

protected:
    virtual float GetPosition();

private:
    ReverseAxisControl(_SDL_Joystick* j, uint index);
};

struct ButtonControl : public InputControl {
    friend struct Joystick;

protected:
    virtual float GetPosition();

private:
    ButtonControl(_SDL_Joystick* j, uint index);

    _SDL_Joystick* _joystick;
    uint _index;
};

#endif
