using System;
using System.Windows.Forms;

using Scintilla;

namespace rho
{
    interface IHighlightStyle
    {
        void SetStyle(ScintillaText text);
    }

    class CSharpHighlightStyle : IHighlightStyle
    {
        public void SetStyle(ScintillaText c)
        {
            c.SetStyleFore(1,0x00888888);	// comments
            c.SetStyleFore(2,0x00888888);	// line comments
            c.SetStyleFore(3,0x0);			// doc comments (?)
            c.SetStyleFore(4,0x00FF00FF);	// numeric constants
            c.SetStyleFore(5,0x00FF0000);	// kewords
            c.SetStyleBold(5,true);
            c.SetStyleFore(6,0x00FF8888);	// string literal
            c.SetStyleFore(7,7);
            c.SetStyleFore(10,0x00008800);
            c.SetStyleItalic(2,true);
            c.SetStyleBold(10,true);
		
            c.SetKeyWords(0,"if else do while switch case "+	// flow control
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

            c.Lexer=Lexer.Cpp;
        }
    }

    class PythonHighlightStyle : IHighlightStyle
    {
        public void SetStyle(ScintillaText c)
        {
            c.SetStyleFore(0,  0x00808080);  // whitespace
            c.SetStyleFore(1,  0x00007F00);  // code comments
            c.SetStyleFore(2,  0x00007F00);  // numeric constants
            c.SetStyleFore(3,  0x007F7F7F);  // " style string literals
            c.SetStyleFore(4,  0x00007F7F);  // ' style string literals
            c.SetStyleFore(5,  0x0000007F);  // keyword
            c.SetStyleFore(6,  0x007F007F);  // ''' strings
            c.SetStyleFore(7,  0x007F007F);  // """ strings
            c.SetStyleFore(8,  0x00007F7F);  // class declaration name
            c.SetStyleFore(9,  0x00007F7F);  // function declarations
            c.SetStyleFore(10, 0x007F007F);  // operators
            c.SetStyleFore(11, 0x00000000);  // identifiers
            c.SetStyleBold(5,  true);
            c.SetStyleBold(10, true);
            c.SetStyleItalic(1,true);
            
            c.Lexer=Lexer.Python;

            c.SetKeyWords(0,
                "def lambda class return yield try raise except pass for while if else elif break continue "+
                "global as import finally exec del print in is assert from and not or None"
                );
        }
    }

    /// <summary>
	/// Summary description for CodeView.
	/// </summary>
	class CodeView : Form
	{
        ScintillaText text;

		public CodeView(MainForm f,string fname,IHighlightStyle style) : base()
		{
            text=new ScintillaText();
            text.Dock=DockStyle.Fill;
            Controls.Add(text);
            text.Show();

            if (style!=null)
                style.SetStyle(text);
		}
	}
}
