using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

using ika=Import.ika;

using rho;
using rho.Controls;

using Cataract.Video;

namespace rho.MapEditor
{

    class MapView : Form, IDocView
    {
        #region EventProxy
        /// <summary>
        /// A little proxy class used so that I don't have to generate a billion little dork functions
        /// for setting each and every single map editor state there is.
        /// <p/>
        /// Feed it the map, and the state, and it'll exist.  Bind the menu item to the OnSelect method
        /// and watch it go.  Super.
        /// </summary>
        class EventProxy
        {
            MapView view;
            State state;

            public void OnSelect(object o,EventArgs e)
            {
                view.state=this.state;
                Menu menu;

                // Check the menuitem just selected.  Uncheck all others.
                // This won't work if something other than a menuitem triggered the event, obviously
                // so we just bomb out if the cast turns out to be invalid.
                try
                {
                    menu=((MenuItem)o).Parent;
                }
                catch (InvalidCastException)
                {
                    return;
                }

                foreach (MenuItem m in menu.MenuItems)
                    m.Checked=(m==o);
            }

            public EventProxy(MapView mv,State s)
            {
                view=mv;
                state=s;
            }
        }
        #endregion

        TileSet tiles;
        ika.Map map;
        int xwin,ywin;
        MainForm parent;
        string filename;
	
        StatusBar    statusbar;
        ScrollWindow scrollwnd;
        OpenGLGraph  gfx;
        ListView     layercontrol;
        MainMenu     menu;
        protected MenuItem statemenu;

        State     state;

        #region Menu initialization
        private void InitMenu()
        {
            menu=new MainMenu();
            
            MenuItem filemenu=new MenuItem("&File",new MenuItem[]
                {
                    new MenuItem("-"),
                    new MenuItem("&Save",new EventHandler(SaveMap))
                }
                );
            filemenu.MergeType=MenuMerge.MergeItems;
            filemenu.MergeOrder=1;

            statemenu=new MenuItem("&Mode");
            foreach (State s in State.States)
                statemenu.MenuItems.Add(
                    new MenuItem(
                        s.Name,
                        new EventHandler(new EventProxy(this,s).OnSelect)
                    )
                );

            statemenu.MergeType=MenuMerge.MergeItems;
            statemenu.MergeOrder=2;

            menu.MenuItems.AddRange(new MenuItem[] { filemenu,statemenu });
            Menu=menu;
        }

        #endregion

        #region Control initialization
        void InitControls()
        {
            scrollwnd=new ScrollWindow();
            scrollwnd.Dock=DockStyle.Fill;
            Controls.Add(scrollwnd);

            gfx=new OpenGLGraph(scrollwnd.Width,scrollwnd.Height,32);
            gfx.Dock=DockStyle.Fill;
            gfx.Paint+=new PaintEventHandler(Redraw);
            scrollwnd.Controls.Add(gfx);
		
            scrollwnd.OnHScroll  +=new ScrollEventHandler(OnHScroll);
            scrollwnd.OnVScroll  +=new ScrollEventHandler(OnVScroll);

            gfx.MouseDown        +=new MouseEventHandler(OnMouseDown);
            gfx.MouseUp          +=new MouseEventHandler(OnMouseUp);
            gfx.MouseWheel       +=new MouseEventHandler(OnMouseWheel);
            gfx.MouseMove        +=new MouseEventHandler(OnMouseMove);

            map.EntityChanged    +=new ika.EntityChangedHandler(EntityChanged);
            map.LayerChanged     +=new ika.LayerChangedHandler(LayerChanged);
            map.PropertiesChanged+=new ika.MapChangedHandler(PropertiesChanged);

            statusbar=new StatusBar();
            statusbar.Dock=DockStyle.Bottom;
            Controls.Add(statusbar);
		
            Splitter splitter=new Splitter();
            splitter.Dock=DockStyle.Left;
            Controls.Add(splitter);
		
            layercontrol=new ListView();
            layercontrol.Dock=DockStyle.Left;
            layercontrol.Width=80;
            layercontrol.View=View.Details;
            Controls.Add(layercontrol);

            statusbar.SendToBack();	
				
            Resize+=new EventHandler(OnResize);
        }
        #endregion

        private void Init(MainForm p,ika.Map m,TileSet t)
        {
            parent=p;
            map=m;
            tiles=t;
		
            xwin=ywin=0;

            state=State.States[0];  // pick a state.  Any state!  It doesn't matter, as long as it's not null!

            InitControls();
            InitMenu();

        }

        public MapView(MainForm p)
        {
            ika.Map m=new ika.Map(100,100);
            m.AddLayer();
            m.RenderString="";

            Init(p,m,null);

            m.RenderString="1";
            TileSet t=new TileSet();
            tiles=t;

            filename="";
            Text="Untitled map";

            UpdateScrollBars();
        }
	
        public MapView(MainForm p,string fn) : base()
        {
            ika.Map m=(ika.Map)p.maps.Load(fn);

            string path=System.IO.Path.GetDirectoryName(fn);            // Get the directory that the map is in...
            TileSet t=(TileSet)p.tilesets.Load(path+"/"+m.TileSetName); // And read the VSP from that directory.

            Init(p,m,t);

            filename=fn;
            Text=filename;

            UpdateScrollBars();
        }
	
        void UpdateScrollBars()
        {
            scrollwnd.AutoScrollMinSize=new Size(map.Width*tiles.Width,map.Height*tiles.Height);	
        }
	
        void OnHScroll(object o,ScrollEventArgs e)
        {
            if (e.Type==ScrollEventType.EndScroll)
                return;
				
            XWin=e.NewValue;
		
            Redraw();
        }
	
        void OnVScroll(object o,ScrollEventArgs e)
        {
            if (e.Type==ScrollEventType.EndScroll)
                return;
		
            YWin=e.NewValue;

            Invalidate();
        }
	
        void OnResize(object o,EventArgs e)
        {
            UpdateScrollBars();
            Invalidate();
        }
	
        void EntityChanged(ika.Map m,int idx)
        {}

        void LayerChanged(ika.Map m,int idx,Rectangle r)
        {
            r.X=(r.X-XWin)*tiles.Width;
            r.Y=(r.Y-ywin)*tiles.Height;
            r.Width*=tiles.Width;
            r.Height*=tiles.Height;
            Redraw();
        }

        void PropertiesChanged(ika.Map m)
        {}

        void Redraw(object o,PaintEventArgs e)
        {
            Redraw();
        }

        void Redraw()
        {
            int n=0;

            gfx.Clear();
		
            foreach (char c in map.RenderString)
            {
                int i=(int)c-'1';
			
                if (i>=0 && i<map.NumLayers)
                {
                    DrawLayer(i,n!=0);
                    n++;
                }	
            }

            gfx.ShowPage();
        }

	    // This is WAY too slow.  TODO: either OpenGL, or C++ (neither option is very attractive, sadly)
        void DrawLayer(int layer,bool trans)
        {
            ika.LayerInfo li=map.get_LayerInfo(layer);

            // parallax
            int xw=(int)(1.0 * xwin * li.pmulx / li.pdivx);
            int yw=(int)(1.0 * ywin * li.pmuly / li.pdivy);				
		
            // first tile to be drawn
            int xtile= xw / tiles.Width;
            int ytile= yw / tiles.Height;
		
            // where to draw that first tile
            int xadj = - ( xw % tiles.Width  );
            int yadj = - ( yw % tiles.Height );
				
            // How many tiles to draw on each axis
            int xlen = gfx.XRes / tiles.Width  + 2;
            int ylen = gfx.YRes / tiles.Height + 2;
		
            // Clip
            if (xtile+xlen>map.Width)		xlen=map.Width-xtile;
            if (ytile+ylen>map.Height)	ylen=map.Height-ytile;
		
            // now we loop, and draw
            int xt=xtile;	// need this later
            
            int sy=yadj;    // Screen Y.  The spot where the next tile is drawn.
            for (int y=0; y<ylen; y++)
            {
                int sx=xadj;    // Screen X.
                for (int x=0; x<xlen; x++)
                {
                    int t=map.get_Tile(xtile,ytile,layer);
				
                    if (trans && t==0)
                    {
                        xtile++;
                        sx+=tiles.Height;
                        continue;
                    }
					
                    gfx.Blit(tiles.Images[t], sx,sy,trans);
				
                    sx+=tiles.Width;
                    xtile++;
                }
                
                sx=xadj;
                sy+=tiles.Height;

                ytile++;
                xtile=xt;
            }
        }

        protected override void OnClosed(EventArgs e)
        {
            parent.maps.Free(map);
            parent.tilesets.Free(tiles);

            base.OnClosed(e);
        }

        public string FileName
        {
            get {   return filename;    }
            set {   filename=value;     }
        }
        public int XWin
        {
            get {   return xwin;    }
            set
            {
                xwin=value;
                if (xwin<0) xwin=0;
                if (xwin>map.Width*tiles.Width-gfx.XRes)
                    xwin=map.Width*tiles.Width-gfx.XRes;
            }
        }

        public int YWin
        {
            get {   return ywin;    }
            set
            {
                ywin=value;
                if (ywin<0) ywin=0;
                if (ywin>map.Height*tiles.Height-gfx.YRes)
                    ywin=map.Height*tiles.Height-gfx.YRes;
            }
        }

        public ika.Map Map
        {
            get {   return map; }
        }


        public void Save()
        {
            Save(filename);
        }

        public void Save(string filename)
        {
        }

        public void GetTileCoords(ref int x,ref int y,int layer)
        {
            // Possible performance issue: we create a new LayerInstance info every time we call this.
            // Deal with it later.
            ika.LayerInfo li=map.get_LayerInfo(layer);
            x+=(XWin*li.pmulx/li.pdivx);
            y+=(YWin*li.pmuly/li.pdivy);

            x/=tiles.Width;
            y/=tiles.Height;
        }

        public void GetTileCoords(ref int x,ref int y)
        {
            x+=XWin;
            y+=YWin;

            x/=tiles.Width;
            y/=tiles.Height;
        }

        public void GetPixelCoords(ref int x,ref int y,int layer)
        {
            x*=tiles.Width;
            y*=tiles.Height;
            x-=XWin;
            y-=YWin;

            ika.LayerInfo li=map.get_LayerInfo(layer);
            if (li==null)
                return;

            x=x*li.pmulx/li.pdivx;
            y=y*li.pmuly/li.pdivy;
        }

        public void GetPixelCoords(ref int x,ref int y)
        {
            x*=tiles.Width;
            y*=tiles.Height;
            x-=XWin;
            y-=YWin;
        }

        void SaveMap(object o,EventArgs e)
        {

        }

        protected void OnMouseDown(object o,MouseEventArgs e)
        {
            state.MouseDown(this,e);
        }

        protected void OnMouseUp(object o,MouseEventArgs e)
        {
            state.MouseUp(this,e);
        }

        protected void OnMouseWheel(object o,MouseEventArgs e)
        {
            state.MouseWheel(this,e);
        }

        protected void OnMouseMove(object o,MouseEventArgs e)
        {
            state.MouseMove(this,e);
        }
    }

}