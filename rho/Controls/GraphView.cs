/*
 * This is a bit hackish, as the Reraw event gives you a rect,
 * but expects you to draw what would be in that rect, but at
 * (0, 0)
 * 
 * Not really ideal.
 */

using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;

namespace rho.Controls {

    public class GraphView : ScrollWindow {
        const int buffersize=256;
        Bitmap buffer;
        Graphics graphics;
	
        public GraphView() : base() {
            buffer=new Bitmap(buffersize, buffersize, PixelFormat.Format32bppArgb);
            graphics=Graphics.FromImage(buffer);
        }
	
        protected override void OnPaintBackground(PaintEventArgs e) {
            // ;D
        }

        protected override void OnPaint(PaintEventArgs e) {
            base.OnPaint(e);

            if (Redraw == null)
                return;

            // disable alpha blending; we're doing that offscreen
            e.Graphics.CompositingMode=CompositingMode.SourceCopy;
	
            Rectangle r=e.ClipRectangle;
		
            int startx=e.ClipRectangle.X/buffersize;
            int endx=e.ClipRectangle.Right/buffersize+1;
            int starty=e.ClipRectangle.Y/buffersize;
            int endy=e.ClipRectangle.Bottom/buffersize+1;
		
            r.Width=buffersize;
            r.Height=buffersize;
            for (int y=starty; y<endy; y++) {
                for (int x=startx; x<endx; x++) {
                    graphics.Clear(Color.Black);

                    Redraw(this, new PaintEventArgs(graphics, r));
                    e.Graphics.DrawImage(buffer, r.X, r.Y);
                    Console.WriteLine("!");
				
                    r.X+=buffersize;
                }
                r.X=startx*buffersize;
                r.Y+=buffersize;
            }
        }
	
        public event PaintEventHandler Redraw;
    }

}