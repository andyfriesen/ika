// project created on 26/05/2002 at 6:38 AM
using System;
using System.Windows.Forms;
using System.IO;

using Import.ika;

namespace rho
{
    class MainForm : Form
    {
        public TileSetController tilesets=new TileSetController();
	
        public MainForm()
        {
            Text = "rho alpha 1";
            IsMdiContainer=true;
		
            Width=640;
            Height=480;
		
            MenuItem file=new MenuItem("&File", new MenuItem[]
                                {
                                    new MenuItem("&New", new MenuItem[]
                                    {
                                        new MenuItem("&Script", new EventHandler(NewScript)),
                                    }),
                                    
                                    new MenuItem("&Open", new EventHandler(OpenFile)),
                                    new MenuItem("-"),
                                    new MenuItem("E&xit", new EventHandler(Exit))
                                });
            file.MergeType=MenuMerge.MergeItems;
            file.MergeOrder=1;

            MenuItem window=new MenuItem("&Window", new MenuItem[]
                                {
                                });
            window.MdiList=true;
            window.MergeType=MenuMerge.MergeItems;
            window.MergeOrder=3;
		
            Menu=new MainMenu(new MenuItem[]
                        {
                            file,
                            window,                              
                        }
                );

            Splitter splitter=new Splitter();
            splitter.Dock=DockStyle.Left;
            Controls.Add(splitter);		
            splitter.Show();

            TreeView tree=new TreeView();
            tree.Dock=DockStyle.Left;
            Controls.Add(tree);
            tree.Show();
        }

        void CreateDocumentWindow(string filename)
        {
            string extension=Path.GetExtension(filename).ToLower();
            Form doc;

            switch (extension)
            {
                case ".py":  
                    doc=new TextEditor.CodeView(this, filename, new TextEditor.PythonHighlightStyle());    
                    break;

                case ".cs":
                    doc = new TextEditor.CodeView(this, filename, new TextEditor.CSharpHighlightStyle());
                    break;

                default:
                    MessageBox.Show("rho", String.Format("Unrecognized File type \"{0}\"", extension));
                    return;
            }

            doc.MdiParent=this;
            doc.Show();
        }

        void NewScript(object o, EventArgs e)
        {
            TextEditor.CodeView codeview = new TextEditor.CodeView(this, new TextEditor.PythonHighlightStyle());
            codeview.MdiParent = this;
            codeview.Show();
        }

        void OpenFile(object o, EventArgs e)
        {
            using (OpenFileDialog dlg=new OpenFileDialog())
            {
                dlg.Title="Open Document...";
                dlg.Filter="All known|*.map;*.py|Map files (*.map)|*.map|Python Scripts (*.py)|*.py|All Files (*.*)|*.*";
                dlg.Multiselect=true;

                DialogResult result=dlg.ShowDialog(this);

                if (result==DialogResult.OK)
                {
                    foreach (string s in dlg.FileNames)
                        CreateDocumentWindow(s);
                }
            }
        }

        void Exit(object o, EventArgs e)
        {
            Close();
        }
	
        public static void Main(string[] args)
        {
            MainForm f=new MainForm();
            foreach (string s in args)
                f.CreateDocumentWindow(s);

            Application.Run(new MainForm());
        }
    }

}