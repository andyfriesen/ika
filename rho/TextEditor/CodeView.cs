using System;
using System.Windows.Forms;
using System.IO;
using WeifenLuo.WinFormsUI;

using Scintilla;

namespace rho.TextEditor {
    /// <summary>
    /// Summary description for CodeView.
    /// </summary>
    class CodeView : DockContent {
        readonly ScintillaControl text = new ScintillaControl();

        void Init() {
            text.Dock = DockStyle.Fill;
            Controls.Add(text);
            text.Show();
        }

        public CodeView(MainForm f, HighlightStyle style) : base() {
            Init();
            style.SetStyle(text);
            Text = "Untitled Script";
        }

        public CodeView(MainForm f, string fname, HighlightStyle style) : base() {
            Init();

            style.SetStyle(text);

            using (StreamReader txt = new StreamReader(fname)) {
#if true
                string content = txt.ReadToEnd();
                text.Text = content;
#else
                text.Text = txt.ReadToEnd();
#endif
            }

            Text=fname;
        }
    }
}
