using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;

namespace rho.Import {
    public class v2Tileset {
        unsafe static void CreateTileImages(Tileset tileset,int numtiles,byte[] pixeldata,byte[] pal) {		
            int idx=0;

            for (int curtile=0; curtile<numtiles; curtile++) {	
                Bitmap bitmap=new Bitmap(16,16,PixelFormat.Format32bppArgb);
				
                BitmapData data=bitmap.LockBits(
                    new Rectangle(0,0,16,16),
                    ImageLockMode.WriteOnly,
                    PixelFormat.Format32bppArgb
                    );
				
                Int32* temp=(Int32*)data.Scan0.ToPointer();
				
                // First, convert data to 32bpp ARGB
                for (int i=0; i<16*16; i++) {
                    byte c=pixeldata[idx++];
					
                    if (c!=0)
                        temp[i]=
                            (255 << 24) |
                            pal[c*3] << 18 |
                            pal[c*3+1] << 10 |
                            pal[c*3+2] << 2;
                    else
                        temp[i]=0;
						
                }
				
                bitmap.UnlockBits(data);
				
                // Second, add it to the list.
                tileset.AppendTile(bitmap);
            }
        }
		
        public static Tileset Load(string fname) {
            FileStream f=new FileStream(fname,FileMode.Open);
            BinaryReader stream=new BinaryReader(f);
			
            Tileset t=new Tileset();
	
            UInt16 version=stream.ReadUInt16();
            if (version!=3)
                throw new Exception(String.Format("v2Tileset: {0} is not a valid VERGE VSP",fname));
				
            byte[] pal=new byte[768];
            stream.Read(pal,0,768);	// palette
			
            UInt16 numtiles=stream.ReadUInt16();	
			
            int bufsize=stream.ReadInt32();
            byte[] buffer=new byte[bufsize];
            stream.Read(buffer,0,bufsize);
			
            byte[] pixeldata=new byte[numtiles*16*16];
			
            RLE.Read(pixeldata,numtiles*16*16,buffer);
			
            CreateTileImages(t,numtiles,pixeldata,pal);
			
            stream.Close();
            f.Close();
			
            return t;
        }
    }
}
