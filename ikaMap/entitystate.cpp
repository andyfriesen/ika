
#include "entitystate.h"
#include "mainwindow.h"
#include "mapview.h"
#include "chr.h"

#include "entitydlg.h"
#include "command.h"

EntityState::EntityState(MainWindow* mw)
    : EditState(mw)
    , _entLayerIndex(0)
    , _entIndex(-1)
{}

void EntityState::OnMouseDown(wxMouseEvent& event)
{
    int x = event.GetX();
    int y = event.GetY();
    GetMapView()->ScreenToMap(x, y);

    Map::Layer* lay = GetCurLayer();
    uint entIndex = GetMapView()->EntityAt(x, y, GetCurLayerIndex());

    if (entIndex != -1)
    {
        _entLayerIndex = GetCurLayerIndex();
        _entIndex = entIndex;

        GetMapView()->Refresh();
        if (event.ButtonDClick())
        {

            EntityDlg dlg(GetMainWindow(), _entLayerIndex, _entIndex);
            int result = dlg.ShowModal();

            if (result == wxID_OK)
            {
                HandleCommand(new ChangeEntityPropertiesCommand(_entLayerIndex, _entIndex, dlg.newData));
            }
        }
    }
    else if (event.LeftDown() && event.ShiftDown())
    {
        if (wxMessageBox("Create a new entity here?", "", wxYES_NO, GetMainWindow()) == wxYES)
        {
            HandleCommand(new CreateEntityCommand(GetCurLayerIndex(), x, y));
        }
    } // create a new entity etc
}

void EntityState::OnMouseUp(wxMouseEvent& event)
{
}

void EntityState::OnMouseMove(wxMouseEvent& event)
{
}

void EntityState::OnMouseWheel(wxMouseEvent& event)
{
}

void EntityState::OnRenderCurrentLayer()
{
    Map::Layer* layer = GetCurLayer();
    uint xwin = GetMapView()->GetXWin();
    uint ywin = GetMapView()->GetYWin();

    if (!layer)
        return;

    // This is more work than I think it should be, largely due to the whole stupid entity "blueprint" bullshit.
    // I should nuke that shit and be done with it.
    for (std::vector<Map::Entity>::iterator iter = layer->entities.begin();
        iter != layer->entities.end();
        iter++)
    {
        SpriteSet* ss = 0;
        // default position/size if the sprite cannot be found
        int hotx = 0;
        int hoty = 0;
        int width = 16;
        int height = 16;

        ss = GetMainWindow()->GetSprite(iter->spriteName);

        if (ss != 0)
        {
            hotx = ss->GetCHR()->HotX();
            hoty = ss->GetCHR()->HotY();
            width = ss->Width();
            height = ss->Height();
        }

        int x = iter->x - hotx - xwin + layer->x;
        int y = iter->y - hoty - ywin + layer->y;

        // outline them all
        GetMapView()->GetVideo()->Rect(
            x,
            y,
            width, 
            height,
            RGBA(255, 255, 255));

        // Draw a white fady rect over the currently selected one
        if (&*iter == &layer->entities[_entIndex])
            GetMapView()->GetVideo()->RectFill(
                x,
                y,
                width, 
                height,
                RGBA(255, 255, 255, 128));
    }
}