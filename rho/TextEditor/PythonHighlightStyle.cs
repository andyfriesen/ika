using System;
using Scintilla;

namespace rho.TextEditor {
    class PythonHighlightStyle : HighlightStyle {
        public void SetStyle(ScintillaControl c) {
            c.StyleSetFore(0,  0x00808080);  // whitespace
            c.StyleSetFore(1,  0x00007F00);  // code comments
            c.StyleSetItalic(1, true);
            c.StyleSetFont(1, "Comic Sans MS");
            c.StyleSetFore(2,  0x000000FF);  // numeric constants
            c.StyleSetFont(2, "Verdana");
            c.StyleSetFore(3,  0x00000080);  // " style string literals
            c.StyleSetFont(3, "Courier New");
            c.StyleSetFore(4,  0x00000080);  // ' style string literals
            c.StyleSetFont(4, "Courier New");
            c.StyleSetFore(5,  0x00800000);  // keyword
            c.StyleSetFont(5, "Verdana");
            c.StyleSetBold(5,  true);
            c.StyleSetFore(6,  0x007F0000);  // ''' strings
            c.StyleSetFont(6, "Courier New");
            c.StyleSetFore(7,  0x007F0000);  // """ strings
            c.StyleSetFont(7, "Courier New");
            c.StyleSetFore(8,  0x00007F7F);  // class declaration name
            c.StyleSetFont(8, "Verdana");
            c.StyleSetFore(9,  0x00007F7F);  // function declarations
            c.StyleSetFont(9,  "Verdana");
            c.StyleSetFore(10, 0x007F007F);  // operators
            c.StyleSetFont(10, "Verdana");
            c.StyleSetBold(10, true);
            c.StyleSetFore(11, 0x00000000);  // identifiers
            c.StyleSetFont(11, "Verdana");
            
            c.Lexer = ScintillaControl.LEX_PYTHON;

            c.set_KeyWords(0,
                "def lambda class return yield try raise except pass for while if else elif break continue "+
                "global as import finally exec del print in is assert from and not or None"
                );
        }
    }
}
