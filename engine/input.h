#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <map>
#include <queue>

/*!
    Main input class.

    Encapsulates all input devices, and presents a single, unified interface.
*/

class KeyControl;

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
        void* onPress;
        void* onUnpress;

        operator bool() { return Position() != 0; }
    };

private:
    static std::map<std::string, int> _keymap;
    static bool _keymapinitted;

    std::queue<void*> _hookqueue;

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

    typedef std::map<std::string, Control*>::iterator iterator;
    iterator begin() { return _controls.begin(); }
    iterator end()   { return _controls.end();   }
};

#endif
