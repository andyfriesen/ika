
// project created on 26/05/2002 at 6:38 AM
using System;
using System.Windows.Forms;
using System.IO;
using WeifenLuo.WinFormsUI;
using rho.Dialogs;

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

        //public TilesetController tilesets=new TilesetController();
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

        #endregion
	
        public MainForm() {
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MainForm));
            Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));

            System.Reflection.AssemblyName name = System.Reflection.Assembly.GetExecutingAssembly().GetName();
            Version version = name.Version;

#if DEBUG
            Text = string.Format("rho build {0}", version);
#else
            Text = string.Format("rho {0}.{1}", version.Major, version.Minor);
#endif

            dockManager.Parent = this;
            dockManager.Dock = DockStyle.Fill;
            dockManager.MdiIntegration = true;
            IsMdiContainer = true;

            MenuItem file = 
                MenuBuilder.menu("&File",
                    MenuBuilder.menu(0, "&New",
                        MenuBuilder.menu("&Script", new EventHandler(NewScript)),
                        MenuBuilder.menu("S&prite", new EventHandler(NewSprite))
                    ),
                    MenuBuilder.menu(1, "&Open", new EventHandler(OpenFile), Shortcut.CtrlO),
                    MenuBuilder.menu(998, "-"),
                    MenuBuilder.menu(999, "E&xit", new EventHandler(Exit))
                );

            file.MergeType = MenuMerge.MergeItems;
            file.MergeOrder = 1;

            MenuItem window = new MenuItem("&Window", new MenuItem[0]);
            window.MdiList = true;
            //window.MergeType = MenuMerge.MergeItems;
            window.MergeOrder = 3;

            Menu = new MainMenu(new MenuItem[] {
                    file,
                    window,                              
                }
            );

            int width = Width;
            if (width < 630) {
                width = 630;
            }
            int height = Height;
            if (height < 470) {
                height = 470;
            }
            Size = new System.Drawing.Size(width, height);
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

        void NewSprite(object o, EventArgs e) {
            using (ResizeDialog dlg = new ResizeDialog()) {
                if (dlg.ShowDialog() == DialogResult.OK) {

                    SpriteEditor.SpriteDocument sprite = new rho.SpriteEditor.SpriteDocument(dlg.NewSize.Width, dlg.NewSize.Height);
                    DockContent content = new SpriteEditor.SpriteEditor(sprite);
                    content.Text = "Untitled sprite";
                    content.Show(dockManager);
                }
            }
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

        [STAThread]
        public static void Main(string[] args) {
            MainForm f = new MainForm();
            foreach (string s in args) {
                f.CreateDocumentWindow(s);
            }

            Application.Run(new MainForm());
        }
    }
}
