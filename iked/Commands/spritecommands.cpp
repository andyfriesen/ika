
#include "commands/spritecommands.h"

namespace iked { namespace commands {

    DeleteSpriteFrameCommand::DeleteSpriteFrameCommand(SpriteSet* spriteSet, int frameIndex)
        : spriteSet(spriteSet)
        , frameIndex(frameIndex)
    {}

    DeleteSpriteFrameCommand::~DeleteSpriteFrameCommand() {
    }

    void DeleteSpriteFrameCommand::Do(CommandContext* ctx) {
    }

    void DeleteSpriteFrameCommand::Undo(CommandContext* ctx) {
    }

}};
