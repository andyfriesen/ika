
#ifndef TIMER_H
#define TIMER_H

#include <windows.h>
#include <mmsystem.h>

/*!
    Timer callback encapsulation stuff.  Pretty dull, really.
*/

class Timer
{
    UINT curtimer;
public:
    int t;		//!< read/write timer.  Mangled frequently.
    int systime;	//!< read-only timer
    
    bool Init(int Hz);  //!< Starts calling 
    void Shutdown();    //!< Shutdown. ;P
    ~Timer();

    //! The callback.  Called however often.
    static void CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);
};

#endif