
#ifndef TOGGLEBITMAPBUTTON_H
#define TOGGLEBITMAPBUTTON_H

#include "wxinc.h"

#ifdef _WIN32
#   define WIN32_IS_GAY
#endif

struct ToggleBitmapButton : wxBitmapButton {
    DECLARE_DYNAMIC_CLASS(ToggleBitmapButton)

    ToggleBitmapButton() {}

    ToggleBitmapButton(
        wxWindow* parent, 
        wxWindowID id, 
        const wxBitmap& bitmap, 
        const wxPoint& pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize, 
        long style = 0, 
        const wxValidator& validator = wxDefaultValidator, 
        const wxString& name = "button"
    );

    bool IsPressed() const;
    void SetPressed(bool p);

    virtual wxSize GetBestSize() const;

private:

#ifdef WIN32_IS_GAY
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT* item);
#else
    void OnPaint(wxPaintEvent& evt);
#endif
    void Draw(wxDC& dc, bool selected, bool focused, bool disabled);

    bool pressed;

    wxBrush unpressedBrush;
    wxBrush pressedBrush;
    wxBrush focusedBrush;

    DECLARE_EVENT_TABLE();
};

#endif
