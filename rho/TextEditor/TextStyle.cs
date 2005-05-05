using System;

namespace rho.TextEditor {
    public class TextStyle : HighlightStyle {
        public void SetStyle(Scintilla.ScintillaText text) {
            text.Lexer = Scintilla.Lexer.Null;
        }
    }
}
