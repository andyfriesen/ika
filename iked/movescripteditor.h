#if 0

#pragma once

#include "wx/wx.h"
#include "common/utility.h"

class CCHRfile;
class SpriteSet;
class SpriteSetView;

using std::vector;

class wxGridEvent;

class MoveScriptEditor : public wxDialog {
    public:

        MoveScriptEditor(SpriteSetView* parent, SpriteSet* sprite, int idx = 0);
        
        void UpdateData();
        void UpdateDlg();
    
    private:

        void OnSize(wxCommandEvent& event);
        void OnClose(wxCommandEvent& event);
        void OnOk(wxCommandEvent&);

        void BeginEdit(wxGridEvent& event);
        void EditCell(wxGridEvent& event);
        void EditAnimScript(wxGridEvent& event);
        void EditMetaData(wxGridEvent& event);

        void InitControls();

        SpriteSetView*     pParent;
        SpriteSet*         pSprite;
        int                 nCurframe;

        class wxGrid*       animScriptGrid;
        class wxGrid*       metaDataGrid;

        // HACK because wx change events don't tell me what the old value of a cell is.
        std::string         oldValue;

        // controls

        /*vector<wxTextCtrl*>     movescript;
        wxTextCtrl*             pDesc;
        wxTextCtrl*             pHotx;
        wxTextCtrl*             pHoty;
        wxTextCtrl*             pHotw;
        wxTextCtrl*             pHoth;
        // idle frames
        wxTextCtrl*             pLeft;
        wxTextCtrl*             pRight;
        wxTextCtrl*             pUp;
        wxTextCtrl*             pDown;*/


        DECLARE_EVENT_TABLE()
};
#endif
