#ifndef INPUT_H
#define INPUT_H

#include <windows.h>
#include <dinput.h>
#include <queue>

const int nControls=20;                                     // ugh

/*!
    Main input class.

    Encapsulates all input devices, and presents a single, unified interface.
*/
class Input
{
private:
    // TODO: also make a struct for a joystick button.

    //! Information regarding a single keyboard key
    struct SKey
    {
        bool    bPressed;                                   //!< true if the key is down
        int     nControl;                                   //!< the index of the virtual control this key is bound to
        bool    bIsbound;                                   //!< true if nControl is valid
    };
    
    // handles
    HINSTANCE hInst;                                        //!< app handle
    HWND      hWnd;                                         //!< window handle
    
    LPDIRECTINPUT       lpdi;                               //!< DirectInput instance
    LPDIRECTINPUTDEVICE keybd;                              //!< Keyboard instance
    // LPDIRECTINPUTDEVICE joystick;                        //!< Joystick(s)?
    
    // key buffer
    //! keep this, or std::queue?  I rather like this setup, it's so elegant.  But a little limiting.
    unsigned char key_buffer[256];                          //!< 256 entry long key queue
    unsigned char kb_start,kb_end;                          //!< start/end index of keyboard buffer.  Since they're bytes, they automaticly wrap around.
    
    std::queue<char> controlbuffer;                         //!< virtual control event queue
    
    RECT    mclip;                                          //!< The rect that the mouse cursor is confined to
    
    bool    Test(HRESULT result,char* errmsg);              //!< Little test function to make the init code cleaner.
    
public:
    
    Input();                                                //!< constructor
    ~Input();                                               //!< destructor
    
    int     Init(HINSTANCE hinst,HWND hwnd);                //!< Initialization
    void    ShutDown();                                     //!< Cleanup
    
    void    Poll();                                         //!< updates key queue and key array
    void    Update();                                       //!< updates left, down, etc...
    
    // Virtual controls (a control is basically a button)
    bool    control[nControls];                             //!< virtual key thingies    
    bool&   up;
    bool&   down;                                                 
    bool&   left;
    bool&   right;
    bool&   enter;
    bool&   cancel;
    
    int     NextControl();                                  //!< returns the code of the last button pressed
    void    ClearControls();                                //!< clears the control queue
    void    BindKey(int nButtonidx,int nKeycode);           //!< Links nKeycode with nButtonidx
    void    UnbindKey(int nKeycode);                        //!< unbinds a key.
    
    // Keyboard stuff
    SKey    keys[256];                                      //!< Current key states
    char    last_pressed;                                   //!< last key event here
    
    int     GetKey();                                       //!< returns the next key in the queue
    void    StuffKey(int key);                              //!< adds the key to the queue, as if it had been pressed
    void    ClearKeys();                                    //!< Clears the key queue
    char    Scan2ASCII(int scancode);                       //!< Converts a keyboard scan code to its ASCII equivalent
    
    // mouse stuff
    int        mousex,mousey;                               //!< Current mouse coordinates
    int        mouseb;                                      //!< Current mouse state. (bitmask)
    
    void    MoveMouse(int x,int y);                         //!< Sets the mouse position
    void    UpdateMouse();                                  //!< Polls the hardware for changes in the mouse's state.
    void    ClipMouse(int x1,int y1,int x2,int y2);         //!< Clips the mouse to the specified rect.  The user will not be able to move the mouse cursor outside of this rect.
    void    HideMouse();                                    //!< Hides the win32 mouse cursor
    void    ShowMouse();                                    //!< Shows the win32 mouse cursor
};

#endif
