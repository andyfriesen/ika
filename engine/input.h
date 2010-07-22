#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <map>
#include <vector>
#include <queue>

#include "common/utility.h"
#include "common/utility.h"
#include "scriptobject.h"

struct Joystick;
struct Keyboard;
struct Mouse;

struct InputControl;

struct InputDevice {
    friend struct Input;

    virtual ~InputDevice(){}

    virtual void Update()  = 0;                             // poll the device
    virtual void Unpress() = 0;                             // resets the delta of every control to 0

    // The only polymorphic way to get a control. (subclasses usually
    // offer a better way than with arbitrary string identifiers)
    virtual InputControl* GetControl(const std::string& name) = 0;
};

struct InputControl {
    friend struct Input;

    InputControl();
    virtual ~InputControl() {}

    bool  Pressed();
    float Position();
    float Delta();
    float PeekDelta();

    // Pretty looking method for 'unpressing'.
    inline void Unpress() {
        Pressed(); 
    }

    // event thingies.  Python objects go here.
    ScriptObject onPress;
    ScriptObject onUnpress;

    operator bool() { return Position() > 0; }

    void UpdatePosition(float newPos);

private:
    // Used to implement Pressed() and Delta()
	float  _curPos;    
	float  _oldPos;    
};

// TODO: some kind of control compositor, so we can have a single
// control instance that responds to more than one source of input.

// Input singleton.  Handles everything.
struct Input {
    friend struct InputDevice;
    friend struct InputControl;
    friend struct ScopedPtr<Input>;

    static Input* getInstance();
    static void Destroy();

    Keyboard* GetKeyboard() const;
    Mouse*    GetMouse() const;
    uint      NumJoysticks() const;
    Joystick* GetJoystick(uint index); // not const because we may be creating it

    // Event handling stuff.  We relay to the appropriate control.
    // This structure makes hooking scripts to controls really simple.
    void KeyDown(uint key);
    void KeyUp(uint key);
    void JoyAxisMove(uint stick, uint axis, int value);
    void JoyButtonChange(uint stick, uint button, bool value);
    void MouseMoved(int x, int y);
    void MouseButtonChange(uint button, bool value);

    // Poll all connected devices
    void Update();

    // Flush key and event buffers
    void Flush();

    // Unpress everything.
    void Unpress();

    // teehee.
    void SetStandardControl(InputControl* const& ctrl, InputControl* newControl);

    ScriptObject* GetQueuedEvent();
    void QueueEvent(ScriptObject* script);

private:
    Input();
    ~Input();

    void UpdateControl(InputControl* ctrl, int newValue);
    void UpdateControl(InputControl* ctrl, bool newValue);
    void UpdateControl(InputControl* ctrl, float newValue);

    static ScopedPtr<Input> _theInstance;

    ScopedPtr<Keyboard> _keyboard;
    ScopedPtr<Mouse>    _mouse;
    std::vector<Joystick*> _joysticks;

public:

	// Standard controls:
	InputControl* const& up;
	InputControl* const& down;
	InputControl* const& left;
	InputControl* const& right;
	InputControl* const& enter;
	InputControl* const& cancel;

private:

    // Actual standard controls.
    InputControl* _up;
    InputControl* _down;
    InputControl* _left;
    InputControl* _right;
    InputControl* _enter;
    InputControl* _cancel;

    // onPress/onUnpress queue.
    ScriptObject* _eventQueue;

    // NO.
    Input(Input&);
    Input& operator=(Input&);
};

#endif
