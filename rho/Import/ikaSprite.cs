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
        public ikaSprite() {
            Size.Width = Size.Height = 0;
            HotSpot = Rectangle.FromLTRB(0, 0, 0, 0);
        }

        public ikaSprite(string fileName)
        : this(new StreamReader(fileName)) { 
        }

        public unsafe ikaSprite(System.IO.TextReader sourceStream) {
            DataNode document = DataNode.ReadDocument(sourceStream);

            DataNode rootNode = document.GetChild("ika-sprite");

            DataNode infoNode = rootNode.GetChild("information");
            title = infoNode.GetChild("title").GetString();
            DataNode metaNode = infoNode.GetChild("meta");
            foreach (DataNode n in metaNode.Children) {
                DataNode node = (DataNode)n.Children[0];
                MetaData.Add(node.Name, node.GetString());
            }

            DataNode framesNode = rootNode.GetChild("frames");

            int frameCount = Convert.ToInt32(framesNode.GetChild("count").GetString());

            DataNode dimNode = framesNode.GetChild("dimensions");
            Size = new Size(
                Convert.ToInt32(dimNode.GetChild("width").GetString()),
                Convert.ToInt32(dimNode.GetChild("height").GetString())
            );
            
            DataNode hsNode = framesNode.GetChild("hotspot");
            HotSpot = Rectangle.FromLTRB(
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
            int width = Size.Width;
            int height = Size.Height;

            MemoryStream compressed = new MemoryStream(cdata);
            InflaterInputStream iis = new InflaterInputStream(compressed);
            byte[] pixels = new byte[width * height * frameCount * 4];
            iis.Read(pixels, 0, pixels.Length);
            iis.Close();
            compressed.Close();

            frames.Resize(width, height);

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

        public IList Frames {
            get { return frames; }
        }

        public string Title {
            get {
                return title;
            }
        }

        string title;
        public Size Size;
        public Rectangle HotSpot;
        public readonly StringDictionary Scripts = new StringDictionary();
        public readonly StringDictionary MetaData = new StringDictionary();
        readonly ImageArray frames = new ImageArray(16, 16);
    }
}
