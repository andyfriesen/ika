using System;
using Scintilla;

namespace rho.TextEditor {
    class PythonHighlightStyle : HighlightStyle {
        public void SetStyle(ScintillaText c) {
            c.SetStyleFore(0,  0x00808080);  // whitespace
            c.SetStyleFore(1,  0x00007F00);  // code comments
            c.SetStyleItalic(1, true);
            c.SetStyleFont(1, "Comic Sans MS");
            c.SetStyleFore(2,  0x00008800);  // numeric constants
            c.SetStyleFont(2, "Verdana");
            c.SetStyleFore(3,  0x00000080);  // " style string literals
            c.SetStyleFont(3, "Courier New");
            c.SetStyleFore(4,  0x00000080);  // ' style string literals
            c.SetStyleFont(4, "Courier New");
            c.SetStyleFore(5,  0x00800000);  // keyword
            c.SetStyleFont(5, "Verdana");
            c.SetStyleBold(5,  true);
            c.SetStyleFore(6,  0x007F0000);  // ''' strings
            c.SetStyleFont(6, "Courier New");
            c.SetStyleFore(7,  0x007F0000);  // """ strings
            c.SetStyleFont(7, "Courier New");
            c.SetStyleFore(8,  0x00007F7F);  // class declaration name
            c.SetStyleFont(8, "Verdana");
            c.SetStyleFore(9,  0x00007F7F);  // function declarations
            c.SetStyleFont(9,  "Verdana");
            c.SetStyleFore(10, 0x007F007F);  // operators
            c.SetStyleFont(10, "Verdana");
            c.SetStyleBold(10, true);
            c.SetStyleFore(11, 0x00000000);  // identifiers
            c.SetStyleFont(11, "Verdana");
            
            c.Lexer = Lexer.Python;

            c.SetKeyWords(0,
                "def lambda class return yield try raise except pass for while if else elif break continue "+
                "global as import finally exec del print in is assert from and not or None True False"
            );


            c.SetMarginWidth(0, 10);
            c.SetMarginType(0, Margin.Symbol);
            c.SetMarginSensitivity(0, true);
            c.SetMarginMask(0, MarginMask.MaskFolders);

            c.Fold = true;
#if false
            c.set_Property("fold", "1");

            MarkNum[] marknum = new MarkNum[] { 
                MarkNum.FolderOpen, MarkNum.Folder, MarkNum.FolderSub, 
                MarkNum.FolderTail, MarkNum.FolderEnd, 
                MarkNum.FolderOpenMid, MarkNum.FolderMidTail 
            };

            Mark[][] styles = new Mark[][] {
                new Mark[] { Mark.Minus, Mark.Plus, Mark.Empty, Mark.Empty, Mark.Empty, Mark.Empty, Mark.Empty }, 
                new Mark[] { Mark.CircleMinus, Mark.CirclePlus, Mark.VLine, Mark.LCornerCurve, Mark.CirclePlusConnected, Mark.CircleMinusConnected, Mark.TCornerCurve }, 
                new Mark[] { Mark.BoxMinus, Mark.BoxPlus, Mark.VLine, Mark.LCorner, Mark.BoxPlusConnected, Mark.BoxMinusConnected, Mark.TCorner }
            };

            for (int i = 0; i < marknum.Length; i++) {
                c.MarkerDefine((int)marknum[i], (int)(styles[2][i]));
            }
#endif
        }
    }
}
