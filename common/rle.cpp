#include "types.h"

void ReadCompressedLayer1(u8 *dest,int numbytes,u8 *src)
{
    int j,n=0;
    u8 run;
    u8 w;
    u8 *buf;
    
    buf=src; n=0;
    do
    {
        w=*buf; buf++;
        if (w==0xFF)
        {
            run=*buf; buf++;
            w=*buf; buf++;
            for (j=0; j<run; j++)
                dest[n+j]=w;
            n+=run;
        }
        else
        {
            dest[n]=w;
            n++;
        }
    } while (n<numbytes);
}

void ReadCompressedLayer2(u16 *dest,int numwords,u16 *src)
{
    int j,n;
    u8 run;
    u16 w;
    u16 *buf;
    
    buf=src; n=0;
    do
    {
        w=*buf; buf++;
        if ((w & 0xFF00)==0xFF00)
        {
            run=(w & 0x00FF);
            w=*buf; buf++;
            for(j=0; j<run; j++)
                dest[n+j]=w;
            n+=run;
        }
        else
        {
            dest[n]=w;
            n++;
        }
    } while(n<numwords);
}

void ReadCompressedLayer2tou32(u32* dest,int numu32s,void* src)
{
    u16* pTemp=new u16[numu32s];
    
    ReadCompressedLayer2(pTemp,numu32s,(u16*)src);
    
    for (int i=0; i<numu32s; i++)
    {
        dest[i]=pTemp[i];
    }
    
    delete[] pTemp;
}

void WriteCompressedLayer1(u8 *dest,int numbytes,int& bufsize,u8 *src)
{
    int i;
    u8 byt,samect;
    
    i=0; bufsize=0;
    do
    {   
        byt=src[i++];
        samect=1;
        while (samect<254 && i<numbytes && byt==src[i])
        {
            samect++;
            i++;
        }
        if (samect==2 && byt != 0xFF) { *dest++=byt; bufsize++;}
        if (samect==3 && byt != 0xFF) { *dest++=byt; *dest++=byt; bufsize+=2;}
        if (samect>3 || byt == 0xFF)
        {
            *dest++=0xFF;
            *dest++=(char)samect;
            bufsize+=2;
        }
        *dest++=byt;
        bufsize++;
    } while (i<numbytes);
}

void WriteCompressedLayer2(u16 *dest,int numwords,int& bufsize,u16 *src)
{
    int i;
    u16 byt,samect;
    
    i=0; bufsize=0;
    do
    {   
        byt=src[i++];
        samect=1;
        while (samect<255 && i<numwords && byt==src[i])
        {
            samect++;
            i++;
        }
        if (samect>1 || ((byt&0xFF00)==0xFF00))  // oopsies! - tSB
        {
            *dest++=(0xFF00)+samect;
            bufsize+=2;
        }
        *dest++=byt;
        bufsize+=2;
    } while (i<numwords);
}


void WriteCompressedLayer2fromu32(void* dest,int numu32s,int& bufsize,u32* src)
{
    u16* pTemp=new u16[numu32s];
    
    for (int i=0; i<numu32s; i++)
        pTemp[i]=src[i];
    
    WriteCompressedLayer2((u16*)dest,numu32s,bufsize,pTemp);
    delete[] pTemp;
}