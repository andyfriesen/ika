using System;
using System.Windows.Forms;
using Import.ika;

namespace rho.MapEditor
{
	/// <summary>
	/// Summary description for TileSetState.
	/// </summary>
	class TileSetState : IState
	{
        MapView mapview;
        Map map;

		public TileSetState(MapView m)
		{
            mapview=m;
            map=m.Map;
		}

        public void MouseDown(MouseEventArgs e){}
        public void MouseUp(MouseEventArgs e){}
        public void MouseWheel(MouseEventArgs e){}
        public void MouseMove(MouseEventArgs e){}

        public void KeyDown(KeyEventArgs e){}
        public void KeyUp(KeyEventArgs e){}
    }
}
