
#include "common/utility.h"

void ReadCompressedLayer1(u8 *dest, int numBytes, u8 *src) {
    int j, n = 0;
    u8 run;
    u8 w;
    u8 *buf;
    
    buf = src;
    do {
        w=*buf; buf++;
        if (w == 0xFF) {
            run = *buf++;
            w = *buf++;
            for (j = 0; j < run; j++) {
                dest[n + j]=w;
            }
            n += run;
        } else {
            dest[n]=w;
            n++;
        }
    } while (n < numBytes);
}

void ReadCompressedLayer2(u16 *dest, int numWords, u16 *src) {
    u16* buf = src; 
    int n = 0;
    do {
        u16 w = *buf++;
        if ((w & 0xFF00) == 0xFF00) {
            u8 run = u8(w & 0x00FF);
            w = *buf++;

            for(int j = 0; j < run; j++) {
                dest[n + j]=w;
            }
            n += run;
        } else {
            dest[n]=w;
            n++;
        }
    } while(n < numWords);
}

void ReadCompressedLayer2tou32(u32* dest, int numu32s, void* src) {
    ScopedArray<u16> buffer16(new u16[numu32s]);
    
    ReadCompressedLayer2(buffer16.get(), numu32s, (u16*)src);
    
    std::copy(buffer16.get(), buffer16.get() + numu32s, dest);
}

void WriteCompressedLayer1(u8 *dest, int numBytes, int& bufSize, u8 *src) {
    int i = 0; 
    bufSize = 0;
    do {   
        u8 byt = src[i++];
        u8 sameCount = 1;
        while (sameCount < 254 && i < numBytes && byt == src[i]) {
            sameCount++;
            i++;
        }
        
        if (sameCount == 2 && byt != 0xFF) { 
            *dest++ = byt; bufSize++;
        } else if (sameCount == 3 && byt != 0xFF) { 
            *dest++ = byt; *dest++ = byt; bufSize += 2;
        } else if (sameCount > 3 || byt == 0xFF) {
            *dest++ = 0xFF;
            *dest++ = (u8)sameCount;
            bufSize += 2;
        }
        *dest++ = byt;
        bufSize++;
    } while (i < numBytes);
}

void WriteCompressedLayer2(u16 *dest, int numWords, int& bufSize, u16 *src) {
    int i = 0; 
    bufSize = 0;
    do {
        u16 byt = src[i++];
        u16 samect = 1;
        while (samect < 255 && i < numWords && byt == src[i]) {
            samect++;
            i++;
        }
        if (samect > 1 || ((byt&0xFF00) == 0xFF00)) {
            *dest++ = (0xFF00) | samect;
            bufSize += 2;
        }
        *dest++ = byt;
        bufSize += 2;
    } while (i < numWords);
}


void WriteCompressedLayer2fromu32(void* dest, int numu32s, int& bufsize, u32* src) {
    u16* pTemp = new u16[numu32s];
    
    for (int i = 0; i < numu32s; i++) {
        pTemp[i] = u16(0xFFFF & src[i]);
    }
    
    WriteCompressedLayer2((u16*)dest, numu32s, bufsize, pTemp);
    delete[] pTemp;
}
