
#ifndef MOVESCRIPTEDITOR_H
#define MOVESCRIPTEDITOR_H

#include "wx\wx.h"
#include "types.h"

class CCHRfile;
class CSpriteSet;
class CSpriteSetView;


using std::vector;

class CMovescriptEditor : public wxDialog
{
    public:

        CMovescriptEditor(CSpriteSetView* parent, CSpriteSet* sprite, int idx = 0);

        
        void UpdateData();
        void UpdateDlg();
    
        void OnClose(wxCommandEvent& event);


    private:

        void InitControls();

        CSpriteSetView*     pParent;
        CSpriteSet*         pSprite;
        int                 nCurframe;


        // controls

        vector<wxTextCtrl*>     movescript;
        wxTextCtrl*             pDesc;
        wxTextCtrl*             pHotx;
        wxTextCtrl*             pHoty;
        wxTextCtrl*             pHotw;
        wxTextCtrl*             pHoth;
        // idle frames
        wxTextCtrl*             pLeft;
        wxTextCtrl*             pRight;
        wxTextCtrl*             pUp;
        wxTextCtrl*             pDown;


        DECLARE_EVENT_TABLE()
};

#endif
        
        
        


