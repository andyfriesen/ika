using System;
using Scintilla;

namespace rho.TextEditor {
    class CSharpHighlightStyle : HighlightStyle {
        public void SetStyle(ScintillaControl c) {
            c.StyleSetFore(1, 0x00888888);	// comments
            c.StyleSetFore(2, 0x00888888);	// line comments
            c.StyleSetFore(3, 0x0);			// doc comments (?)
            c.StyleSetFore(4, 0x00FF00FF);	// numeric constants
            c.StyleSetFore(5, 0x00FF0000);	// kewords
            c.StyleSetBold(5, true);
            c.StyleSetFore(6, 0x00FF8888);	// string literal
            c.StyleSetFore(7, 0);
            c.StyleSetFore(10, 0x00008800);
            c.StyleSetItalic(2, true);
            c.StyleSetBold(10, true);
		
            c.set_KeyWords(0, "if else do while switch case "+	// flow control
                "void int uint string char byte bool object "+	// intrinsic types
                "decimal enum float double sbyte short long ushort ulong "+
                "using namespace as "+				// namespace stuff
                "ref out params "+
                "interface class struct public delegate event "+
                "public private protected internal "+
                "sealed unsafe abstract base this virtual override static const "+			// classes
                "extern new true false sizeof typeof "+				// misc
                "stackalloc is foreach "+
                "return break continue goto default "+
                "try catch throw finally "+
                "checked unchecked "+
                "implicit explicit operator "+
                "add remove set get "+
                "null value "
		                
                );

            c.Lexer = ScintillaControl.LEX_CPP;
        }
    }
}
