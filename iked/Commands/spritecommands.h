
#pragma once

#include "common/canvas.h"
#include "commands/command.h"

namespace iked { 
    struct SpriteSet;
}

namespace iked { namespace commands {

    struct UpdateSpriteAnimScriptCommand : Command {
        UpdateSpriteAnimScriptCommand(SpriteSet* spriteSet, const std::string& name, const std::string& script);

        virtual void Do(CommandContext* ctx);
        virtual void Undo(CommandContext* ctx);

    private:
        SpriteSet* spriteSet;

        std::string name;
        std::string script;
        std::string oldScript;
    };



    struct DeleteSpriteFrameCommand : Command {
        DeleteSpriteFrameCommand(SpriteSet* spriteSet, int frameIndex);

        virtual void Do(CommandContext* ctx);
        virtual void Undo(CommandContext* ctx);

    private:
        int frameIndex;
        SpriteSet* spriteSet;

        Canvas oldFrame;
    };


    /**
     * Describes a strategy for importing images into a file. (currently applies to sprites and tilesets)
     */
    enum ImportMode {
        insertFrames,
        replaceFrames,
        eraseAndReplaceFrames,
    };

    /**
     * FIXME: way, WAY too much memory babysitting going on here. --andy
     */
    struct ImportSpriteFramesCommand : Command {
        ImportSpriteFramesCommand(
            SpriteSet* spriteSet, 
            const std::vector<Canvas>& frames, 
            int firstIndex,
            ImportMode importMode
        );

        virtual void Do(CommandContext* ctx);
        virtual void Undo(CommandContext* ctx);
    private:
        SpriteSet* spriteSet;
        std::vector<Canvas> frames;
        std::vector<Canvas> oldFrames;
        int firstIndex;
        ImportMode importMode;
    };

}}
