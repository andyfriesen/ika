/*
    Generic Image editor class for iked.  Everything that is an image should be editable here.
*/

#if 0

#pragma once

#include "common/utility.h"
#include "wx/wx.h"
#include "docview.h"

class MainWindow;
class GraphicsFrame;
class Image;
class Canvas;

class ImageView : DocumentPanel {
    ImageView(MainWindow* parent, Canvas* img);
    virtual ~ImageView();

    // events
    void OnPaint();

    virtual void OnSave(wxCommandEvent&);
    virtual const void* GetResource() const;

protected:
    MainWindow*     pParent;
    GraphicsFrame*  pGraph;     // the main editing grid

    Canvas*         pData;      // the image we're editing (real pixel data)
    Image*          pImage;     // hardware dependant copy of the image

    DECLARE_EVENT_TABLE()
};

#endif
