using System;
using System.Windows.Forms;

using Scintilla;

namespace rho
{
    interface IHighlightStyle
    {
    }

	/// <summary>
	/// Summary description for CodeView.
	/// </summary>
	class CodeView : Form
	{
		public CodeView(MainForm f,string fname,IHighlightStyle style) : base()
		{
		}
	}
}
