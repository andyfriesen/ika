#if 0
#include <windows.h>
#include <mmsystem.h>

#include "graph.h"
#include "misc.h"
#include "win32.h"
#include "log.h"
#include "configfile.h"


const int TIME_PER_TEST = 1000;  // milliseconds
const int RIGHT_RESULT_MARGIN = 180;

namespace
{
    int testBlitImageEmpty()
    {
        RGBA* pTemp=new RGBA[16*16];
        memset(pTemp, 0, 16*16*sizeof(RGBA));

        handle img=gfxCreateImage(16, 16);
        gfxCopyPixelData(img, (u32*)pTemp, 16, 16);

        int t = timeGetTime();
        int count=0;
        while (timeGetTime() - t < TIME_PER_TEST)
        {
            count++;
            gfxBlitImage(img, 0, 0, true);
        }

        return count;
    }

    int testBlitImageFilled()
    {
        RGBA* pTemp=new RGBA[16*16];
        memset(pTemp, 255, 16*16*sizeof(RGBA));

        handle img=gfxCreateImage(16, 16);
        gfxCopyPixelData(img, (u32*)pTemp, 16, 16);

        int t = timeGetTime();
        int count=0;
        while (timeGetTime() - t < TIME_PER_TEST)
        {
            count++;
            gfxBlitImage(img, 0, 0, true);
        }

        return count;
    }

    int testBlitImageRandom()
    {
        RGBA* pTemp=new RGBA[16*16];
        for (int i=0; i<16*16; i++)
            pTemp[i]=RGBA(Random(0, 255), Random(0, 255), Random(0, 255), Random(0, 255));

        handle img=gfxCreateImage(16, 16);
        gfxCopyPixelData(img, (u32*)pTemp, 16, 16);

        int t = timeGetTime();
        int count=0;
        while (timeGetTime() - t < TIME_PER_TEST)
        {
            count++;
            gfxBlitImage(img, 0, 0, true);
        }

        return count;
    }

//    int testBlitImageMask();
//    int testTransformBlit();
//    int testDirectBlit();
//    int testDirectTransformBlit();
    int testPoints()
    {
        handle scr=gfxGetScreenImage();
        RGBA c(255, 255, 255, 128);

        int t = timeGetTime();
        int count=0;
        while (timeGetTime() - t < TIME_PER_TEST)
        {
            count++;
            gfxSetPixel(scr, 0, 0, (u32)c);
        }

        return count;
    }
    int testLines()
    {
        handle scr=gfxGetScreenImage();
        RGBA c(255, 255, 255, 128);
        int x1=0    , y1=0;
        int x2=319  , y2=239;

        int t = timeGetTime();
        int count=0;
        while (timeGetTime() - t < TIME_PER_TEST)
        {
            count++;
            gfxLine(scr, x1, y1, x2, y2, (u32)c);
        }

        return count;
    }
//    int testGradientLines();
//    int testTriangles();
    int testGradientTriangles()
    {
        int x[] = { 0, 32, 32 };
        int y[] = { 16, 0, 32 };
        int c[] =
        {
            (u32)RGBA(255, 0, 0, 128),
            (u32)RGBA(0, 255, 0, 192),
            (u32)RGBA(0, 0, 255, 255)
        };

        handle scr=gfxGetScreenImage();

        int t = timeGetTime();
        int count=0;
        while (timeGetTime() - t < TIME_PER_TEST)
        {
            count++;
            gfxFlatPoly(scr, x, y, c);
        }

        return count;
    }

    int testRectangles()
    {
        RGBA c(128, 128, 128, 128);
        handle scr=gfxGetScreenImage();

        int t = timeGetTime();
        int count=0;
        while (timeGetTime() - t < TIME_PER_TEST)
        {
            count++;
            gfxRect(scr, 0, 0, 32, 32, (u32)c, true);
        }

        return count;
    }
//    int testGradientRectangles();
    int testShowPage()
    {
        int t = timeGetTime();
        int count=0;
        while (timeGetTime() - t < TIME_PER_TEST)
        {
            count++;
            gfxShowPage();
        }

        return count;
    }
    
    
    static struct {
        
        // constants
        const char* name;
        int (*function)();
        
        // mutables
        int result;
        
    } tests[] = {
        { "BlitImage (empty)",   testBlitImageEmpty      },
        { "BlitImage (filled)",  testBlitImageFilled     },
        { "BlitImage (random)",  testBlitImageRandom     },
//        { "BlitImageMask",       testBlitImageMask       },
//        { "TransformBlit",       testTransformBlit       },
//        { "DirectBlit",          testDirectBlit          },
//        { "DirectTransformBlit", testDirectTransformBlit },
        { "Points",              testPoints              },
        { "Lines",               testLines               },
//        { "Gradient Lines",      testGradientLines       },
//        { "Triangles",           testTriangles           },
        { "Gradient Triangles",  testGradientTriangles   },
        { "Rectangles",          testRectangles          },
//        { "Gradient Rectangles", testGradientRectangles  },
        { "ShowPage",          testShowPage          },
    };
    const int nTests = sizeof(tests) / sizeof(*tests);

};

////////////////////////////////////////////////////////////////////////////////

void Benchmark(HWND hwnd)
{
    CConfigFile cfg("user.cfg");

    SetUpGraphics(cfg["graphdriver"].c_str());

    gfxInit(hwnd, 320, 240, 32, false);

    int i;
    for (i=0; i<nTests; i++)
    {
        tests[i].result=tests[i].function();
    }

    Log::Init("benchmark.log");
    for (i=0; i<nTests; i++)
        Log::Write("%20s\t%ims", tests[i].name, tests[i].result);

    exit(-1);
}
#endif
