#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "Python.h"
#include "script.h"

struct MainWindow;

/*
 * Python and C++ don't get along very well.
 * 'Private' stuff is in an anon namespace in scriptengine.cpp
 * It may be necessary to split the Python object definition stuff into additional source files.
 * It sure as hell was in ika, after all. -_-
 * An important thing to note is that we *MUST* assume that there is only one MainWindow instance,
 * because there is only one Python interpreter.  A switch to Lua may be necessary if this becomes
 * a serious issue, but I sincerely doubt that.
 *
 * This design sucks.  Badly.
 */
namespace ScriptEngine
{
    // Just tells the script engine where to find the main window. :P
    void Init(MainWindow* mw);

    // Actually inits.  The Script constructor calls this.  This way, Python doesn't
    // get initialized until it is actually needed for something.
    // (redundant Init() invokations are safe)
    void Init();

    // Shutdown. :P
    void ShutDown();
}

#endif