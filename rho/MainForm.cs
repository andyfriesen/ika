// project created on 26/05/2002 at 6:38 AM
using System;
using System.Windows.Forms;

namespace rho
{

    class MainForm : Form
    {
        public TileSetController tilesets=new TileSetController();
        public MapController maps=new MapController();
	
        public MainForm()
        {
            Text = "rho alpha 1";
            IsMdiContainer=true;
		
            Width=640;
            Height=480;
		
            MenuItem file=new MenuItem("&File",new MenuItem[]
                                {
                                    new MenuItem("&New",new MenuItem[]
                                    {
                                        new MenuItem("&Map",new EventHandler(NewMap)),
                                }),
                                    new MenuItem("&Open",new MenuItem[]
                                    {
                                        new MenuItem("&Map",new EventHandler(OpenMap)),
                                        new MenuItem("&Script",new EventHandler(OpenScript)),
                                }),
                                    new MenuItem("-"),
                                    new MenuItem("E&xit",new EventHandler(Exit))
                                });
		
            Menu=new MainMenu(new MenuItem[]
                          {
                              file
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
	
        void NewMap(object o,EventArgs e)
        {
            MapView mapview=new MapView(this);
            mapview.MdiParent=this;
            mapview.Show();
        }

        void OpenMap(object o,EventArgs e)
        {
            using (OpenFileDialog dlg=new OpenFileDialog())
            {
                dlg.Filter="v2 map files (*.map)|*.map";

                DialogResult result=dlg.ShowDialog(this);

                if (result==DialogResult.OK)
                {
                    MapView mapview=new MapView(this,dlg.FileName);
                    mapview.MdiParent=this;
                    mapview.Show();
                }
            }
        }

        void OpenScript(object o,EventArgs e)
        {
            using (OpenFileDialog dlg=new OpenFileDialog())
            {
                dlg.Filter="Python scripts (*.py)|*.py";

                DialogResult result=dlg.ShowDialog(this);

                if (result==DialogResult.OK)
                {
                    CodeView codeview=new CodeView(this,dlg.FileName,new PythonHighlightStyle());
                    codeview.MdiParent=this;
                    codeview.Show();
                }
            }
        }
	
        void Exit(object o,EventArgs e)
        {
            Close();
        }
	
        public static void Main(string[] args)
        {
            Application.Run(new MainForm());
        }
    }

}