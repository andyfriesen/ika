/*
 * Logging stuff.
 *
 * Not sure if the singleton pattern is the best way to handle this stuff.  Not an issue right now anyway. --andy
 */

#pragma once

#include "common/utility.h"

//#define LOG_CALLBACK

namespace Log {
    void Init(const char* fname);
    void Write(const char*, ...);        // writes to the log file
    void Write(const std::string& s);
    void Writen(const char*, ...);       // same, but does not append a newline
    void Writen(const std::string& s);
};

#ifdef LOG_CALLBACK

    class CCallbackLog {
        static std::string sHistory;

        int nOldlen;
    public:
        CCallbackLog(const char* s);
        ~CCallbackLog();
    };

#   define CDEBUG(x) CCallbackLog __callbacklogger(x)

#else
#   define CDEBUG(x)
#endif

