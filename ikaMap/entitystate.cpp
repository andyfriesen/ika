
#include "entitystate.h"
#include "mainwindow.h"
#include "mapview.h"
#include "chr.h"

#include "entitydlg.h"
#include "command.h"

EntityState::EntityState(MainWindow* mw)
    : EditState(mw)
    , _curEnt(0)
{}

void EntityState::OnMouseDown(wxMouseEvent& event)
{
    int x = event.GetX();
    int y = event.GetY();
    GetMapView()->ScreenToMap(x, y);

    Map::Layer* lay = GetCurrentLayer();
    Map::Layer::Entity* ent = GetMapView()->EntityAt(x, y, GetMapView()->GetCurLayer());

    if (ent)
    {
        _curEnt = ent;

        GetMapView()->Refresh();
        if (event.ButtonDClick())
        {
            // <GAY>
            uint idx = 0;
            while (_curEnt != &lay->entities[idx])
            {
                idx++;
                wxASSERT(idx < lay->entities.size());
            }
            // </GAY>

            EntityDlg dlg(GetMainWindow(), GetMapView()->GetCurLayer(), idx);
            int result = dlg.ShowModal();

            if (result == wxID_OK)
            {
                HandleCommand(new ChangeEntityPropertiesCommand(GetMapView()->GetCurLayer(), idx, dlg.newData, dlg.newBlueprint));
            }
        }
    }
    else
    {} // create a new entity etc
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

void EntityState::OnRender()
{
    Map::Layer* layer = GetCurrentLayer();
    uint xwin = GetMapView()->GetXWin();
    uint ywin = GetMapView()->GetYWin();

    // This is more work than I think it should be, largely due to the whole stupid entity "blueprint" bullshit.
    // I should nuke that shit and be done with it.
    for (std::vector<Map::Layer::Entity>::iterator iter = layer->entities.begin();
        iter != layer->entities.end();
        iter++)
    {
        SpriteSet* ss = 0;
        // default position/size if the sprite cannot be found
        int hotx = 0;
        int hoty = 0;
        int width = 16;
        int height = 16;

        if (GetMap()->entities.count(iter->bluePrint))
        {
            Map::Entity* bluePrint = &GetMap()->entities[iter->bluePrint];
            ss = GetMainWindow()->GetSprite(bluePrint->spriteName);
        }

        if (ss != 0)
        {
            hotx = ss->GetCHR()->HotX();
            hoty = ss->GetCHR()->HotY();
            width = ss->Width();
            height = ss->Height();
        }

        // outline them all
        GetMapView()->GetVideo()->Rect(
            iter->x - hotx - xwin, 
            iter->y - hoty - ywin, 
            width, 
            height,
            RGBA(255, 255, 255));

        // Draw a white fady rect over the currently selected one
        if (&*iter == _curEnt)
            GetMapView()->GetVideo()->RectFill(
                iter->x - hotx - xwin, 
                iter->y - hoty - ywin, 
                width, 
                height,
                RGBA(255, 255, 255, 128));
    }
}