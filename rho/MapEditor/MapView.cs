using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

using Import.ika;

using rho;
using rho.Controls;

namespace rho.MapEditor
{

    class MapView : Form, IDocView
    {
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

            public void OnSelect(object o, EventArgs e)
            {
                view.state = this.state;

                // Check the menuitem just selected.  Uncheck all others.
                Menu menu=((MenuItem)o).Parent;
                foreach (MenuItem m in menu.MenuItems)
                    m.Checked=(m == o);
            }

            public EventProxy(MapView mv, State s)
            {
                view = mv;
                state = s;
            }
        }

        TileSet tiles;
        Map map;
        int xwin, ywin;
        MainForm parent;
        string filename;
	
        StatusBar  statusbar;
        GraphView  gfx;
        ListView   layercontrol;
        MainMenu   menu;
        protected MenuItem       statemenu;

        State     state;

        object[][] states;

        #region Menu Initialization
        private void InitMenu()
        {
            menu = new MainMenu();
            
            MenuItem filemenu = new MenuItem("&File", new MenuItem[]
                {
                    new MenuItem("-"),
                    new MenuItem("&Save", new EventHandler(SaveMap))
                }
                );
            filemenu.MergeType = MenuMerge.MergeItems;
            filemenu.MergeOrder = 1;

            statemenu = new MenuItem("&Mode");
            foreach (object[] o in states)
                statemenu.MenuItems.Add(
                    new MenuItem(
                        o[0].ToString(),
                        new EventHandler(new EventProxy(this, (State)o[1]).OnSelect)
                    )
                );

            statemenu.MergeType = MenuMerge.MergeItems;
            statemenu.MergeOrder = 2;

            menu.MenuItems.AddRange(new MenuItem[] { filemenu, statemenu });
            Menu = menu;
        }
        #endregion

        #region Control Initialization
        void InitControls()
        {
            gfx=new GraphView();
            gfx.Dock=DockStyle.Fill;
            gfx.Paint+=new PaintEventHandler(Redraw);
            Controls.Add(gfx);
            
            //scrollwnd.Layout += new LayoutEventHandler(LayoutThing);
		
            gfx.OnHScroll  +=new ScrollEventHandler(OnHScroll);
            gfx.OnVScroll  +=new ScrollEventHandler(OnVScroll);

            gfx.MouseDown        +=new MouseEventHandler(OnMouseDown);
            gfx.MouseUp          +=new MouseEventHandler(OnMouseUp);
            gfx.MouseWheel       +=new MouseEventHandler(OnMouseWheel);
            gfx.MouseMove        +=new MouseEventHandler(OnMouseMove);

            //map.EntityChanged    +=new EntityChangedHandler(EntityChanged);
            //map.LayerChanged     +=new LayerChangedHandler(LayerChanged);
            //map.PropertiesChanged+=new MapChangedHandler(PropertiesChanged);

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

        // What a mess. -_-;
        private void Init(MainForm p, Map m, TileSet t)
        {
            // When we add a new map editor state, just add another entry here.
            // I have a handy dandy loop that converts this to menu items. :D
            states = new object[][]
            {
                new object[]    {   "&Tiles",   new TileSetState()  },
            };

            InitControls();
            InitMenu();

            parent = p;
            map = m;
            tiles = t;
		
            xwin = ywin = 0;
		
            state = new TileSetState();
        }

        public MapView(MainForm p)
        {
            Map m = new Map(100, 100);
            m.AddLayer();
            m.RenderString = "1";

            TileSet t = new TileSet();
            //t.AppendTile();

            Init(p, m, t);

            filename = "";
            Text = "Untitled map";

            UpdateScrollBars();
        }
	
        public MapView(MainForm p, string fn) : base()
        {
            Map m = (Map)p.maps.Load(fn);

            string path = System.IO.Path.GetDirectoryName(fn);            // Get the directory that the map is in...
            TileSet t = (TileSet)p.tilesets.Load(path + "/" + m.TileSetName); // And read the VSP from that directory.

            Init(p, m, t);

            filename = fn;
            Text = filename;

            UpdateScrollBars();
        }
	
        void UpdateScrollBars()
        {
            gfx.ScrollSize = new Size(map.Width*tiles.Width, map.Height*tiles.Height);	
            gfx.PageSize = ClientSize;
        }
	
        void OnHScroll(object o, ScrollEventArgs e)
        {
            if (e.Type == ScrollEventType.EndScroll)
                return;
				
            XWin = e.NewValue;
		
            gfx.Invalidate();
        }
	
        void OnVScroll(object o, ScrollEventArgs e)
        {
            if (e.Type == ScrollEventType.EndScroll)
                return;
		
            YWin = e.NewValue;

            gfx.Invalidate();
        }
	
        void OnResize(object o, EventArgs e)
        {
            UpdateScrollBars();

            // Need a way to find out how big the window was before now.
            // Then we have at most two skinny rects that need to be redrawn.
            //    gfx.Invalidate();
        }
	
        void Redraw(object o, PaintEventArgs e)
        {
            // Disable alpha blending for the first layer...
            e.Graphics.CompositingMode = CompositingMode.SourceCopy;
		
            int n = 0;
		
            foreach (char c in map.RenderString)
            {
                int i=(int)c - '1';
			
                if (i >= 0 && i < map.NumLayers)
                {
                    DrawLayer(i, e, n != 0);

                    if (n == 0)
                        // ... and enable it for all subsequent layers
                        e.Graphics.CompositingMode = CompositingMode.SourceOver;

                    n++;
                }	
            }
        }
	
        void DrawLayer(int layer, PaintEventArgs e, bool trans)
        {
            LayerInfo li = map.get_LayerInfo(layer);

            // parallax
            int xw=(int)(1.0 * xwin * li.pmulx / li.pdivx);
            int yw=(int)(1.0 * ywin * li.pmuly / li.pdivy);				
		
            // Only drawing the specified rect.  Dirty rectangles are fast. :D
            xw += e.ClipRectangle.Left;
            yw += e.ClipRectangle.Top;
		
            // first tile to be drawn
            int xtile= xw / tiles.Width;
            int ytile= yw / tiles.Height;
		
            // where to draw that first tile
            int xadj = - ( xw % tiles.Width  );
            int yadj = - ( yw % tiles.Height );
				
            // How many tiles to draw on each axis
            int xlen = e.ClipRectangle.Width / tiles.Width  + 2;
            int ylen = e.ClipRectangle.Height/ tiles.Height + 2;
		
            // Clip
            if (xtile+xlen>map.Width)		xlen = map.Width-xtile;
            if (ytile+ylen>map.Height)	ylen = map.Height-ytile;

            // now we loop, and draw
            int xt = xtile;	// need this later
            for (int y = 0; y<ylen; y++)
            {
                for (int x = 0; x<xlen; x++)
                {
                    int t = map.get_Tile(xtile, ytile, layer);
				
                    if (trans && t == 0)
                    {
                        xtile++;
                        continue;
                    }
					
                    e.Graphics.DrawImage(tiles[t], x*tiles.Width+xadj, y*tiles.Height+yadj);
				
                    xtile++;
                }
                ytile++;
                xtile = xt;
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
            set {   filename = value;     }
        }
        public int XWin
        {
            get {   return xwin;    }
            set
            {
                xwin = value;
                if (xwin<0) xwin = 0;
                if (xwin>map.Width*tiles.Width-gfx.ClientSize.Width)
                    xwin = map.Width*tiles.Width-gfx.ClientSize.Width;
            }
        }

        public int YWin
        {
            get {   return ywin;    }
            set
            {
                ywin = value;
                if (ywin < 0) ywin = 0;
                if (ywin > map.Width*tiles.Width - gfx.ClientSize.Width)
                    ywin = map.Width*tiles.Width - gfx.ClientSize.Width;        }
        }

        public Map Map
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

        public void GetTileCoords(ref int x, ref int y, int layer)
        {
            // Possible performance issue: we create a new LayerInstance info every time we call this.
            // Deal with it later.
            LayerInfo li = map.get_LayerInfo(layer);
            x-=(XWin*li.pmulx/li.pdivx);
            y-=(YWin*li.pmuly/li.pdivy);

            x/=tiles.Width;
            y/=tiles.Height;
        }

        public void GetTileCoords(ref int x, ref int y)
        {
            x-=XWin;
            y-=YWin;

            x/=tiles.Width;
            y/=tiles.Height;
        }

        void SaveMap(object o, EventArgs e)
        {

        }

        void OnMouseDown(object o, MouseEventArgs e)
        {
            state.MouseDown(this, e);
            base.OnMouseDown(e);
        }

        void OnMouseUp(object o, MouseEventArgs e)
        {
            state.MouseUp(this, e);
            base.OnMouseUp(e);
        }

        void OnMouseWheel(object o, MouseEventArgs e)
        {
            state.MouseWheel(this, e);
            base.OnMouseWheel(e);
        }

        void OnMouseMove(object o, MouseEventArgs e)
        {
            state.MouseMove(this, e);
            base.OnMouseMove(e);
        }
    }

}