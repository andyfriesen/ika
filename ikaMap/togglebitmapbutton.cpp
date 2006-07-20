
#include "common/types.h"
#include "togglebitmapbutton.h"
#include <wx/object.h>
#include <windows.h>

#ifndef wxDEFAULT_BUTTON_MARGIN
const uint wxDEFAULT_BUTTON_MARGIN = 4;
#endif

IMPLEMENT_DYNAMIC_CLASS(ToggleBitmapButton, wxBitmapButton)

BEGIN_EVENT_TABLE(ToggleBitmapButton, wxBitmapButton)
    #ifndef WIN32_IS_GAY
        EVT_PAINT(ToggleBitmapButton::OnPaint)
    #endif
END_EVENT_TABLE()

ToggleBitmapButton::ToggleBitmapButton(
    wxWindow* parent, 
    wxWindowID id, 
    const wxBitmap& bitmap, 
    const wxPoint& pos, 
    const wxSize& size, 
    long style, 
    const wxValidator& validator, 
    const wxString& name
)
    : wxBitmapButton(parent, id, bitmap, pos, size, style, validator, name)
    , pressed(false)
    , unpressedBrush(wxColor(182, 189, 210), wxSOLID)
    , pressedBrush(wxColor(133, 146, 181), wxSOLID)
    , focusedBrush(wxColor(110, 120, 149), wxSOLID)
{
    if (size == wxDefaultSize) {
        SetSize(GetBestSize());
    }

#ifdef WIN32
    HWND hWnd = (HWND)GetHWND();
    style = GetWindowLong(hWnd, GWL_STYLE);
    SetWindowLong(hWnd, GWL_STYLE, style | BS_PUSHLIKE | BS_AUTOCHECKBOX);
#endif
}


wxSize ToggleBitmapButton::GetBestSize() const {
    const wxBitmap& bmp = GetBitmapLabel();
    if (bmp.Ok()) {
        return wxSize(
            bmp.GetWidth() + wxDEFAULT_BUTTON_MARGIN * 2,
            bmp.GetHeight() + wxDEFAULT_BUTTON_MARGIN * 2
        );
    } else {
        return wxSize(10, 10);
    }
}

#ifdef WIN32_IS_GAY

bool ToggleBitmapButton::IsPressed() const {
    int m = SendMessage((HWND)GetHWND(), BM_GETCHECK, 0, 0);
    return m == BST_CHECKED;
}

void ToggleBitmapButton::SetPressed(bool p) {
    pressed = p;
    SendMessage((HWND)GetHWND(), BM_SETCHECK, p ? BST_CHECKED : BST_UNCHECKED, 0);
    Update();
    Refresh();
}

bool ToggleBitmapButton::MSWOnDraw(WXDRAWITEMSTRUCT* item) {
    LPDRAWITEMSTRUCT dis = reinterpret_cast<LPDRAWITEMSTRUCT>(item);
    HDC hDC = dis->hDC;
    uint state = dis->itemState;
    bool selected = (state & ODS_SELECTED) != 0;
    bool focused = (state & ODS_FOCUS) != 0;
    bool disabled = (state & ODS_DISABLED) != 0;

    if (dis->itemAction & ODA_SELECT) {
        dis->itemAction |= 0;
    }

    wxDC dc;
    dc.SetHDC((WXHDC)hDC, false);

    Draw(dc, selected, focused, disabled);
    return true;
}

#else

bool ToggleBitmapButton::IsPressed() const {
    return pressed;
}

void ToggleBitmapButton::SetPressed(bool p) {
    pressed = p;
    Update();
    Refresh();
}

void ToggleBitmapButton::OnPaint(wxPaintEvent& evt) {
    wxPaintDC dc(this);

    Draw(dc, false, this == FindFocus(), IsEnabled());
}

#endif

void ToggleBitmapButton::Draw(wxDC& dc, bool selected, bool focused, bool disabled) {
    wxSize size = GetSize();

    if (focused || selected || pressed) {
        dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE), 1, wxSOLID));
        dc.SetBrush(wxBrush("black", wxTRANSPARENT));
        dc.DrawRectangle(0, 0, size.x, size.y);
    }

    dc.SetPen(wxPen(wxColor(10, 36, 106), 1, wxSOLID));

    if (selected) {
        dc.SetBrush(pressedBrush);
    } else if (focused) {
        dc.SetBrush(focusedBrush);
    } else {
        dc.SetBrush(unpressedBrush);
    }

    if (pressed || selected) {
        dc.DrawRectangle(1, 1, size.x - 3, size.y - 3);
        dc.DrawBitmap(GetBitmapLabel(), 3, 3, true);

    } else {
        dc.DrawRectangle(0, 0, size.x - 3, size.y - 3);
        
        wxColour c = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
        c.Set((c.Red() + 10) / 2, (c.Green() + 36) / 2, (c.Blue() + 106) / 2);
        dc.SetPen(wxPen(c, 1, wxSOLID));
        dc.SetBrush(wxBrush(c, wxSOLID));
        dc.DrawLine(size.x - 3, 1, size.x - 3, size.y - 2);
        dc.DrawLine(1, size.y - 3, size.x - 3, size.y - 3);
        
        dc.DrawBitmap(GetBitmapLabel(), 2, 2, true);
    }
}
