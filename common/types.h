/*
    Commonly used types, enums, etc...
*/

#ifndef TYPES_H
#define TYPES_H

#pragma warning (disable:4786)                      // identifier too long (stupid compiler)

#include <vector>
#include <string>

using std::string;
using std::vector;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef unsigned int uint;

#ifdef MSVC6
#  define for if (0); else for
#endif

//! Everybody has their own Rect struct.  This is mine.
struct Rect
{
    int left;
    int top;
    int right;
    int bottom;
    
    inline int Width() const  { return right-left; }
    inline int Height() const { return bottom-top; }

    Rect(int x1,int y1,int x2,int y2) : left(x1),right(x2),top(y1),bottom(y2) {}
    Rect() : left(0),right(0),top(0),bottom(0) {}
};

struct Point
{
    int x;
    int y;

    Point(int _x = 0, int _y = 0)
        : x(_x)
        , y(_y)
    {}
};

enum Direction											// directions an entity can be facing
{
    face_up,
    face_down,
    face_left,
    face_right,
    face_upleft,
    face_upright,
    face_downleft,
    face_downright,
    face_nothing,										// a special case, entity::getcommand returns this if the entity in question isn't walking
};

enum MoveCode											// entity movecodes
{
    mc_nothing=0,										// entity just stands there
    mc_wander,											// entity wanders around like an idiot
    mc_wanderrect,										// ditto, but the entity is restricted to a rectangular region
    mc_wanderzone,										// ditto again, but the entity is restricted to a particular zone
    mc_script,											// entity follows its script
    mc_chase,											// the entity is following another entity
};

#pragma pack (push,1)

//! A color. ;)
union RGBA
{
    struct {
        u8 r, g, b, a;
    };

    uint i;

    RGBA()
        : i(0)
    {}

    RGBA(u8 index, u8* palette)
    {
        int i = index * 3;
        r = palette[i++] << 2;
        g = palette[i++] << 2;
        b = palette[i]   << 2;
        a = index ? 255 : 0;
    }

    RGBA(uint bleagh) 
        : i(bleagh)
    {}

    RGBA(u8 _r, u8 _g, u8 _b, u8 _a = 255)
        : r(_r), g(_g), b(_b), a(_a)
    {}

    operator u32() { return i; }
};

//! Also a colour.
struct BGRA
{
    u8 b,g,r,a;
    
    BGRA()
        : b(0),g(0),r(0),a(0)	{}
    BGRA(u8 ab,u8 ag,u8 ar,u8 aa)
        : b(ab),g(ag),r(ar),a(aa)	{}
    BGRA(u32 c)
    {
        a=c>>24;
        b=(c>>16)&255;
        g=(c>>8)&255;
        r=c&255;
    }

    BGRA(const RGBA& c)
    {	r=c.r; g=c.g; b=c.b; a=c.a;	}

    inline operator u32() const
    {
        return *(u32*)this;
    }
};

// win32 ;P
#undef RGB

//! A colour, but without any alpha information.
struct RGB
{
    u8 r,g,b;
    
    RGB(const RGBA& c)
        : r(c.r),g(c.g),b(c.b)
    {}

    RGB(u8 R,u8 G,u8 B,u8 A=0)
        : r(R),g(G),b(B)
    {}
};

//inline RGBA::RGBA(const BGRA& c) {	r=c.r; g=c.g; b=c.b; a=c.a;	}
#pragma pack (pop)

// kudos to kepler and aegis for this trick
// fixes gay for scoping in VC6
#ifdef MSVC
//#   define for if (0); else for
#endif

#endif
