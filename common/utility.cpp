/*
 * Details in utility.h
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "utility.h"

bool isPowerOf2(uint i) {
    return (i & (i - 1)) == 0;
}

// http://bob.allegronetwork.com/prog/tricks.html#nextPowerOf2
int nextPowerOf2(int i) {
    i--;
    i |= (i >> 1);
    i |= (i >> 2);
    i |= (i >> 4);
    i |= (i >> 8);
    i |= (i >> 16);
    i++;
    return i;
}

int sgn(int x) {
    if (x < 0) {
        return -1;
    }
    if (x > 0) {
        return  1;
    }
    return 0;
}

char* va(const char* format, ...) {
    va_list argptr;
    static char str[1024];
    
    va_start(argptr, format);
#ifdef WIN32
    _vsnprintf(str, sizeof(str)-1, format, argptr);
#else
    vsnprintf(str, sizeof(str)-1, format, argptr);
#endif
    va_end(argptr);
    
    return str;
}

void SeedRandom() {
    time_t curTime;
    time(&curTime);
    srand(u32(curTime));
}

int Random(int min, int max) {
    int i;

    if (min == max) {
        // :P
        return min;
    }
    
    if (max < min) {
        swap(max, min);
    }
    
    i = (rand() % (max - min)) + min;
    
    return i;
}

#include <string>
#include <sstream>

const std::string trim(const std::string& s) {
    uint start = 0;
    uint end = s.length();

    while (s[start] == ' ' && start < s.length()) {
        start++;
    }

    while (s[end - 1] == ' ' && end > start) {
        end--;
    }

    if (start >= end) {
        return "";
    } else {
        return s.substr(start, end - start);
    }
}

std::string toLower(const std::string& s) {
    std::string t(s);

    for (uint i = 0; i < t.length(); i++) {
        if (t[i] >= 'A' && t[i] <= 'Z') {
            t[i] ^= 32;
        }
    }
    
    return t;
}

std::string toUpper(const std::string& s) {
    std::string t(s);

    for (uint i = 0; i < t.length(); i++) {
        if (t[i] >= 'a' && t[i] <= 'z') {
            t[i] ^= 32;
        }
    }

    return t;
}

std::string toString(int i) {
    std::stringstream s;
    s << i;
    return s.str();
}

bool isHexNumber(const std::string& s) {
    const std::string digits = "0123456789ABCDEFabcdef";
    for (std::string::const_iterator iter = s.begin(); iter != s.end(); iter++) {
        if (digits.find(*iter) == std::string::npos) {
            return false;
        }
    }
    return true;
}

u32 hexToInt(const std::string& s) {
    const std::string digits = "0123456789ABCDEF";
    std::string t = toUpper(s);

    u32 value = 0;
    for (uint i = 0; i < t.length(); i++) {
        uint digit = digits.find(t[i]);
        if (digit != std::string::npos) {
            value = (value << 4) | digit;
        } else {
            break;
        }
    }
    return value;
}


std::vector<std::string> split(const std::string& delimiter, const std::string& s) {
    std::vector<std::string> result;
    uint startPos = 0;

    while (startPos < s.length()) {
        uint endPos = s.find(delimiter, startPos);

        if (endPos != std::string::npos) {
            std::string blah = s.substr(startPos, endPos - startPos);
            result.push_back(s.substr(startPos, endPos - startPos));
            startPos = endPos + delimiter.length();
        } else {
            result.push_back(s.substr(startPos));
            break;
        }
    }

    return result;
}

std::string join(const std::string& delimiter, const std::vector<std::string>& s) {
    std::stringstream result;
    if (!s.empty()) {
        result << s[0];
        for (uint i = 1; i < s.size(); i++) {
            result << delimiter << s[i];
        }
    }
    return result.str();
}

namespace Path {
    //---------------------------------------
    // TODO: put this in its own file? --andy

    std::string getDirectory(const std::string& s) {
        uint p = s.find_last_of(delimiters);
        if (p == std::string::npos) {
            return "";
        }

        return s.substr(0, p + 1);
    }

    std::string getFileName(const std::string& s) {
        uint p = s.find_last_of(Path::delimiters);

        if (p == std::string::npos) {
            return s;
        } else {
            return s.substr(p + 1);
        }
    }

    std::string getExtension(const std::string& s) {
        uint pos = s.rfind('.');
        if (pos == std::string::npos) {
            return "";
        } else {
            return s.substr(pos + 1);
        }
    }

    std::string replaceExtension(const std::string& s, const std::string& extension) {
        uint pos = s.rfind('.');
        if (pos == std::string::npos) {
            return s + "." + extension;
        } else {
            return s.substr(0, pos + 1) + extension;
        }
    }

    bool equals(const std::string& s, const std::string& t) {
    #ifdef WIN32
        return toUpper(s) == toUpper(t);
    #else
        return s == t;
    #endif
    }

}