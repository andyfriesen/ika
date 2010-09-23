//#define DEBUG

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string>
#include <sstream>

#ifdef DEBUG

std::string int2string(const int& number);

/* just a helper for code location */
#define PyDEBUGSTR (std::string("print('debug: ") + __FILE__ + ":" + int2string(__LINE__) + "')").c_str()
#define LOC std::cout << "debug:" << __FILE__ << ":" << __LINE__ << " ";
#define PyLOC PyRun_SimpleString(PyDEBUGSTR);

/* macro for general debug print statements. */
#define DEBUG_PRINT(text) LOC std::cout << text << endl;

/* macro that prints a variable name and its actual value */
#define DEBUG_VAR(text) LOC std::cout << (#text) << "=" << text << endl;

/* python macro that prints a variable name and its actual value */
#define DEBUG_PyRUN(text) PyLOC PyRun_SimpleString(text);

/* python macro that prints an error */
#define DEBUG_PyERR(type, text) PyLOC PyErr_SetString(type, text);

#else

/* when debug isn't defined all the macro calls do absolutely nothing for C++ debugging code and the same without helper for python debugging code */
#define DEBUG_PRINT(text)
#define DEBUG_VAR(text)
#define DEBUG_PyRUN(text)
#define DEBUG_PyERR(type, text) PyErr_SetString(type, text);

#endif /* DEBUG */
#endif /* DEBUG_H */
