
#pragma once

#include <string>

#include "commands/command.h"
#include "common/listener.h"
#include "common/refcount.h"

namespace iked {

    namespace commands {
        struct Command;
    }

    struct DocumentResource;

    template <typename T> struct Controller;

    /**
     * Wraps a document resource.  Forces mutations to that resource to go
     * through the command interface, maintains the undo stack.
     */
    struct Document : RefCounted {
        virtual ~Document();

        virtual std::string getName() = 0;
        virtual bool isChanged() = 0;
        
        void save() { save(getName()); }
        virtual void save(const std::string& fileName) = 0;

        virtual void sendCommand(commands::Command* cmd) = 0;

        // Temp hack: (icky!)
        virtual struct TileSet* asTileSet() = 0;
        virtual struct Font* asFont() = 0;
        virtual struct Text* asText() = 0;
        virtual struct SpriteSet* asSpriteSet() = 0;

        // Events
        Listener<Document*> destroyed;  // fired when the document is deallocated.
    };

    /**
     * Document that does all the usual things for some resource T.
     */
    struct AbstractDocument : Document {
        AbstractDocument(const std::string& n="") 
            : name(n)
        { }

        /// Returns the filename of the document, or some otherwise interesting name.
        virtual std::string getName() { 
            return name; 
        }

        /// True if the undo stack isn't where it was when the document was last saved.
        virtual bool isChanged() {
            return false; // NYI
        }

        virtual void save(const std::string& fileName) = 0;

        virtual void sendCommand(commands::Command* cmd) = 0;

        // Temp hack: (icky!)
        virtual struct TileSet* asTileSet() { return 0; }
        virtual struct Font* asFont() { return 0; }
        virtual struct Text* asText() { return 0; }
        virtual struct SpriteSet* asSpriteSet() { return 0; }

    private:
        std::string name;
    };
}
