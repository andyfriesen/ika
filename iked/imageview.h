/*
    Generic Image editor class for iked.  Everything that is an image should be editable here.
*/

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "types.h"
#include "wx\wx.h"
#include "docview.h"

class CMainWnd;
class CGraphFrame;
class CImage;
class CPixelMatrix;

class CImageView : public IDocView
{
protected:
    CMainWnd*       pParent;
    CGraphFrame*    pGraph;     // the main editing grid

    CPixelMatrix*   pData;      // the image we're editing (real pixel data)
    CImage*         pImage;     // hardware dependant copy of the image

public:

    CImageView(CMainWnd* parent,CPixelMatrix* img);
    virtual ~CImageView();

    // events
    void OnPaint();

    void OnSave(wxCommandEvent&);

    DECLARE_EVENT_TABLE()
};

#endif