// I'm not entirely happy with this interface, but meh.
// It's conveniant for now.
//  --khross

#include "wx/event.h"

#include "spritesetview.h"
#include "imageview.h"
#include "main.h"
#include "movescripteditor.h"
#include "importframesdlg.h"

namespace
{
    enum
    {
        id_filler = 100,

        id_filesave,
        id_filesaveas,
        id_fileclose,

        id_chrmovescript,
        id_chrimportframes,

        id_deleteframe,
        id_insertframe,
        id_copyframe,
        id_pasteinto,
        id_pasteover,
        id_insertandpaste,
        id_editframe,

        id_zoomin,
        id_zoomout,
        id_zoomnormal,

        // I was planning on a button or something
        // for these.
        id_prevframe,
        id_nextframe
    };

    class SpriteFrame : public CGraphFrame
    {
        DECLARE_EVENT_TABLE()
        
        CSpriteSetView* pSpritesetview;
    public:
        SpriteFrame(CSpriteSetView* parent)
            : CGraphFrame(parent)
            , pSpritesetview(parent)
        {}

        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC blah(this);

            pSpritesetview->Render();
        }
    };

    BEGIN_EVENT_TABLE(SpriteFrame, CGraphFrame)
        EVT_PAINT(SpriteFrame::OnPaint)
    END_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(CSpriteSetView, IDocView)

    EVT_MENU(id_chrmovescript, CSpriteSetView::OnShowMovescriptEditor)
    EVT_MENU(id_chrimportframes, CSpriteSetView::OnImportFrames)
    EVT_MENU(id_editframe, CSpriteSetView::OnEditFrame)

    EVT_MENU(id_zoomin, CSpriteSetView::OnZoomIn)
    EVT_MENU(id_zoomout, CSpriteSetView::OnZoomOut)
    EVT_MENU(id_zoomnormal, CSpriteSetView::OnZoomNormal)

    EVT_MENU(id_filesave, CSpriteSetView::OnSave)
    EVT_MENU(id_filesaveas, CSpriteSetView::OnSaveAs)
    EVT_MENU(id_fileclose, CSpriteSetView::OnClose)
    
    EVT_SCROLLWIN(CSpriteSetView::OnScroll)
    EVT_SIZE(CSpriteSetView::OnSize)
    
    EVT_PAINT(CSpriteSetView::OnPaint)

    EVT_LEFT_DOWN(CSpriteSetView::OnLeftClick)
    EVT_RIGHT_DOWN(CSpriteSetView::OnRightClick)

END_EVENT_TABLE()

CSpriteSetView::CSpriteSetView(CMainWnd* parentwnd, const std::string& fname)
    : IDocView(parentwnd, fname)
    , _parent(parentwnd)
    , _curFrame(0)
    , _ywin(0)
    , _zoom(16)
    , _sprite(0)
{
    _sprite = parentwnd->spriteset.Load(fname);
    if (!_sprite)
        throw std::runtime_error(va("Unable to load sprite %s", fname.c_str()));

    Init();
}

CSpriteSetView::CSpriteSetView(CMainWnd* parentwnd, int width, int height)
    : IDocView(parentwnd, "")
    , _parent(parentwnd)
    , _curFrame(0)
    , _ywin(0)
    , _zoom(16)
    , _sprite(0)
{
    _sprite = new CSpriteSet;
    _sprite->New(width, height);
    Init();
}

CSpriteSetView::~CSpriteSetView()
{
    _parent->spriteset.Release(_sprite);
    delete _contextMenu;
}

void CSpriteSetView::OnSave(wxCommandEvent& event)
{
    _sprite->Save(name.c_str());
}

void CSpriteSetView::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Save CHR",
        "",
        "",
        "ika-sprite files (*.ika-sprite)|*.ika-sprite|"
        "CHR files (*.chr)|*.chr|"
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    int result = dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    std::string oldname(name);

    name = dlg.GetPath().c_str();
    SetTitle(name.c_str());

    OnSave(event);

    if (!Path::equals(name, oldname))
    {
        _sprite->Load(oldname.c_str());
        if (!_parent->spriteset.Release(_sprite))
            delete _sprite;
        _sprite = _parent->spriteset.Load(name.c_str());
    }
}

const void* CSpriteSetView::GetResource() const
{
    return _sprite;
}

void CSpriteSetView::OnPaint()
{
    if (!_graph || !_sprite)
        return;

    wxPaintDC dc(this);

    Render();
}

void CSpriteSetView::Render()
{
    if (!_sprite)
        return;

    int nWidth = _graph->LogicalWidth();
    int nHeight = _graph->LogicalHeight();

    int framex = _sprite->Width();
    int framey = _sprite->Height();

    int xstep = framex + (_pad ? 1 : 0);
    int ystep = framey + (_pad ? 1 : 0);

    int cols = nWidth / xstep;
    if (cols < 1) cols = 1;

    int rows = (nHeight / ystep) + 1;
    int nSprite = _ywin * cols;

    _graph->SetCurrent();
    _graph->Clear();

    for(int y = 0; y < rows; y++)
    {
        for(int x = 0; x < cols; x++)
        {
            Canvas& rBitmap = _sprite->Get(nSprite);
            CImage img(rBitmap);

            _graph->RectFill(x * xstep, y * ystep, framex, framey, RGBA(128, 128, 128));
            _graph->Blit(img, x * xstep, y * ystep, true);
            
            nSprite++;

            if (nSprite >= _sprite->Count()) 
                goto breakloop;
        }
    }
breakloop:

    int x2, y2;
    framex = _sprite->Width();
    framey = _sprite->Height();
    SpritePos(_curFrame, x2, y2);

    _graph->Rect(x2 - 1, y2 - 1, framex + 1, framey + 1, RGBA(255, 255, 255));

    _graph->ShowPage();
}

void CSpriteSetView::OnSize(wxSizeEvent& event)
{
    _graph->SetSize(GetClientSize());

    UpdateScrollbar();
}

void CSpriteSetView::OnScroll(wxScrollWinEvent& event)
{
    if (event.m_eventType == wxEVT_SCROLLWIN_TOP)           _ywin = 0;
    else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM)   _ywin = _sprite->Count();
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEUP)   _ywin--;
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEDOWN) _ywin++;
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEUP)   _ywin -= GetScrollThumb(wxVERTICAL);
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEDOWN) _ywin += GetScrollThumb(wxVERTICAL);
    else                                                    _ywin =  event.GetPosition();

    UpdateScrollbar();
    Render();
}

void CSpriteSetView::OnLeftClick(wxMouseEvent& event)
{    
    int t = FrameAt(event.GetPosition().x, event.GetPosition().y);
   
    if (t != -1)
    {
        _curFrame = t;
        Render();
    }
}

void CSpriteSetView::OnRightClick(wxMouseEvent& event)
{
    int t = FrameAt(event.GetPosition().x, event.GetPosition().y);
    if (t != -1)
    {
        _curFrame = t;
        Render();
        PopupMenu(_contextMenu, event.GetPosition());
    }
}

void CSpriteSetView::OnEditFrame(wxCommandEvent& event)
{
//    _parent->OpenDocument(new CImageView(_parent, &_sprite->Get(_curFrame)));
}

void CSpriteSetView::OnPreviousFrame(wxCommandEvent& event)
{
    if (_curFrame > 0)
        _curFrame--;
    else
        _curFrame = _sprite->Count();

    Render();
}

void CSpriteSetView::OnNextFrame(wxCommandEvent& event)
{
    if (_curFrame < _sprite->Count())
        _curFrame++;
    else
        _curFrame = 0;

    Render();
}

void CSpriteSetView::OnZoomIn(wxCommandEvent& event)    { Zoom(1);  }
void CSpriteSetView::OnZoomOut(wxCommandEvent& event)   { Zoom(-1); }
void CSpriteSetView::OnZoomNormal(wxCommandEvent& event){ Zoom(16 - _zoom); } 
void CSpriteSetView::UpdateScrollbar()
{
    const int w = _graph->LogicalWidth();
    const int h = _graph->LogicalHeight();

    int cols = w / _sprite->Width();
    int rows = h / _sprite->Height();

    if (cols < 1) cols = 1;

    int nTotalheight = _sprite->Count() / cols + 1;

    if (_ywin > nTotalheight - rows)    _ywin = nTotalheight - rows;
    if (_ywin < 0)                      _ywin = 0;

    SetScrollbar(wxVERTICAL, _ywin, rows, nTotalheight, true);
}

void CSpriteSetView::OnShowMovescriptEditor(wxCommandEvent& event)
{
    _moveScriptEditor->Show(true);
    _moveScriptEditor->UpdateDlg();
}

void CSpriteSetView::OnImportFrames(wxCommandEvent& event)
{
    ImportFramesDlg dlg(this);

    if (dlg.ShowModal() != wxID_OK)
        return;

    std::vector<Canvas>& frames = dlg.frames;
    if (frames.size() == 0)
        return;

    // this is horribly inefficient
    if (!dlg.append)
        while (_sprite->Count())
            _sprite->DeleteFrame(0);

    if (frames[0].Width() != _sprite->Width() || frames[0].Height() != _sprite->Height())
    {
        if (!dlg.append)
            _sprite->Resize(frames[0].Width(), frames[0].Height());
        else
        {
            // TODO: offer to crop or scale or some other things.
            wxMessageBox("The frames aren't the same size as the sprite!", "Error", wxOK | wxCENTER, this);
            return;
        }
    }

    for (uint i = 0; i < frames.size(); i++)
        _sprite->AppendFrame(frames[i]);
}

void CSpriteSetView::Init()
{
    InitMenu();
    InitAccelerators();

    _graph = new SpriteFrame(this);
    _graph->SetSize(GetClientSize());

    _moveScriptEditor = new CMovescriptEditor(this, _sprite);

    SetFocus();
}

void CSpriteSetView::InitMenu()
{
    wxMenuBar* menubar = _parent->CreateBasicMenu();

    wxMenu* filemenu = menubar->Remove(0);
    filemenu->InsertSeparator(2);
    filemenu->Insert(3, new wxMenuItem(filemenu, id_filesave, "&Save", "Save the sprite to disk."));
    filemenu->Insert(4, new wxMenuItem(filemenu, id_filesaveas, "Save &As", "Save the sprite under a new filename."));
    filemenu->Insert(5, new wxMenuItem(filemenu, id_fileclose, "&Close", "Close the sprite view."));
    menubar->Append(filemenu, "&File");

    wxMenu* chrmenu = new wxMenu;
    chrmenu->Append(id_chrmovescript, "&Movescript...");
    chrmenu->Append(id_chrimportframes, "Import &Frames...");
    menubar->Append(chrmenu, "&CHR");

    SetMenuBar(menubar);

    // Context menu
    _contextMenu = new wxMenu();
    _contextMenu->Append(id_deleteframe, "Delete");
    _contextMenu->Append(id_insertframe, "Insert");
    _contextMenu->Append(id_copyframe, "Copy");
    _contextMenu->Append(id_pasteinto, "Paste into");
    _contextMenu->Append(id_pasteover, "Paste over");
    _contextMenu->Append(id_insertandpaste, "Insert and paste");
    // Until the pixel editor is up and running
    //_contextMenu->AppendSeparator();
    //_contextMenu->Append(id_editframe, "Edit");
}

void CSpriteSetView::InitAccelerators()
{
    std::vector<wxAcceleratorEntry> accel = _parent->CreateBasicAcceleratorTable();

    int p = accel.size();
    accel.resize(accel.size()+4);

    accel[p++].Set(wxACCEL_CTRL, (int)'S', id_filesave);
    accel[p++].Set(0, (int)'+', id_zoomin);
    accel[p++].Set(0, (int)'-', id_zoomout);
    accel[p++].Set(0, (int)'=', id_zoomnormal);

    wxAcceleratorTable table(p, &*accel.begin());
    SetAcceleratorTable(table);
}

void CSpriteSetView::SpritePos(int idx, int& x, int& y) const
{
    int xstep = _sprite->Width()  + (_pad ? 1 : 0);
    int ystep = _sprite->Height() + (_pad ? 1 : 0);

    int cols = max(_graph->LogicalWidth() / xstep, 1);

    x = idx % cols;
    y = idx / cols - _ywin;

    x *= xstep;
    y *= ystep;
}

int CSpriteSetView::FrameAt(int x, int y) const
{
    const int xstep = _sprite->Width()  + (_pad ? 1 : 0);
    const int ystep = _sprite->Height() + (_pad ? 1 : 0);

    const int cols = max(_graph->LogicalWidth() / xstep, 1);

    x /= xstep;      y /= ystep;

    int t = (y + _ywin) * cols + x;

    if (t >= _sprite->Count()) return -1;
    return t;
}

void CSpriteSetView::Zoom(int nZoomscale)
{
    int _zoom = _graph->Zoom()-nZoomscale;

    if (_zoom < 1) _zoom = 1;
    if (_zoom > 255) _zoom = 255;

    _graph->Zoom(_zoom);

    UpdateScrollbar();

    Render();      
}
