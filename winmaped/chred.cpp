
#include "resource.h"
#include "chred.h"
#include "importchrdlg.h"

void CCHReditor::UpdateData()
{
    //	pChar->GetFrame(nCurframe)=curimage;
    pChar->UpdateFrame(curimage,nCurframe);	
}

void CCHReditor::GoPrev()
{
    if (!nCurframe)
        return;
    
    if (bAltered)
    {
        int nResult=MessageBox(hWnd,"This frame has been altered.\nDo you wish to keep the changes?","CHRed",MB_YESNO);
        if (nResult==IDYES)
            UpdateData();
    }
    
    nCurframe--;
    curimage=pChar->GetFrame(nCurframe);
    PurgeUndo();
    PurgeRedo();
    *iCurrent=curimage;
    bAltered=false;
}

bool CCHReditor::InitProc(HWND hwnd)
{
    hWnd=hwnd;
    HMENU hMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_CHREDMENU));
    SetMenu(hWnd,hMenu);
    DrawMenuBar(hWnd);
    return true;
}

void CCHReditor::GoNext()
{
    if (nCurframe==pChar->NumFrames()-1)
        return;
    
    if (bAltered)
    {
        int nResult=MessageBox(0,"This frame has been altered.\nDo you wish to keep the changes?","CHRed",MB_YESNO);
        if (nResult==IDYES)
            UpdateData();
    }
    
    nCurframe++;
    curimage=pChar->GetFrame(nCurframe);
    PurgeUndo();
    PurgeRedo();
    *iCurrent=curimage;
    bAltered=false;
}

void CCHReditor::Execute(HINSTANCE hInst,HWND hWnd,CCHRfile* chr)
{
    pChar=chr;
    nCurframe=0;
    StartEdit(hInst,hWnd,chr->GetFrame(0));
}

int  CCHReditor::HandleCommand(HWND hWnd,int wParam)
{
    switch (LOWORD(wParam))
    {
    case ID_EDIT_PROPERTIES:
        {
            CCHRPropDlg dlg;
            dlg.Execute(hInstance,hWnd,pChar);
        }
        return 0;
        
    case ID_FILE_IMPORT:
        {
            CImportCHRDlg dlg;
            dlg.Execute(hInstance,hWnd,pChar);
            curimage=pChar->GetFrame(nCurframe);
            PurgeUndo();
            PurgeRedo();
            *iCurrent=curimage;
            Redraw();
            return 0;
        }
    case ID_FILE_OPENCHR:
        Open();
        return 0;
        
    case ID_FILE_SAVECHR:
        if (sLastfilename.length())
        {
            // save file
            Save();
            return 0;
        }
        // Note that there's no break here.  If the file is untitled, then we let it fall through, and treat the 'save' command as a 'save as'*/
    case ID_FILE_SAVEAS:
        UpdateData();
        SaveAs();
        return 0;
        
    case ID_FILE_EXIT:
        PostMessage(hWnd,WM_CLOSE,0,0);
        return 0;
    }
    return 0;
}

void CCHReditor::Open()
{
    char chrfilter[] = "CHR files\0*.chr\0\0";
    char sFilename[256];
    
    OPENFILENAME ofn;
    
    ZeroMemory(&ofn,sizeof ofn);
    ofn.hwndOwner=hWnd;
    ofn.lStructSize=sizeof ofn;
    ofn.hInstance=hInstance;
    ofn.lpstrFilter=chrfilter;
    ofn.lpstrFileTitle=sFilename;
    ofn.nMaxFileTitle=255; // length of the filename array
    ofn.lpstrTitle="Load CHR";
    ofn.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
    
    if (!GetOpenFileName(&ofn))
        return;
    
    pChar->Load(sFilename);
    
    sLastfilename=sFilename;
    nCurframe=0;
    curimage=pChar->GetFrame(nCurframe);
    PurgeUndo();
    PurgeRedo();
    *iCurrent=curimage;
    bAltered=false;
}

void CCHReditor::Save()
{
    pChar->Save(sLastfilename.c_str());
    bAltered=false;
}

void CCHReditor::SaveAs()
{
    char chrfilter[] = "CHR files\0*.chr\0\0";
    char sFilename[256];
    OPENFILENAME ofn;
    
    ZeroMemory(&ofn,sizeof ofn);
    ofn.hwndOwner=hWnd;
    ofn.lStructSize=sizeof ofn;
    ofn.hInstance=hInstance;
    ofn.lpstrFilter=chrfilter;
    ofn.lpstrFileTitle=sFilename;
    ofn.nMaxFileTitle=255;
    ofn.lpstrTitle="Save CHR As...";
    ofn.Flags=OFN_OVERWRITEPROMPT;
    
    if (!GetSaveFileName(&ofn)) return;
    
    pChar->Save(sFilename);
    sLastfilename=sFilename;
    bAltered=false;
}

// ---------------------------------------------------  CHR properties dialogue window  ----------------------------------------

void CCHRPropDlg::UpdateDlg(HWND hWnd)
{
    SetDlgItemInt(hWnd,IDC_EDITHOTX,pChar->HotX(),false);	
    SetDlgItemInt(hWnd,IDC_EDITHOTY,pChar->HotY(),false);
    SetDlgItemInt(hWnd,IDC_EDITHOTWIDTH,pChar->HotW(),false);
    SetDlgItemInt(hWnd,IDC_EDITHOTHEIGHT,pChar->HotH(),false);
}

void CCHRPropDlg::UpdateData(HWND hWnd)
{
    pChar->HotX()=GetDlgItemInt(hWnd,IDC_EDITHOTX,NULL,false);
    pChar->HotY()=GetDlgItemInt(hWnd,IDC_EDITHOTY,NULL,false);
    pChar->HotW()=GetDlgItemInt(hWnd,IDC_EDITHOTWIDTH,NULL,false);
    pChar->HotH()=GetDlgItemInt(hWnd,IDC_EDITHOTHEIGHT,NULL,false);
}

bool CCHRPropDlg::InitProc(HWND hWnd)
{
    UpdateDlg(hWnd);
    SetWindowText(hWnd,"CHREd");
    return true;
}

int  CCHRPropDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            UpdateData(hWnd);
        case IDCANCEL:
            EndDialog(hWnd,0);
            return 0;
        }
    }
    return 0;
}

void CCHRPropDlg::Execute(HINSTANCE hInst,HWND hWnd,CCHRfile* chr)
{
    pChar=chr;
    StartDlg(hInst,hWnd,"CHRpropdlg");
}