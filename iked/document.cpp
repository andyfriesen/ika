
#include <stdexcept>

#include "document.h"
#include "documentresource.h"

#include "commands/command.h"

namespace iked {

    namespace {
        void clearStack(std::stack<commands::Command*>& stack) {
            while (!stack.empty()) {
                commands::Command* cmd = stack.top();
                delete cmd;
                stack.pop();
            }
        }
    }

    Document::Document()
        : refCount(0)
    {}

    int Document::getRefCount() const {
        return refCount;
    }

    void Document::ref() {
        assert(refCount >= 0);
        refCount++;
    }

    void Document::unref() {
        assert(refCount > 0);
        refCount--;
        if (refCount == 0) {
            // Temporarily make refcount 1, call destroyed event, then 
            // decrement and retest against 0, just in case somebody 
            // revived the document.
            // Potential bug: an erronious unref() someplace else may cause scary infinite loops!
            refCount = 1;
            destroyed.fire(this);
            if (refCount == 1) {
                delete this;
            }
        }
    }

    Document::~Document() {
        destroyed.fire(this);
    }

    AbstractDocument::~AbstractDocument() {
        clearStack(undoStack);
        clearStack(redoStack);
    }

    void AbstractDocument::sendCommand(commands::Command* cmd) {
        // CommandContext is not used presently. :P
        cmd->Do(0);
        clearStack(redoStack);
        undoStack.push(cmd);
    }

    bool AbstractDocument::canUndo() {
        return !undoStack.empty();
    }

    bool AbstractDocument::canRedo() {
        return !redoStack.empty();
    }

    void AbstractDocument::undo() {
        if (!undoStack.empty()) {
            commands::Command* cmd = undoStack.top();
            undoStack.pop();
            
            cmd->Undo(0);
            redoStack.push(cmd);
        }
    }

    void AbstractDocument::redo() {
        if (!redoStack.empty()) {
            commands::Command* cmd = redoStack.top();
            redoStack.pop();
            
            cmd->Do(0);
            undoStack.push(cmd);
        }
    }
}
