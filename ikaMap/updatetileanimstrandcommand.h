
#ifndef UPDATETILEANIMSTRANDCOMMAND_H
#define UPDATETILEANIMSTRANDCOMMAND_H

#include "command.h"
#include "vsp.h"

class UpdateTileAnimStrandCommand : public Command
{
public:
    UpdateTileAnimStrandCommand(uint index, const VSP::AnimState& newStrand);

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);

private:
    VSP::AnimState _newStrand;
    VSP::AnimState _oldStrand;
    uint _index;
};

#endif
