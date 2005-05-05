using System;
using Scintilla;

namespace rho.TextEditor {
    public interface HighlightStyle {
        void SetStyle(ScintillaText text);
    }
}
