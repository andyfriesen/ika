using System;

namespace rho.TextEditor {
    public class TextStyle : HighlightStyle {
        public void SetStyle(Scintilla.ScintillaControl text) {
            text.Lexer = Scintilla.ScintillaControl.LEX_NULL;
        }
    }
}
