
#include "log.h"
#include "utility.h"

#include <stdarg.h>
#include <stdio.h>

#include <ios>
#include <iostream>
#include <fstream>
#include <set>

using std::ios;
using std::endl;

// -----------------------

namespace Log {
    bool isLogging = false;
    std::string logName;

    std::ofstream logFile;

    std::set<std::string> flags;
};

// -----------------------

void Log::Init(const char* fname) {
    if (isLogging) {
        return;
    }
    isLogging = true;

    remove(fname);

    logName = fname;

    logFile.open(fname);

#ifdef LOG_CALLBACK
    remove("callback.log");
#endif
};

void Log::Writen(const char* s, ...) {
    if (!isLogging) {
        return;
    }

    va_list lst;
    va_start(lst, s);

#ifdef _MSC_VER
    int bufferLength = _vscprintf(s, lst) + 1; // plus terminating null
    ScopedArray<char> buffer = new char[bufferLength];
    vsprintf(buffer.get(), s, lst);
    logFile << buffer.get();

#elif defined(__GNUC__)
    char* buffer = 0;
    int len = vasprintf(&buffer, s, lst);

    if (len == -1) {
        logFile << "Log system error: Unable to write some stuff.";

    } else {
        logFile << buffer;
        ::free(buffer);
    }

#else
#   error Buffer overrun gayness that must be overcome!
    char buffer[1024];
    vsprintf(buffer, s, lst);
    logFile << buffer;
#endif

    va_end(lst);
}

void Log::Write(const char* s, ...) {
    if (!isLogging) {
        return;
    }

    va_list lst;
    va_start(lst, s);

#ifdef _MSC_VER
    int bufferLength = _vscprintf(s, lst) + 1; // plus terminating null
    ScopedArray<char> buffer = new char[bufferLength];
    vsprintf(buffer.get(), s, lst);
    logFile << buffer.get();

#elif defined(__GNUC__)
    char* buffer = 0;
    int len = vasprintf(&buffer, s, lst);

    if (len == -1) {
        logFile << "Log system error: Unable to write some stuff.";

    } else {
        logFile << buffer;
        ::free(buffer);
    }

#else
#   error Buffer overrun gayness that must be overcome!
    char buffer[1024];
    vsprintf(buffer, s, lst);
    logFile << buffer;
#endif

    logFile << endl;

    va_end(lst);
}

void Log::Write(const std::string& s) {
    logFile << s << endl;
}

void Log::Writen(const std::string& s) {
    logFile << s;
}

// -----------------------

#ifdef LOG_CALLBACK

string CCallbackLog::sHistory;

CCallbackLog::CCallbackLog(const char* s)
    : nOldlen(sHistory.length())
{
    sHistory += "-> ";
    sHistory += s;

    std::ofstream f("callback.log", ios::app);
    f << sHistory << std::endl;
}

CCallbackLog::~CCallbackLog() {
    sHistory = sHistory.substr(0, nOldlen);
}

#endif
