using System;
using Scintilla;

namespace rho.TextEditor {
    public interface HighlightStyle {
        void SetStyle(ScintillaControl text);
    }
}
