using System;
using System.Windows.Forms;
using System.Drawing;
using Import.ika;

namespace rho.MapEditor
{
	/// <summary>
	/// Summary description for TileSetState.
	/// </summary>
	class TileSetState : State
	{
        public override string Name
        {
            get {   return "Tiles"; }
        }

        public override void MouseDown(MapView view,MouseEventArgs e)
        {
            int x=e.X;
            int y=e.Y;
            view.GetTileCoords(ref x,ref y,0);

            view.Map.set_Tile(x,y,0,100);
        }
        
        public override void MouseUp(MapView view,MouseEventArgs e){}
        public override void MouseWheel(MapView view,MouseEventArgs e){}
        public override void MouseMove(MapView view,MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left)!=0)
                MouseDown(view,e);
        }

        public override void KeyDown(MapView view,KeyEventArgs e){}
        public override void KeyUp(MapView view,KeyEventArgs e){}
    }
}
