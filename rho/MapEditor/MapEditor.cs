using System;
using Import.ika;

namespace rho.MapEditor
{
	/// <summary>
	/// Facade Pattern.  GoF.
	/// 
	/// The MapEditor is the editor treated as a whole.  Provides a high level
	/// abstraction for editing the map, and maniuplating the UI elements.
	/// </summary>
	public class MapEditor
	{
        MapView view;
        Map map;
        TileSet tileset;

		public MapEditor()
		{
			//
			// TODO: Add constructor logic here
			//
		}
	}
}
