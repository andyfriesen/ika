#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <map>
#include <queue>

// >_<
#include "script.h"

/**
 *  Main input class.
 *
 *  Encapsulates all input devices, and presents a single, unified interface.
 */
class KeyControl;
class ScriptObject;

class Input
{
    friend class Control;

public:
    class Control
    {
    private:
        Input* _parent;

    protected:
        Control(Input* p) 
            : _parent(p)
            , onPress(0)
            , onUnpress(0)
        {}

    public:
        virtual bool  Pressed()  = 0;
        virtual float Position() = 0;
        virtual float Delta()    = 0;
        virtual ~Control(){}

    public:
        // event thingies.  Python objects go here.
        ScriptObject onPress;
        ScriptObject onUnpress;

        operator bool() { return Position() != 0; }
    };

private:
    static std::map<std::string, int> _keyMap;
    static bool _keyMapInitted;

    std::queue<void*> _hookQueue;
    std::queue<char> _keyQueue;

    Control* _up;
    Control* _down;
    Control* _left;
    Control* _right;
    Control* _enter;
    Control* _cancel;

protected:
    std::map<std::string, Control*> _controls;  // Name : control pairs.
    
    // keyboard things go here.  They can be handled efficiently if we can get them efficiently through their keysym.
    // Something to point out is that this is considered strictly aggregate.  _controls holds *all* input controls.
    // This essentially boils down to a speed hack. (it also simplifies the KeyUp and KeyDown methods considerably.
    std::map<int, KeyControl*> _keys;

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

    typedef std::map<std::string, Control*>::iterator iterator;
    iterator begin() { return _controls.begin(); }
    iterator end()   { return _controls.end();   }

    void Unpress(const std::string& name);
    void Unpress(int i);
    void Unpress();
};

#endif
