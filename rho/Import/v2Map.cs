using System;
using System.IO;

namespace Import {
    public class v2Map {
        public static Map Load(string fname) {
            FileStream f=new FileStream(fname,FileMode.Open);
            BinaryReader stream=new BinaryReader(f,System.Text.ASCIIEncoding.ASCII);
			
            Map map=new Map();
						
            char[] buffer;
            buffer=stream.ReadChars(6);	// TODO: make sure the signature is valid
			
            buffer=stream.ReadChars(4);	// skip a few bytes
			
            // Gay,  Convert a C string of arbitrary length to a C# string.
            buffer=stream.ReadChars(60);	map.vspname=new String(buffer,0,Array.IndexOf(buffer,'\0'));
            buffer=stream.ReadChars(60);	map.musicname=new String(buffer,0,Array.IndexOf(buffer,'\0'));
            buffer=stream.ReadChars(20);	map.renderstring=new String(buffer,0,Array.IndexOf(buffer,'\0'));
			
            buffer=stream.ReadChars(55);	// skip a few bytes
	
            int numlayers=stream.ReadByte();
							
            int width=0;
            int height=0;
            // temp placeholders until it's time to instantiate the layer classes
            float[] parx=new float[numlayers];
            float[] pary=new float[numlayers];
            bool[] trans=new bool[numlayers];
			
            for (int i=0; i<numlayers; i++) {
                int mx,my,dx,dy;
                mx=stream.ReadSByte();
                dx=stream.ReadSByte();
                my=stream.ReadSByte();
                dy=stream.ReadSByte();
                parx[i]=1.0f* mx/dx;
                pary[i]=1.0f* my/dy;
				
                width=stream.ReadUInt16();
                height=stream.ReadUInt16();
	
                int hline=stream.ReadByte();
                trans[i]=stream.ReadByte()!=0;
				
                stream.ReadInt16();	// skip 2 bytes
            }
			
            map.Resize(width,height);
			
            for (int i=0; i<numlayers; i++) {
                UInt32 buffersize=stream.ReadUInt32();
                UInt16[] cbuffer=new UInt16[buffersize];
                int[] layerdata=new int[width*height];
				
                // ... RETARDED.  (probably slow too)
                for (int j=0; j<buffersize/2; j++)
                    cbuffer[j]=stream.ReadUInt16();
				
                RLE.Read(layerdata,width*height,cbuffer);
				
                Map.Layer l=map.AddLayer(layerdata);
                l.parx=parx[i];
                l.pary=pary[i];
                //l.trans=trans[i];
            }
			
            stream.Close();
            f.Close();
			
            // other stuff that I don't care about now.
			
            return map;
        }
    }
}
