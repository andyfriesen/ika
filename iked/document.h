
#pragma once

#include <string>
#include <stack>

#include "commands/command.h"
#include "common/listener.h"
#include "common/refcount.h"

namespace iked {

    namespace commands {
        struct Command;
    }

    template <typename T> struct Controller;

    /**
     * Wraps a document.  Forces mutations to an actual resource to go
     * through the command interface.  Maintains the undo stack.
     */
    struct Document {
        Document();

        int getRefCount() const;
        void ref();
        void unref();

        /// Returns the filename of the document, or some otherwise interesting name.
        virtual std::string getName() = 0;

        /// Returns true if the undo stack isn't where it was when the document was last saved.
        virtual bool isChanged() = 0;
        
        virtual void save(const std::string& fileName) = 0;
        void save() {
            save(getName());
        }

        virtual void sendCommand(commands::Command* cmd) = 0;
        virtual bool canUndo() = 0;
        virtual bool canRedo() = 0;
        virtual void undo() = 0;
        virtual void redo() = 0;

        // Temp hack: (icky!)
        virtual struct TileSet* asTileSet() = 0;
        virtual struct Font* asFont() = 0;
        virtual struct Text* asText() = 0;
        virtual struct SpriteSet* asSpriteSet() = 0;

        // Events
        Listener<Document*> changed;
        Listener<Document*> destroyed;

    protected:
        virtual ~Document();

        int refCount;
    };

    /**
     * Default implementation of some of the Document interface
     */
    struct AbstractDocument : Document {
        AbstractDocument(const std::string& n="") 
            : name(n)
        { }

        ~AbstractDocument();

        virtual std::string getName() { 
            return name; 
        }

        virtual bool isChanged() {
            return false; // NYI
        }

        virtual void save(const std::string& fileName) = 0;

        virtual void sendCommand(commands::Command* cmd);
        virtual bool canUndo();
        virtual bool canRedo();
        virtual void undo();
        virtual void redo();

        // Temp hack: (icky!)
        virtual struct TileSet* asTileSet() { return 0; }
        virtual struct Font* asFont() { return 0; }
        virtual struct Text* asText() { return 0; }
        virtual struct SpriteSet* asSpriteSet() { return 0; }

    private:
        std::string name;

        std::stack<commands::Command*> undoStack;
        std::stack<commands::Command*> redoStack;
    };
}
