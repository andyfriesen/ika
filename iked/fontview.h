
#pragma once

#include "wx/wx.h"
#include "wx\colordlg.h"

#include "common/utility.h"
#include "docview.h"
#include "graph.h"
#include "common/fontfile.h"

#if 0
class FontView : public DocumentPanel {
    enum
    {
        id_font = 300,
        id_filesave,
        id_filesaveas,
        id_fileclose,
        id_optionscolor
    };

    public:

        FontView(MainWindow* parentwnd, const std::string& fname);
        ~FontView();

        void Paint();

        void OnChangeBackgroundColor(wxCommandEvent&);
        virtual void OnSave(wxCommandEvent&);
        void OnSaveAs(wxCommandEvent&);
        virtual const void* GetResource() const;
        void OnSize(wxSizeEvent&);
        void OnEraseBackground(wxEraseEvent&) {}
        void OnScroll(wxScrollWinEvent&);
        void OnClose();
        void OnLeftClick(wxMouseEvent&);
        void OnRightClick(wxMouseEvent&);
        void Render();

    private:

        MainWindow* pParent;
        GraphicsFrame* pGraph;
        FontFile* pFontfile;

        int ywin;
        int nCurfont;
        std::string sFilename;

        void FontPos(int fontidx, int& x, int& y) const;
        int FontAt(int x, int y) const;
        void UpdateScrollbar();


        DECLARE_EVENT_TABLE()

};
#endif
