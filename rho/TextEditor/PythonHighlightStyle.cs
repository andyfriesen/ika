using System;
using Scintilla;

namespace Scintilla {
    enum MarkNum {
        FolderEnd = 25,
        FolderOpenMid = 26,
        FolderMidTail = 27,
        FolderTail = 28,
        FolderSub = 29,
        Folder = 30,
        FolderOpen = 31,
    }

    enum Mark {
        Max = 31,
        Circle = 0,
        RoundRect = 1,
        Arrow = 2,
        SmallRect = 3,
        ShortArrow = 4,
        Empty = 5,
        ArrowDown = 6,
        Minus = 7,
        Plus = 8,

        // Shapes used for outlining column.
        VLine = 9,
        LCorner = 10,
        TCorner = 11,
        BoxPlus = 12,
        BoxPlusConnected = 13,
        BoxMinus = 14,
        BoxMinusConnected = 15,
        LCornerCurve = 16,
        TCornerCurve = 17,
        CirclePlus = 18,
        CirclePlusConnected = 19,
        CircleMinus = 20,
        CircleMinusConnected = 21,

        // Invisible mark that only sets the line background color.
        Background = 22,
        DotDotDot = 23,
        Arrows = 24,
        Pixmap = 25,

        Character = 10000,
    }
    enum Margin {
        Symbol = 0,
        Number = 1,
    };

    [Flags]
    enum MarginMask {
        FolderEnd = 25,
        FolderOpenMid = 26,
        FolderMidTail = 27,
        FolderTail = 28,
        FolderSub = 29,
        Folder = 30,
        FolderOpen = 31,

        MaskFolders = (
            (1 << Folder) | 
            (1 << FolderEnd) |
            (1 << FolderSub) |
            (1 << FolderTail) |
            (1 << FolderMidTail) |
            (1 << FolderOpenMid) |
            (1 << FolderEnd))
    };

}

namespace rho.TextEditor {
    class PythonHighlightStyle : HighlightStyle {
        public void SetStyle(ScintillaControl c) {
            c.StyleSetFore(0,  0x00808080);  // whitespace
            c.StyleSetFore(1,  0x00007F00);  // code comments
            c.StyleSetItalic(1, true);
            c.StyleSetFont(1, "Comic Sans MS");
            c.StyleSetFore(2,  0x00008800);  // numeric constants
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
                "global as import finally exec del print in is assert from and not or None True False"
                );


            c.set_MarginWidthN(0, 10);
            c.set_MarginTypeN(0, (int)Margin.Symbol);
            c.set_MarginSensitiveN(0, true);
            c.set_MarginMaskN(0, (int)MarginMask.MaskFolders);

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
        }
    }
}
