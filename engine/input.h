// Input module for v2.6 by the Speed Bump
// This file is GPL.
// In short: there is NO WARRANTY on this, you can't keep it secret, and you can't steal credit from me.

#ifndef w_input_h
#define w_input_h

#include <windows.h>
#include <dinput.h>
#include <queue>

const int nControls=20;                                     // ugh

class Input
{
private:
    // TODO: also make a struct for a joystick button.
    struct SKey
    {
        bool    bPressed;                                   // true if the key is down
        int     nControl;                                   // the index of the virtual control this key is bound to
        bool    bIsbound;                                   // true if nControl is valid
    };
    
    // handles
    HINSTANCE hInst;                                        // app handle
    HWND      hWnd;                                         // window handle
    
    // DirectInput objects
    LPDIRECTINPUT       lpdi;
    LPDIRECTINPUTDEVICE keybd;
    LPDIRECTINPUTDEVICE mouse;
    // LPDIRECTINPUTDEVICE joystick;
    
    // key buffer
    // keep this, or std::queue?  I rather like this setup, it's so elegant.  But a little limiting.
    unsigned char key_buffer[256];                          // remember up to 256 events
    unsigned char kb_start,kb_end;                          // start/end of keyboard buffer.  Since they're bytes, they automaticly wrap around.
    
    std::queue<char> controlbuffer;                         // virtual control event queue
    
    RECT    mclip;
    
    int        Test(HRESULT result,char* errmsg);
    
public:
    
    Input();                                                // constructor
    ~Input();                                               // destructor
    
    int        Init(HINSTANCE hinst,HWND hwnd);
    void    ShutDown();
    
    void    Poll();                                         // updates key queue and key array
    void    Update();                                       // updates left, down, etc...
    
    // Virtual controls (a control is basically a button)
    bool    control[nControls];                             // virtual key thingies
    bool&    up;                                            // "standard" controls.
    bool&    down;
    bool&    left;
    bool&    right;
    bool&    enter;
    bool&    cancel;
    
    int        NextControl();                               // returns the code of the last button pressed
    void    ClearControls();                                // clears the control queue
    void    BindKey(int nButtonidx,int nKeycode);           // Links nKeycode with nButtonidx
    void    UnbindKey(int nKeycode);                        // unbinds a key.
    
    // Keyboard stuff
    SKey    keys[256];
    char    last_pressed;                                   // last key event here
    
    int        GetKey();                                    // returns the next key in the queue
    void    StuffKey(int key);                              // adds the key to the queue, as if it had been pressed
    void    ClearKeys();
    char    Scan2ASCII(int scancode);                       // returns the ASCII code. ;)
    
    // mouse stuff
    int        mousex,mousey;                               // mouse coordinates
    int        mouseb;
    
    void    MoveMouse(int x,int y);
    void    UpdateMouse();
    void    ClipMouse(int x1,int y1,int x2,int y2);
    void    HideMouse();
    void    ShowMouse();
};

#endif
