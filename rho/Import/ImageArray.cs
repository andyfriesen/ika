using System;
using System.Collections;
using System.Drawing;

namespace rho.Import {
    /// <summary>
    /// An array of same-sized images.
    /// </summary>
    public class ImageArray : ArrayList {
        public ImageArray(int width, int height) {
            this.width = width;
            this.height = height;
        }

        public void Add(Bitmap bmp) {
            TestSize(bmp);
            base.Add(bmp);
        }

        public bool Contains(Bitmap bmp) {
            return base.Contains(bmp);
        }

        public int IndexOf(Bitmap bmp) {
            return base.IndexOf(bmp);
        }

        public void Insert(int index, Bitmap bmp) {
            base.Insert(index, bmp);
        }

        public void Remove(Bitmap bmp) {
            base.Remove(bmp);
        }

        public new Bitmap this[int index] {
            get {
                return (Bitmap)base[index];
            }

            set {
                TestSize(value);
                base[index] = value;
            }
        }

        public void Resize(int newWidth, int newHeight) {
            if (newWidth == width && newHeight == height) {
                return; // bleh
            }

            width = newWidth;
            height = newHeight;

            Size newSize = new Size(newWidth, newHeight);
            for (int i = 0; i < Count; i++) {
                this[i] = new Bitmap(this[i], newSize);
            }
        }

        public int Width {
            get { return width; }
        }

        public int Height {
            get { return height; }
        }

        public Size Size {
            get { 
                return new Size(width, height);
            }
        }

        void TestSize(Bitmap bmp) {
            if (bmp.Width != width || bmp.Height != height) {
                throw new Exception(
                    string.Format("Cannot add image of size {0}x{0} to ImageArray of size {2}x{3}",
                        bmp.Width, bmp.Height, width, height
                    )
               );
            }
        }

        int width;
        int height;
    }
}
