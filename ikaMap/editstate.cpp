#include "editstate.h"
#include "executor.h"
#include "mapview.h"
#include "tilesetview.h"
#include "map.h"
#include "tileset.h"
#include "command.h"

EditState::EditState(Executor* e)
    : _executor(e)
{
}

Executor*    EditState::GetExecutor()       const { return _executor; }
MapView*     EditState::GetMapView()        const { return _executor->GetMapView(); }
TileSetView* EditState::GetTileSetView()    const { return _executor->GetTileSetView(); }
Map*         EditState::GetMap()            const { return _executor->GetMap(); }

Map::Layer*  EditState::GetCurLayer() const
{ 
    if (_executor->GetMap()->NumLayers() == 0)
        return 0;
    else
        return _executor->GetMap()->GetLayer(_executor->GetCurrentLayer());
}

uint      EditState::GetCurLayerIndex()     const { return _executor->GetCurrentLayer(); }
TileSet*  EditState::GetTileSet()           const { return _executor->GetTileSet(); }
wxWindow* EditState::GetParentWindow()      const { return _executor->GetParentWindow(); }

void EditState::HandleCommand(::Command* cmd)
{
    _executor->HandleCommand(cmd);
}