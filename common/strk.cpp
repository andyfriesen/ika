
/*
    strk.cpp, coded by aen
    last updated: dec 19, 2k
*/

#include <ctype.h>
#include <iostream.h>
#include <string.h>
#include <stdlib.h>

#include "strk.h"

string_k::string_k() {
    //	cout << "string_k::string_k()" << endl;
    
    zap();
}

string_k::string_k(const char* source) {
    //	cout << "string_k::string_k(const char*)" << endl;
    
    // have to load something
    if (!source) {
        source = "<NULL>";
    }
    // clip source length to maximum allowed length
    int SL = strlen(source);
    if (SL > STRK_LENGTH)
        SL = STRK_LENGTH;
    // zap existing contents
    zap();
    // copy in new string
    strncpy(F_data, source, SL);
    F_data[SL] = 0;
}


string_k::string_k(char source) {
    //	cout << "string_k::string_k(char)" << endl;
    
    F_data[0] = source;
    F_data[1] = 0;
}


string_k  operator+ (const string_k& a, const string_k& b) {
    //	cout << "operator+(a, b)" << endl;
    
    string_k result(a);
    return result += b;
}

string_k& operator+=(string_k& lhs, const string_k& rhs) {
    //	cout << "operator+=(lhs, rhs)" << endl;
    
    return lhs.append(rhs);
}

string_k& operator+=(string_k& lhs, char rhs) {
    //	cout << "operator+=(lhs, char)" << endl;
    
    int L = lhs.length();
    if (L < STRK_LENGTH) {
        lhs.F_data[L] = rhs;
        lhs.F_data[L + 1] = 0;
    }
    return lhs;
}

char string_k::operator [](int idx)
{
    if (idx<0 || idx>length())
        return '\0';
    else
        return F_data[idx];
}

string_k string_k::mid(int start, int L) const {
    if (L < 1 || start + L < 0 || start >= length()) {
        return "";
    }
    
    // clip left
    if (start < 0) {
        L += start;
        start = 0;
    }
    // clip right
    if (start + L > length()) {
        L = length() - start;
    }
    
    string_k result;
    // patch out
    memcpy(result.F_data, F_data + start, L);
    result.F_data[L] = 0;
    
    return result;
}

string_k string_k::left(int count) const {
    return mid(0, count);
}

string_k string_k::right(int count) const {
    return mid(length() - count, count);
}

const char* string_k::c_str() const {  // obsolete (see below)
    return F_data;
}

int string_k::length() const {
    return strlen(F_data);
}

int string_k::toint() const {
    return atoi(F_data);
}

string_k& string_k::append(const string_k& source) {
    return insert(source, length());
}

string_k& string_k::insert(const string_k& source, int X) {
/*

  definitions:
  SL  = source length
  F   = first character
  X   = insert point
  R   = shift-to point
  L   = last character
  0   = terminating NULL character
  !   = absolute end of storage (last byte)
  
    synonyms:
    R   == X + SL
    rhs == L - X
    
      VISUALS
      source:
      {--S--}
      
        F_data:
        {----rhs---}
        F---X------R---L0------!
        
          F_data after insertion:
          {----rhs---}
          F---{--S--}X------R---L0
    */
    
    // ignore bogus requests
    if (X < 0 || X >= STRK_LENGTH) {
        return *this;
    }
    // if insert point plus source length exceeds or meets
    // maximum allowed length, just overwrite the tail
    int R = X + source.length();
    if (R >= STRK_LENGTH) {
        strncpy(F_data + X, source.F_data, STRK_LENGTH - X);
        F_data[STRK_LENGTH] = 0;
        // otherwise, push over and patch in
    } else {
        int rhs = length() - X;
        if (R + rhs > STRK_LENGTH)
            rhs = STRK_LENGTH - R;
        // push
        memmove(F_data + R, F_data + X, rhs);
        // patch
        memcpy (F_data + X, source.F_data, source.length());
        F_data[R + rhs] = 0;
    }
    
    return *this;
}

string_k string_k::upper() const {
    string_k ret;
    for (int n = length() - 1; n >= 0; n--) {
        ret.F_data[n] = toupper(F_data[n]);
    }
    ret.F_data[length()] = 0;
    return ret;
}

string_k string_k::lower() const {
    string_k ret;
    for (int n = length() - 1; n >= 0; n--) {
        ret.F_data[n] = tolower(F_data[n]);
    }
    ret.F_data[length()] = 0;
    return ret;
}

void string_k::zap() {
    F_data[0] = 0;
}

int compare(const string_k& a, const string_k& b) {
    return strcmp(a.F_data, b.F_data);
}

int operator ==(const string_k& a, const string_k& b) {
    return 0 == compare(a, b);
}

int operator !=(const string_k& a, const string_k& b) {
    return 0 != compare(a, b);
}

int operator < (const string_k& a, const string_k& b) {
    return 0 >  compare(a, b);
}

int operator >=(const string_k& a, const string_k& b) {
    return 0 <= compare(a, b);
}

int operator > (const string_k& a, const string_k& b) {
    return 0 <  compare(a, b);
}

int operator <=(const string_k& a, const string_k& b) {
    return 0 >= compare(a, b);
}