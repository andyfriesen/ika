#ifndef Py_ERRORS_H
#define Py_ERRORS_H
#ifdef __cplusplus
extern "C" {
#endif


/* Error handling definitions */

DL_IMPORT(void) PyErr_SetNone(PyObject *);
DL_IMPORT(void) PyErr_SetObject(PyObject *, PyObject *);
DL_IMPORT(void) PyErr_SetString(PyObject *, const char *);
DL_IMPORT(PyObject *) PyErr_Occurred(void);
DL_IMPORT(void) PyErr_Clear(void);
DL_IMPORT(void) PyErr_Fetch(PyObject **, PyObject **, PyObject **);
DL_IMPORT(void) PyErr_Restore(PyObject *, PyObject *, PyObject *);

/* Error testing and normalization */
DL_IMPORT(int) PyErr_GivenExceptionMatches(PyObject *, PyObject *);
DL_IMPORT(int) PyErr_ExceptionMatches(PyObject *);
DL_IMPORT(void) PyErr_NormalizeException(PyObject**, PyObject**, PyObject**);


/* Predefined exceptions */

extern DL_IMPORT(PyObject *) PyExc_Exception;
extern DL_IMPORT(PyObject *) PyExc_StopIteration;
extern DL_IMPORT(PyObject *) PyExc_StandardError;
extern DL_IMPORT(PyObject *) PyExc_ArithmeticError;
extern DL_IMPORT(PyObject *) PyExc_LookupError;

extern DL_IMPORT(PyObject *) PyExc_AssertionError;
extern DL_IMPORT(PyObject *) PyExc_AttributeError;
extern DL_IMPORT(PyObject *) PyExc_EOFError;
extern DL_IMPORT(PyObject *) PyExc_FloatingPointError;
extern DL_IMPORT(PyObject *) PyExc_EnvironmentError;
extern DL_IMPORT(PyObject *) PyExc_IOError;
extern DL_IMPORT(PyObject *) PyExc_OSError;
extern DL_IMPORT(PyObject *) PyExc_ImportError;
extern DL_IMPORT(PyObject *) PyExc_IndexError;
extern DL_IMPORT(PyObject *) PyExc_KeyError;
extern DL_IMPORT(PyObject *) PyExc_KeyboardInterrupt;
extern DL_IMPORT(PyObject *) PyExc_MemoryError;
extern DL_IMPORT(PyObject *) PyExc_NameError;
extern DL_IMPORT(PyObject *) PyExc_OverflowError;
extern DL_IMPORT(PyObject *) PyExc_RuntimeError;
extern DL_IMPORT(PyObject *) PyExc_NotImplementedError;
extern DL_IMPORT(PyObject *) PyExc_SyntaxError;
extern DL_IMPORT(PyObject *) PyExc_IndentationError;
extern DL_IMPORT(PyObject *) PyExc_TabError;
extern DL_IMPORT(PyObject *) PyExc_ReferenceError;
extern DL_IMPORT(PyObject *) PyExc_SystemError;
extern DL_IMPORT(PyObject *) PyExc_SystemExit;
extern DL_IMPORT(PyObject *) PyExc_TypeError;
extern DL_IMPORT(PyObject *) PyExc_UnboundLocalError;
extern DL_IMPORT(PyObject *) PyExc_UnicodeError;
extern DL_IMPORT(PyObject *) PyExc_ValueError;
extern DL_IMPORT(PyObject *) PyExc_ZeroDivisionError;
#ifdef MS_WINDOWS
extern DL_IMPORT(PyObject *) PyExc_WindowsError;
#endif

extern DL_IMPORT(PyObject *) PyExc_MemoryErrorInst;

/* Predefined warning categories */
extern DL_IMPORT(PyObject *) PyExc_Warning;
extern DL_IMPORT(PyObject *) PyExc_UserWarning;
extern DL_IMPORT(PyObject *) PyExc_DeprecationWarning;
extern DL_IMPORT(PyObject *) PyExc_SyntaxWarning;
extern DL_IMPORT(PyObject *) PyExc_OverflowWarning;
extern DL_IMPORT(PyObject *) PyExc_RuntimeWarning;


/* Convenience functions */

extern DL_IMPORT(int) PyErr_BadArgument(void);
extern DL_IMPORT(PyObject *) PyErr_NoMemory(void);
extern DL_IMPORT(PyObject *) PyErr_SetFromErrno(PyObject *);
extern DL_IMPORT(PyObject *) PyErr_SetFromErrnoWithFilename(PyObject *, char *);
extern DL_IMPORT(PyObject *) PyErr_Format(PyObject *, const char *, ...);
#ifdef MS_WINDOWS
extern DL_IMPORT(PyObject *) PyErr_SetFromWindowsErrWithFilename(int, const char *);
extern DL_IMPORT(PyObject *) PyErr_SetFromWindowsErr(int);
#endif

/* Export the old function so that the existing API remains available: */
extern DL_IMPORT(void) PyErr_BadInternalCall(void);
extern DL_IMPORT(void) _PyErr_BadInternalCall(char *filename, int lineno);
/* Mask the old API with a call to the new API for code compiled under
   Python 2.0: */
#define PyErr_BadInternalCall() _PyErr_BadInternalCall(__FILE__, __LINE__)

/* Function to create a new exception */
DL_IMPORT(PyObject *) PyErr_NewException(char *name, PyObject *base,
                                         PyObject *dict);
extern DL_IMPORT(void) PyErr_WriteUnraisable(PyObject *);

/* Issue a warning or exception */
extern DL_IMPORT(int) PyErr_Warn(PyObject *, char *);
extern DL_IMPORT(int) PyErr_WarnExplicit(PyObject *, char *,
					 char *, int, char *, PyObject *);

/* In sigcheck.c or signalmodule.c */
extern DL_IMPORT(int) PyErr_CheckSignals(void);
extern DL_IMPORT(void) PyErr_SetInterrupt(void);

/* Support for adding program text to SyntaxErrors */
extern DL_IMPORT(void) PyErr_SyntaxLocation(char *, int);
extern DL_IMPORT(PyObject *) PyErr_ProgramText(char *, int);
	
/* These APIs aren't really part of the error implementation, but
   often needed to format error messages; the native C lib APIs are
   not available on all platforms, which is why we provide emulations
   for those platforms in Python/mysnprintf.c */
#if defined(MS_WIN32) && !defined(HAVE_SNPRINTF)
# define HAVE_SNPRINTF
# define snprintf _snprintf
# define vsnprintf _vsnprintf
#endif

/* Always enable the fallback solution during the 2.2.0 alpha cycle
   for enhanced testing */
#if PY_VERSION_HEX < 0x020200B0
# undef HAVE_SNPRINTF
#endif

#ifndef HAVE_SNPRINTF
#include <stdarg.h>
extern DL_IMPORT(int) PyOS_snprintf(char *str, size_t size, const char  *format, ...);
extern DL_IMPORT(int) PyOS_vsnprintf(char *str, size_t size, const char  *format, va_list va);
#else
# define PyOS_vsnprintf	vsnprintf
# define PyOS_snprintf	snprintf
#endif

#ifdef __cplusplus
}
#endif
#endif /* !Py_ERRORS_H */
