
#ifndef COPYPASTESTATE_H
#define COPYPASTESTATE_H

#include "editstate.h"
#include "brush.h"

#include "common/utility.h"
#include "common/matrix.h"

struct CopyPasteState : EditState {
    CopyPasteState(Executor* e);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnRenderCurrentLayer();
    virtual void OnEndState();

    void Paste();           // dumps whatever's in our buffer to the map layer, and clears the selection.

private:
    enum State {
        NoneState,
        AddState,
        SubtractState,
        MoveState,
    };

    State _state;

    Brush _clipboard;
    Brush _tiles;           // TODO: copy this junk to the clipboard, so that it becomes possible to copy/paste tiles between concurrently open instances.

    int _selX;              // x/y coordinates of the floating selection
    int _selY;

    int _offsetX;           // These are the offsets between the selection rect, and the mouse cursor.
    int _offsetY;           // They allow us to make dragging look more natural.

    int _dragX;
    int _dragY;
    bool _dragging;         // true if we're currently dragging a selection rect.

    Rect _selection;        // current selection rect
};

#endif
