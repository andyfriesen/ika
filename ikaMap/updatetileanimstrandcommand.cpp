
#include "updatetileanimstrandcommand.h"
#include "mainwindow.h"
#include "tileset.h"

UpdateTileAnimStrandCommand::UpdateTileAnimStrandCommand(uint index, const VSP::AnimState& newStrand)
    : _newStrand(newStrand)
    , _index(index)
{}

void UpdateTileAnimStrandCommand::Do(MainWindow* m)
{
    VSP::AnimState& strand = m->GetTileSet()->GetAnim()[_index];
    _oldStrand = strand;
    strand = _newStrand;
}

void UpdateTileAnimStrandCommand::Undo(MainWindow* m)
{
    m->GetTileSet()->GetAnim()[_index] = _oldStrand;
}
