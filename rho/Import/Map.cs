using System;
using System.Collections;

namespace Import
{
	//! Represents a map
	public class Map
	{
		//! Represents one map layer.
		public class Layer
		{
			int[,] tiles;
			
			public float parx,pary;
			
			internal Layer(int width,int height)
			{
				tiles=new int[width,height];
			}
			
			public int Width
			{
				get	{	return tiles.GetUpperBound(0);	}
			}
			
			public int Height
			{
				get	{	return tiles.GetUpperBound(1);	}
			}
		
			// TODO: range checking
			public int this[int x,int y]
			{
				get	{	return tiles[x,y];	}
				set	{	tiles[x,y]=value;	}
			}
			
			public void Resize(int x,int y)
			{
				// TODO
			}
		}
		
		ArrayList layers;
		Layer obs;	// obstruction map
		Layer zone; // zone information
		int xsize,ysize;
		
		public string vspname="";
        public string musicname="";
        public string renderstring="";
		
		public Map(int x,int y)
		{
			xsize=x;
			ysize=y;
			layers=new ArrayList();
			
			obs=new Layer(xsize,ysize);
			zone=new Layer(xsize,ysize);
		}
		
		public Map() : this(100,100)
		{}
		
		//! Creates a new map layer, and returns it
		public Layer AddLayer()
		{
			Layer l=new Layer(xsize,ysize);
			layers.Add(l);
			
			return l;
		}
		
		//! Creates a new map layer, with the given tile data.
		internal Layer AddLayer(int[] data)
		{
			Layer l=new Layer(xsize,ysize);

			int i=0;
			for (int y=0; y<ysize; y++)
				for (int x=0; x<xsize; x++)
				{
					l[x,y]=data[i++];
				}
				
			layers.Add(l);
			return l;
		}
		
		//! Removes a map layer, and returns it.
		public Layer RemoveLayer(int idx)
		{
			if (idx<0 || idx>=layers.Count)
				return null;
			
			Layer l=(Layer)layers[idx];
			layers.RemoveAt(idx);
			
			return l;
		}

		public void Resize(int x,int y)
		{
			foreach (Layer l in layers)
				l.Resize(x,y);
			obs.Resize(x,y);
			zone.Resize(x,y);
			
			xsize=x;
			ysize=y;
		}

		//! Returns the specified layer.
		// TODO Range check
		public Layer this[int idx]
		{
			get	{	return (Layer)layers[idx];	}
		}
		
		public int NumLayers	{	get	{	return layers.Count;	}	}
		
		public Layer Obs
		{	get	{	return obs;	}	}
		
		public Layer Zone
		{	get	{	return zone;}	}
		
		public int Width
		{	get	{	return xsize;	}	}
		
		public int Height		
		{	get	{	return ysize;	}	}
		
	}
}
