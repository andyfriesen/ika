/*
    Generic Image editor class for iked.  Everything that is an image should be editable here.
*/

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "common/utility.h"
#include "wx\wx.h"
#include "docview.h"

class CMainWnd;
class CGraphFrame;
class CImage;
struct Canvas;

class CImageView : public IDocView
{
protected:
    CMainWnd*       pParent;
    CGraphFrame*    pGraph;     // the main editing grid

    Canvas*         pData;      // the image we're editing (real pixel data)
    CImage*         pImage;     // hardware dependant copy of the image

public:

    CImageView(CMainWnd* parent, Canvas* img);
    virtual ~CImageView();

    // events
    void OnPaint();

    virtual void OnSave(wxCommandEvent&);
    virtual const void* GetResource() const;

    DECLARE_EVENT_TABLE()
};

#endif