#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <map>
#include <vector>
#include <queue>

#include "common/types.h"
#include "scriptobject.h"

class KeyControl;

/**
 *  Main input class.
 *
 *  Encapsulates all input devices, and presents a single, unified interface.
 */
class Input
{
    friend class Control;

public:
    class Control
    {
    private:
        Input* _parent;
        float  _oldPos; // for Pressed and Delta

    protected:
        Control(Input* p) 
            : _parent(p)
            , onPress(0)
            , onUnpress(0)
        {}

    public:
        virtual bool  Pressed();
        virtual float Position() = 0;
        virtual float Delta();
        virtual float PeekDelta();
        virtual ~Control(){}

    public:
        // event thingies.  Python objects go here.
        ScriptObject onPress;
        ScriptObject onUnpress;

        operator bool() { return Position() != 0; }
    };

private:
    typedef std::map<std::string, int> KeyTableMap;
    typedef std::map<std::string, Control*> ControlMap;
    typedef std::map<int, KeyControl*> KeyMap;

    static KeyTableMap _keyTable;
    static bool _staticInit;

    ScriptObject*    _hookQueue;
    std::queue<char> _keyQueue;

    //std::vector<Joystick*> _joysticks;

    Control* _up;
    Control* _down;
    Control* _left;
    Control* _right;
    Control* _enter;
    Control* _cancel;

    ControlMap _controls;  // Name : control pairs.
    
    // keyboard things go here.  They can be handled efficiently if we can get them efficiently through their keysym.
    // Something to point out is that this is considered strictly aggregate.  _controls holds *all* input controls.
    // This essentially boils down to a speed hack. (it also simplifies the KeyUp and KeyDown methods considerably.
    KeyMap _keys;

public: 
    Input();
    ~Input();

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
    uint GetAxisCount(uint joyIndex) const;
    uint GetButtonCount(uint joyIndex) const;
    std::string GetJoystickName(uint joyIndex) const;

    void Unpress(const std::string& name);
    void Unpress(int i);
    void Unpress();

    void Flush();

};

#endif
