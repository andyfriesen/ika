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
    inline  void  Unpress() { Position(); }

    // event thingies.  Python objects go here.
    ScriptObject onPress;
    ScriptObject onUnpress;

    operator bool() { return Position() != 0; }

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

    static Input* _theInstance;

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

/**
 * Main input class.
 *
 * Encapsulates all input devices, and presents a single, unified interface.
 * Controls are created on demand, and remain in existence until termination of
 * the engine.
 */
/*class Input
{
private:
    static Input* _theInstance;

    typedef std::map<std::string, int> KeyTableMap;
    typedef std::map<std::string, Control*> ControlMap;
    typedef std::map<int, KeyControl*> KeyMap;

    static KeyTableMap _keyTable;
    static bool _staticInit;

    ScriptObject*    _hookQueue;
    std::queue<char> _keyQueue;

    std::vector<Joystick*> _joysticks;

    Control* _up;
    Control* _down;
    Control* _left;
    Control* _right;
    Control* _enter;
    Control* _cancel;

    ControlMap _controls;  // Name : control pairs.
    
    // keyboard things go here.  They can be handled more efficiently if we can get them through their keysym.
    // Something to point out is that this is considered strictly aggregate.  _controls holds *all* input controls.
    // This essentially boils down to a speed hack. (it also simplifies the KeyUp and KeyDown methods considerably)
    KeyMap _keys;

    Input();
    ~Input();

public:
    static Input* getInstance();

    Control& Up()       { return *_up;      }
    Control& Down()     { return *_down;    }
    Control& Left()     { return *_left;    }
    Control& Right()    { return *_right;   }
    Control& Enter()    { return *_enter;   }
    Control& Cancel()   { return *_cancel;  }

    void KeyDown(int key);
    void KeyUp(int key);

    Control* GetControl(const std::string& name);
    Control* operator[](const std::string& name)    {   return GetControl(name);    }

    void* GetNextControlEvent();    // returns 0 if the event queue is empty
    void  ClearEventQueue();

    char GetKey();
    void ClearKeyQueue();
    bool WasKeyPressed() const;

    typedef ControlMap::iterator iterator;
    iterator begin() { return _controls.begin(); }
    iterator end()   { return _controls.end();   }

    uint GetJoystickCount() const;
    Joystick* GetJoystick(uint index);

    void Unpress(const std::string& name);  // Unpresses the control with the given name
    void Unpress(int i);                    // Unpresses the key control with the given keysym
    void Unpress();                         // Unpresses everything.

    void Flush();

};*/

#endif
