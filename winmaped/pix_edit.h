/*
    I am making love to my dialog class.  It has removed all necessity for singleton classes in winmaped.

    Generic graphics editor.  Subclass this for VSP/CHR and font editors
*/

#ifndef PIX_EDIT_H
#define PIX_EDIT_H

#include <windows.h>
#include <list>
#include "winmaped.h"
#include "dialog.h"
#include "graph.h"
#include "Canvas.h"

class CEdit : public CDialog
{
protected:
    HINSTANCE hInstance;
    HWND hWnd;
    
    Canvas curimage;
    std::list<Canvas> UndoList;
    std::list<Canvas> RedoList;
    int         nMaxUndos;
    bool        bAltered;

//    HBRUSH      hBrush;
    
    CDIB*       iCurrent;               // the image of the graphic we're editing
    CDIB*       iSwatch;                // The colour field
    CDIB*       iBackbuffer;            // canvas for the blown up tile
    
    virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
    virtual bool InitProc(HWND hwnd);
    
    bool PointIsInRect(int x,int y,Rect r);
    
    void DoLeftDownOnBigTile(int x,int y,int b,RGBA& nCurcolour);
    void DoLeftUpOnBigTile(int x,int y,int b);
    
    void DoLeftDownOnSwatch(int x,int y,int b,RGBA& nCurcolour);
    void DoLeftUpOnSwatch(int x,int y,int b);
    
    void HandleMouse(int x,int y,int b);
    
    void ClipUndoList();
    void AddToUndo();
    void AddToRedo();
    void PurgeRedo();
    void PurgeUndo();
    void Undo();
    void Redo();
    
    void DrawSwatch();
    void DrawBlownUpImage();
    void Redraw();
    
    RGBA CalcColour(int nLumina,RGBA chroma);
    void UpdateLumina(HWND hBar);
    void UpdateAlpha(HWND hBar);
    void MakeSwatchImage();
    
    RGBA    chroma;             // hue of the current color
    int     nLumina;            // light
    int     nAlpha;             // translucency
    RGBA    nCurcolour[2];      // Current colour index
    bool    lbutton,rbutton;    // Mouse button state flags
    int     nOldx,nOldy;        // old mouse coords, to prevent redundant tile-mangling.
    
    //---------------------------------- Rects --------------------------------------
    Rect rBigimage;
    Rect rSmallimage;
    Rect rSwatch;
    //---------------------- Things that make derivation nifty ----------------------
    // TODO: make these pure virtuals?
    virtual void UpdateData() {}                                        // Copies the current data to wherever it should go
    virtual void GoNext() {}                                            // next tile/frame/whatever
    virtual void GoPrev() {}                                            // Previous tile/frame/whatever
    virtual void Resize(int newwidth,int newheight) {}                  // called when the image is resized. (everything ika uses is resizeable anyway)
    virtual int  HandleCommand(HWND hWnd,int wParam) { return 0; }	// handles any menu commands that the base class is unaware of
    
    void StartEdit(HINSTANCE hInst,HWND hWndParent,Canvas& src);
    
public:
    
};

#endif