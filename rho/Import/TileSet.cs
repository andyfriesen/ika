using System.Drawing;
using System.Drawing.Imaging;
using System.Collections;

//! Represents a tileset.

namespace Import
{
	public class Tileset
	{
		ArrayList tiles;		//!< Array of Bitmaps; used for tile images
		int tilex,tiley;		//!< Tile dimensions
		
		public Tileset() : this(16,16)
		{}
		
		public Tileset(int x,int y) 
		{
			tilex=x;
			tiley=y;
			tiles=new ArrayList();		
		}
		
		public Bitmap InsertTile(int idx)
		{
			return InsertTile(new Bitmap(tilex,tiley,PixelFormat.Format32bppArgb),idx);
		}
		
		public Bitmap AppendTile()
		{
			return AppendTile(new Bitmap(tilex,tiley,PixelFormat.Format32bppArgb));
		}
		
		//! Inserts a tile into the tileset and returns it.
		public Bitmap InsertTile(Bitmap b,int idx)
		{
			if (idx<0 || idx>tiles.Count)
				return null;
			
			tiles.Insert(idx,b);
			
			return b;
		}
		
		//! Appends a new tile at the end of the tileset, and returns it.
		public Bitmap AppendTile(Bitmap b)
		{
			tiles.Add(b);
			
			return b;
		}
		
		//! Removes a tile from the tileset, and returns it.
		public Bitmap RemoveTile(int idx)
		{
			if (idx<0 || idx>=tiles.Count)
				return null;
			
			Bitmap b=(Bitmap)tiles[idx];
			tiles.RemoveAt(idx);
			
			return b;
		}
		
		//! Get/set a given tile
		public Bitmap this[int idx]
		{
			get	{	return (Bitmap)tiles[idx];	}
			set	{	tiles[idx]=value;			}
		}
		
		public int Width
		{
			get	{	return tilex;	}
		}
		
		public int Height
		{
			get	{	return tiley;	}
		}
		
		void Save(string fname)
		{
			// TODO: implement this
		}
	}
}
