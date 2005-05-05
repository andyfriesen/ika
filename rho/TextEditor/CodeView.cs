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
        readonly ScintillaText text = new ScintillaText();

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
                string content = txt.ReadToEnd();
                if (content == "") {
                    content = " "; // bug in scintillaNET: freaks out if you give it an empty string. :P
                }
                    
                text.Text = content;
            }

            Text = fname;
        }
    }
}
