/*
 * Miscellaneous types used throughout ika.
 */

#pragma once

#include "utility.h"

/// Everybody has their own Rect struct.  This is mine.
struct Rect {
	int left;	
	int right; 	
	int top;  	
	int bottom;    
    
	inline int Width() const  { return right - left; }    	
	inline int Height() const { return bottom - top; }    	
    void Normalize() {
        if (left > right)
            std::swap(left, right);
        if (top > bottom)
            std::swap(top, bottom);
    }

    Rect(int x1, int y1, int x2, int y2) : left(x1), right(x2), top(y1), bottom(y2) {}
    Rect() : left(0), right(0), top(0), bottom(0) {}
    Rect(const Rect& r)
        : left(r.left)
        , right(r.right)
        , top(r.top)
        , bottom(r.bottom)
    {}
};

/// Simple 2D point
struct Point {
    int x;
    int y;

    Point(int _x = 0, int _y = 0)
        : x(_x)
        , y(_y)
    {}
};

/// Directions an entity can be facing
enum Direction {
    face_up,
    face_down,
    face_left,
    face_right,
    face_upleft,
    face_upright,
    face_downleft,
    face_downright,
    face_nothing, 										///< a special case, entity::getcommand returns this if the entity in question isn't walking
};

#pragma pack (push, 1)

/// A color.
union RGBA {
    // Nonstandard, but supported by both GCC and MSVC.
#pragma warning (push)
#pragma warning (disable : 4201)
    struct {
        u8 r, g, b, a;
    };
#pragma warning (pop)

    u32 i;

    RGBA()
        : i(0)
    {}

    RGBA(u8 index, u8* palette) {
        int i = index * 3;
        r = palette[i++] << 2;
        g = palette[i++] << 2;
        b = palette[i]   << 2;
        a = index ? 255 : 0;
    }

    RGBA(u32 bleagh) 
        : i(bleagh)
    {}

    RGBA(u8 _r, u8 _g, u8 _b, u8 _a = 255)
        : r(_r), g(_g), b(_b), a(_a)
    {}

    operator u32() const { return i; }
};

#pragma pack (pop)
