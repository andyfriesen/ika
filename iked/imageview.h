/*
    Generic Image editor class for iked.  Everything that is an image should be editable here.
*/

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "types.h"
#include <wx\wx.h>
#include "docview.h"

class CMainWnd;
class CGraphFrame;
class CImage;

class CImageView : IDocView
{
    CGraphFrame*    pGraph;     // the main editing grid

    CImage*         pImage;     // the image we're editing

public:

    CImageView(CMainWnd* parent,CImage* img);

    // events
    void OnPaint();

    DECLARE_EVENT_TABLE()
};

#endif