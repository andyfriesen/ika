/* types.h

  Types, enums, etc...
*/

// blah!

#ifndef TYPES_H
#define TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

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

struct RGBA
{
    u8 r,g,b,a;
    
    RGBA()
        : r(0),g(0),b(0),a(0)		{}
    
    RGBA(u8 ar,u8 ag,u8 ab,u8 aa)
        : r(ar),g(ag),b(ab),a(aa) 	{}
    
    // convert from 16bpp
    RGBA(u16 c)
    {
        b=(c&31)<<3;
        g=(c>>3)&0xFC;
        r=(c>>8)&0xF8;
        a=c?255:0;
    }
    
    // conversion from 8bpp with palette
    RGBA(u8 c,u8* pPal)
    {
        r=pPal[c*3  ]<<2;
        g=pPal[c*3+1]<<2;
        b=pPal[c*3+2]<<2;
        a=c?255:0;
    }
    
    inline operator u32() const
    {
        return (a<<24) | (r<<16) | (g<<8) | b;
    }
    //	RGBA(const BGRA& c);
};

struct BGRA
{
    u8 b,g,r,a;
    
    BGRA()
        : b(0),g(0),r(0),a(0)	{}
    BGRA(u8 ab,u8 ag,u8 ar,u8 aa)
        : b(ab),g(ag),r(ar),a(aa)	{}
    BGRA(const RGBA& c)
    {	r=c.r; g=c.g; b=c.b; a=c.a;	}
};

#undef RGB

struct RGB
{
    u8 r,g,b;
    
    RGB(const RGBA& c)
        : r(c.r),g(c.g),b(c.b)
    {}
};

//inline RGBA::RGBA(const BGRA& c) {	r=c.r; g=c.g; b=c.b; a=c.a;	}
#pragma pack (pop)

#endif
