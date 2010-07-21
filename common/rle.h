/* 
   Some Run-Length Encoding Crap (tm)
   Originally coded by vecna (c) 1999, I think.
   Severely cut, pasted, butchered, and almost rewritten by the Speed Bump
   (c) 2000

   MapEd uses this stuff for compressing / uncompressing maps and VSPs.
*/

#pragma once

#include "utility.h"

extern void ReadCompressedLayer1(u8 *dest, int numbytes, u8 *src);
extern void ReadCompressedLayer2(u16 *dest, int numwords, u16 *src);
extern void ReadCompressedLayer2tou32(u32* dest, int numu32s, void* src);
extern void WriteCompressedLayer1(u8 *dest, int numbytes, int &bufsize, u8 *src);
extern void WriteCompressedLayer2(u16 *dest, int numwords, int &bufsize, u16 *src);
extern void WriteCompressedLayer2fromu32(void* dest, int numu32s, int& bufsize, u32* src);

