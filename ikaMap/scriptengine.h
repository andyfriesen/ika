#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "python.h"
#include "script.h"

class MainWindow;

/*
 * Python and C++ don't get along very well.
 * 'Private' stuff is in an anon namespace in scriptengine.cpp
 * It may be necessary to split the Python object definition stuff into additional source files.
 * It sure as hell was in ika, after all. -_-
 * An important thing to note is that we *MUST* assume that there is only one MainWindow instance,
 * because there is only one Python interpreter.  A switch to Lua may be necessary if this becomes
 * a serious issue, but I sincerely doubt that.
 */
namespace ScriptEngine
{
    void Init(MainWindow* mw);
    void ShutDown();
}

#endif