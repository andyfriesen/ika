
#include "commands/spritecommands.h"
#include "common/chr.h"
#include "spriteset.h"

namespace iked { namespace commands {

    UpdateSpriteAnimScriptCommand::UpdateSpriteAnimScriptCommand(SpriteSet* spriteSet, const std::string& name, const std::string& script)
        : spriteSet(spriteSet)
        , name(name)
        , script(script)
    {
        oldScript = spriteSet->GetCHR().moveScripts[name];
    }

    void UpdateSpriteAnimScriptCommand::Do(CommandContext* ctx) {
        if (script.empty()) {
            spriteSet->GetCHR().moveScripts.erase(name);
        } else {
            spriteSet->GetCHR().moveScripts[name] = script;
        }
        spriteSet->changed.fire(spriteSet);
    }

    void UpdateSpriteAnimScriptCommand::Undo(CommandContext* ctx) {
        if (oldScript.empty()) {
            spriteSet->GetCHR().moveScripts.erase(name);
        } else {
            spriteSet->GetCHR().moveScripts[name] = oldScript;
        }
        spriteSet->changed.fire(spriteSet);
    }

    ///

    DeleteSpriteFrameCommand::DeleteSpriteFrameCommand(SpriteSet* spriteSet, int frameIndex)
        : spriteSet(spriteSet)
        , frameIndex(frameIndex)
    {
        assert(spriteSet != 0);
        assert(0 <= frameIndex && frameIndex < spriteSet->getCount());
        oldFrame = spriteSet->getCanvas(frameIndex);
    }

    void DeleteSpriteFrameCommand::Do(CommandContext* ctx) {
        spriteSet->remove(frameIndex);
        spriteSet->changed.fire(spriteSet);
    }

    void DeleteSpriteFrameCommand::Undo(CommandContext* ctx) {
        spriteSet->insert(oldFrame, frameIndex);
        spriteSet->changed.fire(spriteSet);
    }

    ///

    ImportSpriteFramesCommand::ImportSpriteFramesCommand(
        SpriteSet* spriteSet, 
        const std::vector<Canvas>& frames, 
        int firstIndex, 
        ImportMode importMode
    )   : spriteSet(spriteSet)
        , frames(frames)
        , firstIndex(firstIndex)
        , importMode(importMode)
    {
        assert(spriteSet != 0);
        assert(0 <= firstIndex && firstIndex <= spriteSet->GetCHR().NumFrames());

        const std::vector<Canvas*>& chrFrames = spriteSet->GetCHR().GetAllFrames();
        
        if (importMode == eraseAndReplaceFrames) {
            // have to save every frame
            oldFrames.reserve(chrFrames.size());
            for (uint i = 0; i < chrFrames.size(); i++) {
                oldFrames.push_back(*chrFrames[i]);
            }
        } else if (importMode == replaceFrames) {
            int start = firstIndex;
            int len = min(frames.size() - firstIndex, frames.size());

            oldFrames.reserve(len);
            for (uint i = start; i < start + len; i++) {
                oldFrames.push_back(*chrFrames[i]);
            }
        } else if (importMode == insertFrames) {
            // Nothing to save
        } else {
            // illegal importMode value
            assert(false);
        }
    }

    void ImportSpriteFramesCommand::Do(CommandContext* ctx) {
        if (importMode == insertFrames) {
            for (uint i = 0; i < frames.size(); i++) {
                spriteSet->insert(frames[i], i + firstIndex);
            }
        } else if (importMode == replaceFrames) {
            for (uint i = 0; i < frames.size(); i++) {
                spriteSet->update(frames[i], i + firstIndex);
            }
        } else if (importMode == eraseAndReplaceFrames) {
            while (spriteSet->getCount() > 0) {
                spriteSet->remove(0);
            }
            for (uint i = 0; i < frames.size(); i++) {
                spriteSet->append(frames[i]);
            }
        } else {
            // bad import mode
            assert(false);
        }
        spriteSet->changed.fire(spriteSet);
    }

    void ImportSpriteFramesCommand::Undo(CommandContext* ctx) {
        if (importMode == insertFrames) {
            for (uint i = 0; i < frames.size(); i++) {
                spriteSet->remove(firstIndex);
            }
        } else if (importMode == replaceFrames) {
            for (uint i = 0; i < oldFrames.size(); i++) {
                spriteSet->update(oldFrames[i], firstIndex + i);
            }
            // delete frames that wound up being appended
            for (uint i = oldFrames.size(); i < frames.size(); i++) {
                spriteSet->remove(spriteSet->getCount() - 1);
            }
        } else if (importMode == eraseAndReplaceFrames) {
            while (spriteSet->getCount() > 0) {
                spriteSet->remove(0);
            }
            for (uint i = 0; i < oldFrames.size(); i++) {
                spriteSet->append(oldFrames[i]);
            }
        } else {
            assert(false);
        }

        spriteSet->changed.fire(spriteSet);
    }

}};
