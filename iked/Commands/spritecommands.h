
#pragma once

#include "common/canvas.h"
#include "commands/command.h"

namespace iked { 
    struct SpriteSet;
}

namespace iked { namespace commands {

    struct DeleteSpriteFrameCommand : Command {
        DeleteSpriteFrameCommand(SpriteSet* spriteSet, int frameIndex);
        virtual ~DeleteSpriteFrameCommand();

        virtual void Do(CommandContext* ctx);
        virtual void Undo(CommandContext* ctx);

    private:
        int frameIndex;
        SpriteSet* spriteSet;

        Canvas oldFrame;
    };

}}
