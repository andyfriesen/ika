#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <map>
#include <vector>
#include <queue>

#include "common/types.h"
#include "common/misc.h"
#include "scriptobject.h"

class KeyControl;
class Joystick;
class Keyboard;
class Mouse;

class InputControl;

class InputDevice
{
    friend class Input;

public:
    virtual ~InputDevice(){}

    virtual void Update()  = 0;                             // poll the device
    virtual void Unpress() = 0;                             // resets the delta of every control to 0

    // The only polymorphic way to get a control. (subclasses usually
    // offer a better way than with arbitrary string identifiers)
    virtual InputControl* GetControl(const std::string& name) = 0;
};

class InputControl
{
    friend class Input;
public:
    virtual ~InputControl(){}
    virtual bool  Pressed();
    virtual float Position() = 0;
    virtual float Delta();
    virtual float PeekDelta();

    // Pretty looking method for 'unpressing'.
    inline  void  Unpress() 
    {
        Pressed(); 
    }

    // event thingies.  Python objects go here.
    ScriptObject onPress;
    ScriptObject onUnpress;

    operator bool() { return Position() > 0; }

private:
    float  _oldPos; // for Pressed and Delta
};

// TODO: some kind of control compositor, so we can have a single
// control instance that responds to more than one source of input.

// Input singleton.  Handles everything.
class Input
{
    friend class InputDevice;
    friend class InputControl;
    friend class ScopedPtr<Input>;

public:
    static Input* GetInstance();
    static void Destroy();

    Keyboard* GetKeyboard() const;
    Mouse*    GetMouse() const;
    uint      NumJoysticks() const;
    Joystick* GetJoystick(uint index); // not const because we may be creating it

    // Event handling stuff.  We relay to the appropriate control.
    // This structure makes hooking scripts to controls really simple.
    void KeyDown(uint key);
    void KeyUp(uint key);
    void JoyAxisMove(uint stick, uint axis, uint value);
    void JoyButtonChange(uint stick, uint button, bool value);
    void MouseMoved(int x, int y);
    void MouseButtonChange(uint button, bool value);

    // Poll all connected devices
    void Update();

    // Flush key and event buffers
    void Flush();

    // Unpress everything.
    void Unpress();

    // Standard controls:
    InputControl* const& up;
    InputControl* const& down;
    InputControl* const& left;
    InputControl* const& right;
    InputControl* const& enter;
    InputControl* const& cancel;

    // teehee.
    void SetStandardControl(InputControl* const& ctrl, InputControl* newControl);

    ScriptObject* GetQueuedEvent();
    void QueueEvent(ScriptObject* script);

private:
    Input();
    ~Input();

    static ScopedPtr<Input> _theInstance;

    ScopedPtr<Keyboard> _keyboard;
    ScopedPtr<Mouse>    _mouse;
    std::vector<Joystick*> _joysticks;

    // Actual standard controls.
    InputControl* _up;
    InputControl* _down;
    InputControl* _left;
    InputControl* _right;
    InputControl* _enter;
    InputControl* _cancel;

    // onPress/onUnpress queue.
    ScriptObject* _eventQueue;
};

#endif
