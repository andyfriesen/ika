// project created on 26/05/2002 at 6:38 AM
using System;
using System.Windows.Forms;
using System.IO;
using WeifenLuo.WinFormsUI;

#if false
using Import.ika;
#endif

namespace rho {
    struct FileType {
        public string Name;
        public string[] Extensions;

        public FileType(string name, params string[] extensions) {
            Name = name;
            Extensions = extensions;
        }

        public override string ToString() {
            return string.Format("{0} ({1})|{1}", Name, string.Join(";", Extensions));
        }
    }

    class MainForm : Form {
        static readonly FileType[] knownExtensions = {
            new FileType("Text", "*.txt"),
            new FileType("Python scripts", "*.py", "*.pyw"),
            new FileType("C# source", "*.cs"),
            new FileType("Sprites", "*.ika-sprite")
        };

        static readonly string fileFilter = CreateFilter(knownExtensions);

        //public TileSetController tilesets=new TileSetController();
        readonly DockPanel dockManager = new DockPanel();

        #region Stupid convenience functions that make me REALLY wish Python was faster and ran on .NET better
        static string CreateFilter(FileType[] types) {
            System.Text.StringBuilder sb1 = new System.Text.StringBuilder();
            foreach (FileType ft in types) {
                sb1.Append(";");
                sb1.Append(string.Join(";", ft.Extensions));
            }

            string glob = sb1.ToString().Substring(1); // clip leading ;

            System.Text.StringBuilder sb = new System.Text.StringBuilder();
            sb.AppendFormat("All known ({0})|{0}", glob);
            foreach (FileType ft in types) {
                sb.AppendFormat("|{0} ({1})|{1}", ft.Name, string.Join(";", ft.Extensions));
            }
            sb.Append("|All (*.*)|*.*");

            return sb.ToString();
        }

        private static MenuItem menu(string name, params MenuItem[] children) {
            return new MenuItem(name, children);
        }

        private static MenuItem menu(string name, EventHandler func) {
            return new MenuItem(name, func);
        }

        private static MenuItem menu(string name, EventHandler func, Shortcut shortcut) {
            return new MenuItem(name, func, shortcut);
        }

        private static MenuItem separator() {
            return new MenuItem("-");
        }
        #endregion
	
        public MainForm() {
            Text = "rho alpha 1";
		
            dockManager.Parent = this;
            dockManager.Dock = DockStyle.Fill;

            MenuItem file = 
                menu("&File",
                    menu("&New",
                        menu("&Script", new EventHandler(NewScript))
                ),
                menu("&Open", new EventHandler(OpenFile), Shortcut.CtrlO),
                separator(),
                menu("E&xit", new EventHandler(Exit))
                );
            file.MergeType=MenuMerge.MergeItems;
            file.MergeOrder=1;

            MenuItem window=new MenuItem("&Window", new MenuItem[] {
                                                                   });
            window.MdiList=true;
            window.MergeType=MenuMerge.MergeItems;
            window.MergeOrder=3;
		
            Menu=new MainMenu(new MenuItem[] {
                    file,
                    window,                              
                }
            );
        }

        void CreateDocumentWindow(string filename) {
            string extension=Path.GetExtension(filename).ToLower();
            DockContent doc;

            switch (extension) {
                case ".py":  
                    doc = new TextEditor.CodeView(this, filename, new TextEditor.PythonHighlightStyle());    
                    break;

                case ".cs":
                    doc = new TextEditor.CodeView(this, filename, new TextEditor.CSharpHighlightStyle());
                    break;

                case ".txt":
                    doc = new TextEditor.CodeView(this, filename, new TextEditor.TextStyle());
                    break;

                case ".ika-sprite":
                    SpriteEditor.SpriteDocument sprite = new rho.SpriteEditor.SpriteDocument(filename);
                    doc = new SpriteEditor.SpriteEditor(sprite);
                    break;

                default:
                    MessageBox.Show("rho", String.Format("Unrecognized File type \"{0}\"", extension));
                    return;
            }

            doc.Text = System.IO.Path.GetFileName(filename);

            doc.Show(dockManager);
        }

        void NewScript(object o, EventArgs e) {
            TextEditor.CodeView codeview = new TextEditor.CodeView(this, new TextEditor.PythonHighlightStyle());
            codeview.MdiParent = this;
            codeview.Show();
        }

        void OpenFile(object o, EventArgs e) {
            using (OpenFileDialog dlg=new OpenFileDialog()) {
                dlg.Title = "Open Document...";
                dlg.Filter = fileFilter;
                dlg.Multiselect = true;

                DialogResult result=dlg.ShowDialog(this);

                if (result==DialogResult.OK) {
                    foreach (string s in dlg.FileNames) {
                        CreateDocumentWindow(s);
                    }
                }
            }
        }

        void Exit(object o, EventArgs e) {
            Close();
        }
	
        public static void Main(string[] args) {
            MainForm f=new MainForm();
            foreach (string s in args)
                f.CreateDocumentWindow(s);

            Application.Run(new MainForm());
        }
    }
}
