
/* String object interface */

#ifndef Py_STRINGOBJECT_H
#define Py_STRINGOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

/*
Type PyStringObject represents a character string.  An extra zero byte is
reserved at the end to ensure it is zero-terminated, but a size is
present so strings with null bytes in them can be represented.  This
is an immutable object type.

There are functions to create new string objects, to test
an object for string-ness, and to get the
string value.  The latter function returns a null pointer
if the object is not of the proper type.
There is a variant that takes an explicit size as well as a
variant that assumes a zero-terminated string.  Note that none of the
functions should be applied to nil objects.
*/

/* Two speedup hacks.  Caching the hash saves recalculation of a
   string's hash value.  Interning strings (which requires hash
   caching) tries to ensure that only one string object with a given
   value exists, so equality tests are one pointer comparison.
   Together, these can speed the interpreter up by as much as 20%.
   Each costs the size of a long or pointer per string object.  In
   addition, interned strings live until the end of times.  If you are
   concerned about memory footprint, simply comment the #define out
   here (and rebuild everything!). */
#define CACHE_HASH
#ifdef CACHE_HASH
#define INTERN_STRINGS
#endif

typedef struct {
    PyObject_VAR_HEAD
#ifdef CACHE_HASH
    long ob_shash;
#endif
#ifdef INTERN_STRINGS
    PyObject *ob_sinterned;
#endif
    char ob_sval[1];
} PyStringObject;

extern DL_IMPORT(PyTypeObject) PyString_Type;

#define PyString_Check(op) PyObject_TypeCheck(op, &PyString_Type)
#define PyString_CheckExact(op) ((op)->ob_type == &PyString_Type)

extern DL_IMPORT(PyObject *) PyString_FromStringAndSize(const char *, int);
extern DL_IMPORT(PyObject *) PyString_FromString(const char *);
extern DL_IMPORT(PyObject *) PyString_FromFormatV(const char*, va_list);
extern DL_IMPORT(PyObject *) PyString_FromFormat(const char*, ...);
extern DL_IMPORT(int) PyString_Size(PyObject *);
extern DL_IMPORT(char *) PyString_AsString(PyObject *);
extern DL_IMPORT(void) PyString_Concat(PyObject **, PyObject *);
extern DL_IMPORT(void) PyString_ConcatAndDel(PyObject **, PyObject *);
extern DL_IMPORT(int) _PyString_Resize(PyObject **, int);
extern DL_IMPORT(int) _PyString_Eq(PyObject *, PyObject*);
extern DL_IMPORT(PyObject *) PyString_Format(PyObject *, PyObject *);
extern DL_IMPORT(PyObject *) _PyString_FormatLong(PyObject*, int, int,
						  int, char**, int*);

#ifdef INTERN_STRINGS
extern DL_IMPORT(void) PyString_InternInPlace(PyObject **);
extern DL_IMPORT(PyObject *) PyString_InternFromString(const char *);
extern DL_IMPORT(void) _Py_ReleaseInternedStrings(void);
#else
#define PyString_InternInPlace(p)
#define PyString_InternFromString(cp) PyString_FromString(cp)
#define _Py_ReleaseInternedStrings()
#endif

/* Macro, trading safety for speed */
#define PyString_AS_STRING(op) (((PyStringObject *)(op))->ob_sval)
#define PyString_GET_SIZE(op)  (((PyStringObject *)(op))->ob_size)

/* _PyString_Join(sep, x) is like sep.join(x).  sep must be PyStringObject*,
   x must be an iterable object. */
extern DL_IMPORT(PyObject *) _PyString_Join(PyObject *sep, PyObject *x);

/* --- Generic Codecs ----------------------------------------------------- */

/* Create an object by decoding the encoded string s of the
   given size. */

extern DL_IMPORT(PyObject*) PyString_Decode(
    const char *s,              /* encoded string */
    int size,                   /* size of buffer */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Encodes a char buffer of the given size and returns a 
   Python object. */

extern DL_IMPORT(PyObject*) PyString_Encode(
    const char *s,              /* string char buffer */
    int size,                   /* number of chars to encode */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Encodes a string object and returns the result as Python 
   object. */

extern DL_IMPORT(PyObject*) PyString_AsEncodedObject(
    PyObject *str,	 	/* string object */
    const char *encoding,	/* encoding */
    const char *errors		/* error handling */
    );

/* Encodes a string object and returns the result as Python string
   object.   
   
   If the codec returns an Unicode object, the object is converted
   back to a string using the default encoding.

   DEPRECATED - use PyString_AsEncodedObject() instead. */

extern DL_IMPORT(PyObject*) PyString_AsEncodedString(
    PyObject *str,	 	/* string object */
    const char *encoding,	/* encoding */
    const char *errors		/* error handling */
    );

/* Decodes a string object and returns the result as Python 
   object. */

extern DL_IMPORT(PyObject*) PyString_AsDecodedObject(
    PyObject *str,	 	/* string object */
    const char *encoding,	/* encoding */
    const char *errors		/* error handling */
    );

/* Decodes a string object and returns the result as Python string
   object.  
   
   If the codec returns an Unicode object, the object is converted
   back to a string using the default encoding.

   DEPRECATED - use PyString_AsDecodedObject() instead. */

extern DL_IMPORT(PyObject*) PyString_AsDecodedString(
    PyObject *str,	 	/* string object */
    const char *encoding,	/* encoding */
    const char *errors		/* error handling */
    );

/* Provides access to the internal data buffer and size of a string
   object or the default encoded version of an Unicode object. Passing
   NULL as *len parameter will force the string buffer to be
   0-terminated (passing a string with embedded NULL characters will
   cause an exception).  */

extern DL_IMPORT(int) PyString_AsStringAndSize(
    register PyObject *obj,	/* string or Unicode object */
    register char **s,		/* pointer to buffer variable */
    register int *len		/* pointer to length variable or NULL
				   (only possible for 0-terminated
				   strings) */
    );
    

#ifdef __cplusplus
}
#endif
#endif /* !Py_STRINGOBJECT_H */
