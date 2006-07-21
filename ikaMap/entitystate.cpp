#include <limits>

#include "entitystate.h"
#include "executor.h"
#include "mapview.h"
#include "spriteset.h"
#include "common/chr.h"

#include "entitydlg.h"
#include "command.h"

EntityState::EntityState(Executor* e)
    : EditState(e, "Entities")
    , _entLayerIndex(0)
    , _entIndex((std::numeric_limits<unsigned int>::max)())
{}

void EntityState::OnMouseDown(wxMouseEvent& event)
{
    int x = event.GetX();
    int y = event.GetY();
    GetMapView()->ScreenToLayer(x, y);

    // Map::Layer* lay = GetCurLayer();  // Unused.
    uint entIndex = GetMapView()->EntityAt(x, y, GetCurLayerIndex());

    if (entIndex != -1)
    {
        _entLayerIndex = GetCurLayerIndex();
        _entIndex = entIndex;

        GetMapView()->Refresh();
        if (event.ButtonDClick())
        {

            EntityDlg dlg(GetExecutor(), _entLayerIndex, _entIndex);
            int result = dlg.ShowModal();

            if (result == wxID_OK)
            {
                HandleCommand(new ChangeEntityPropertiesCommand(_entLayerIndex, _entIndex, dlg.newData));
            }
        }
    }
    else if (event.LeftDown() && event.ShiftDown())
    {
        if (wxMessageBox("Create a new entity here?", "", wxYES_NO, GetParentWindow()) == wxYES)
        {
            HandleCommand(new CreateEntityCommand(GetCurLayerIndex(), x, y));
        }
    } // create a new entity etc
    else    // deselect
    {
        entIndex = (std::numeric_limits<unsigned int>::max)();
        GetMapView()->Refresh();
    }
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

void EntityState::OnKeyPress(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DELETE && _entIndex != -1)
    {
        int result = ::wxMessageBox("Are you sure you want to destroy this entity?", "KILL", wxYES_NO | wxCENTER, GetParentWindow());
        if (result == wxYES)
        {
            HandleCommand(new DestroyEntityCommand(_entLayerIndex, _entIndex));
            _entIndex = (std::numeric_limits<unsigned int>::max)();
        }
    }
}

void EntityState::OnRenderCurrentLayer()
{
    Map::Layer* layer = GetCurLayer();
    uint xwin = GetMapView()->GetXWin();
    uint ywin = GetMapView()->GetYWin();

    if (!layer)
        return;

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

        ss = GetExecutor()->GetSpriteSet(iter->spriteName);

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
        GetMapView()->GetVideo()->DrawRect(
            x,
            y,
            width,
            height,
            RGBA(255, 255, 255));

        // Draw a white fady rect over the currently selected one
        if (&*iter == &layer->entities[_entIndex])
            GetMapView()->GetVideo()->DrawRectFill(
                x,
                y,
                width,
                height,
                RGBA(255, 255, 255, 128));
    }
}

void EntityState::OnSwitchLayers(unsigned int oldLayer, unsigned int newLayer)
{
	_entIndex = (std::numeric_limits<unsigned int>::max)();
}