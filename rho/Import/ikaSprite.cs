using System;
using System.Collections;
using System.Collections.Specialized;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using Aries;
using ICSharpCode.SharpZipLib.Zip.Compression.Streams;

namespace rho.Import {

    public class ikaSprite {
        const string SPRITE_VER = "1.2";

        public ikaSprite()
        : this(16, 16) {
        }

        public ikaSprite(int width, int height) {
            HotSpot = Rectangle.FromLTRB(0, 0, 0, 0);
            frames = new ImageArray(width, height);
        }

        public ikaSprite(string fileName)
        : this(new StreamReader(fileName)) { 
        }

        public unsafe ikaSprite(System.IO.TextReader sourceStream) {
            DataNode document = DataNode.ReadDocument(sourceStream);

            DataNode rootNode = document.GetChild("ika-sprite");

            string version = rootNode.GetChild("version").GetString();

            DataNode infoNode = rootNode.GetChild("information");
            title = infoNode.GetChild("title").GetString();
            DataNode metaNode = infoNode.GetChild("meta");
            foreach (DataNode n in metaNode.Children) {
                DataNode node = (DataNode)n.Children[0];
                Metadata.Add(node.Name, node.GetString());
            }

            DataNode framesNode = rootNode.GetChild("frames");

            int frameCount = Convert.ToInt32(framesNode.GetChild("count").GetString());

            DataNode dimNode = framesNode.GetChild("dimensions");
            Size size = new Size(
                Convert.ToInt32(dimNode.GetChild("width").GetString()),
                Convert.ToInt32(dimNode.GetChild("height").GetString())
                );
            
            DataNode hsNode = framesNode.GetChild("hotspot");
            HotSpot = new Rectangle(
                Convert.ToInt32(hsNode.GetChild("x").GetString()),
                Convert.ToInt32(hsNode.GetChild("y").GetString()),
                Convert.ToInt32(hsNode.GetChild("width").GetString()),
                Convert.ToInt32(hsNode.GetChild("height").GetString())
            );
            
            DataNode animNode = rootNode.GetChild("scripts");
            foreach (DataNode child in animNode.GetChildren("script")) {
                string name = child.GetChild("label").GetString();
                string script = child.GetString();
                Scripts.Add(name, script);
            }

            // Read pixel data
            DataNode dataNode = framesNode.GetChild("data");

            string dataFormat = dataNode.GetChild("format").GetString();
            if (dataFormat != "zlib") {
                throw new Exception(string.Format("Invalid data format \"{0}\".  Only \"zlib\" compression is supported.", dataFormat));
            }

            string data64 = dataNode.GetString();
            byte[] cdata = Convert.FromBase64String(data64);

            // shorthand
            int width = size.Width;
            int height = size.Height;

            byte[] pixels = new byte[width * height * frameCount * 4];

            using (MemoryStream compressed = new MemoryStream(cdata))
            using (InflaterInputStream iis = new InflaterInputStream(compressed)) {

                /**
                 * This is silly.
                 * InflaterInputStream doesn't actually read until it's got what you asked for.
                 * It seems to just stop when it feels like a good idea, then returns the number
                 * of bytes read.
                 * 
                 * What the hell.
                 */

                int byteCount = 0;
                while (byteCount < pixels.Length) {
                    byteCount += iis.Read(pixels, byteCount, pixels.Length - byteCount);
                }

            }

            frames = new ImageArray(width, height);

            fixed (byte* p = &pixels[0]) {
                byte* ptr = p;

                for (int i = 0; i < frameCount; i++) {
                    Bitmap bmp = new Bitmap(width, height, PixelFormat.Format32bppArgb);

                    BitmapData bd = bmp.LockBits(
                        new Rectangle(0, 0, width, height),
                        ImageLockMode.WriteOnly,
                        PixelFormat.Format32bppArgb
                        );

                    byte* dest = (byte*)bd.Scan0;
                    byte* src = ptr;
                    for (int k = 0; k < width * height; k++) {
                        // swap red and blue
                        dest[0] = src[2];
                        dest[1] = src[1];
                        dest[2] = src[0];
                        dest[3] = src[3];
                        dest += 4;
                        src += 4;
                    }

                    bmp.UnlockBits(bd);

                    frames.Add(bmp);
                    ptr += width * height * 4;
                }
            }
        }

        public void Save(string fileName) {
            using (TextWriter stream = new System.IO.StreamWriter(fileName)) {
                Save(stream);
            }
        }

        public unsafe void Save(System.IO.TextWriter stream) {
            DataNode rootNode = new DataNode("ika-sprite");
            rootNode.AddChild(new DataNode("version").AddChild(SPRITE_VER));

            DataNode infoNode = new DataNode("information");
            rootNode.AddChild(infoNode);
            infoNode.AddChild(new DataNode("title").AddChild("Untitled")); 
        
            DataNode metaNode = new DataNode("meta");
            infoNode.AddChild(metaNode);
            
            foreach (DictionaryEntry iter in Metadata) {
                metaNode.AddChild(new DataNode((string)iter.Key).AddChild((string)iter.Value));
            }

            rootNode.AddChild(new DataNode("header")
                .AddChild(new DataNode("depth").AddChild("32"))
                );
        

            DataNode scriptNode = new DataNode("scripts");
            rootNode.AddChild(scriptNode);

            foreach (DictionaryEntry iter in Scripts) {
                scriptNode.AddChild(
                    new DataNode("script").AddChild(
                        new DataNode("label").AddChild((string)iter.Key)
                    )
                    .AddChild((string)iter.Value)
                    );
            }

            DataNode frameNode = new DataNode("frames");
            rootNode.AddChild(frameNode);

            frameNode
                .AddChild(new DataNode("count").AddChild(frames.Count))
                .AddChild(new DataNode("dimensions")
                    .AddChild(new DataNode("width").AddChild(Size.Width))
                    .AddChild(new DataNode("height").AddChild(Size.Height))
                )
                .AddChild(new DataNode("hotspot")
                    .AddChild(new DataNode("x").AddChild(HotSpot.X))
                    .AddChild(new DataNode("y").AddChild(HotSpot.Y))
                    .AddChild(new DataNode("width").AddChild(HotSpot.Width))
                    .AddChild(new DataNode("height").AddChild(HotSpot.Height))
                );

            MemoryStream data = new MemoryStream();

            foreach (Bitmap bmp in Frames) {
                BitmapData bd = bmp.LockBits(
                    Rectangle.FromLTRB(0, 0, bmp.Width, bmp.Height), 
                    ImageLockMode.ReadOnly, 
                    PixelFormat.Format32bppArgb
                    );

                int numPixels = bmp.Width * bmp.Height;
                byte* b = (byte*)bd.Scan0;
                for (int i = 0; i < numPixels; i++) {
                    // Swap red and blue
                    data.WriteByte(b[2]);
                    data.WriteByte(b[1]);
                    data.WriteByte(b[0]);
                    data.WriteByte(b[3]);

                    b += 4;
                }

                bmp.UnlockBits(bd);
            }

            MemoryStream cdata = new MemoryStream();
            DeflaterOutputStream dos = new DeflaterOutputStream(cdata);
            dos.Write(data.GetBuffer(), 0, (int)data.Position);

            dos.Finish();

            string cdata64 = Convert.ToBase64String(cdata.GetBuffer(), 0, (int)cdata.Length);
        
            data.Close();
            dos.Close();

            frameNode.AddChild(
                new DataNode("data")
                    .AddChild(new DataNode("format").AddChild("zlib"))
                    .AddChild(cdata64)
            );

            rootNode.Write(stream);
        }
    
        public ImageArray Frames {
            get { return frames; }
        }

        public string Title {
            get {
                return title;
            }
        }

        string title;
        
        public Size Size {
            get {
                return frames.Size;
            }
        }

        public Rectangle HotSpot;
        public readonly StringDictionary Scripts = new StringDictionary();
        public readonly StringDictionary Metadata = new StringDictionary();
        readonly ImageArray frames;
    }
}
