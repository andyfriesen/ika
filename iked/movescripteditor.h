
#ifndef MOVESCRIPTEDITOR_H
#define MOVESCRIPTEDITOR_H

#include "wx\wx.h"
#include "types.h"

class CCHRfile;
class CSpriteSet;
class CSpriteSetView;

using std::vector;

class wxGridEvent;

class CMovescriptEditor : public wxDialog
{
    public:

        CMovescriptEditor(CSpriteSetView* parent, CSpriteSet* sprite, int idx = 0);
        
        void UpdateData();
        void UpdateDlg();
    
    private:

        void OnSize(wxCommandEvent& event);
        void OnClose(wxCommandEvent& event);

        void BeginEdit(wxGridEvent& event);
        void EditCell(wxGridEvent& event);
        void EditAnimScript(wxGridEvent& event);
        void EditMetaData(wxGridEvent& event);

        void InitControls();

        CSpriteSetView*     pParent;
        CSpriteSet*         pSprite;
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
        
        
        


