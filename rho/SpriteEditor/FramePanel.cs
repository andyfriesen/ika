using System;
using System.Windows.Forms;
using System.Drawing;

namespace rho.SpriteEditor {
    public class FramePanel : Panel {
        public FramePanel(SpriteDocument doc) {
            document = doc;
        }

        public int Zoom {
            get { return zoom; }
            set { zoom = value; }
        }

        public int Pad {
            get { return pad; }
            set { pad = value; }
        }

        public event FrameEventHandler FrameSelected;
        public event FrameEventHandler FrameRightClicked;

        protected override void OnPaintBackground(PaintEventArgs pevent) {
            // Do nothing.
        }

        protected override void OnPaint(PaintEventArgs e) {
            base.OnPaint(e);

            Size frameSize = GetScaledFrameSize();

            using (Graphics gfx = Graphics.FromImage(backBuffer)) {
                gfx.SetClip(e.ClipRectangle);
                gfx.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
                gfx.Clear(Color.Black);

                int x = 0;
                int y = 0;

                foreach (Bitmap frame in document.Frames) {
                    gfx.DrawImage(frame, x, y, frameSize.Width, frameSize.Height);
                    x += frameSize.Width + pad;
                    if (x + frameSize.Width + pad >= Width) {
                        x = 0;
                        y += frameSize.Height + pad;
                    }
                }

                DrawSelection(gfx);
            }

            e.Graphics.DrawImageUnscaled(backBuffer, 0, 0);
        }

        protected override void OnResize(EventArgs e) {
            if (backBuffer == null) {
                backBuffer = new Bitmap(Width, Height);
            } else if (Width > backBuffer.Width || Height > backBuffer.Height) {
                if (backBuffer != null) {
                    backBuffer.Dispose();
                }
                backBuffer = new Bitmap(Width, Height);
            }

            Invalidate();
        }


        protected override void OnClick(EventArgs e) {
            base.OnClick(e);
            
            Point cursor = PointToClient(Cursor.Position);
            int index = GetFrameAtPos(cursor);
            if (index == -1) {
                return;
            }

            OnFrameSelected(new FrameEventArgs(index));
        }

        protected override void OnMouseDown(MouseEventArgs e) {
            if (e.Button == MouseButtons.Right) {
                OnRightClick(e);
            } else {
                base.OnMouseDown(e);
            }
        }

        protected virtual void OnRightClick(MouseEventArgs e) {
            Point cursor = PointToClient(Cursor.Position);
            int index = GetFrameAtPos(cursor);
            OnFrameRightClicked(new FrameEventArgs(index));
        }

        protected virtual void OnFrameSelected(FrameEventArgs e) {
            selectedFrame = e.Index;

            if (FrameSelected != null) {
                FrameSelected(e);
            }

            Refresh();
        }

        protected virtual void OnFrameRightClicked(FrameEventArgs e) {
            if (FrameRightClicked != null) {
                FrameRightClicked(e);
            }
        }

        void DrawSelection(Graphics g) {
            Point framePos = GetFramePos(selectedFrame);
            Size frameSize = GetScaledFrameSize();
            Rectangle rect = new Rectangle(framePos, frameSize);
            using (Pen pen = new Pen(Color.White)) {
                g.DrawRectangle(pen, framePos.X, framePos.Y, frameSize.Width, frameSize.Height);
            }
        }

        /// <returns>
        /// The preferred size for the control, given its present width.
        /// </returns>
        public Size GetPreferredSize() {
            return new Size(0,0);
        }

        /// <summary>Returns the sprite frame under the point given.</summary>
        int GetFrameAtPos(Point p) {
            Size frameSize = GetScaledFrameSize();
            frameSize.Width += pad;
            frameSize.Height += pad;
            
            int rowSize = GetRowSize();

            int col = p.X / frameSize.Width;
            int row = p.Y / frameSize.Height;

            int index = row * rowSize + col;

            if (col >= rowSize || index >= document.Frames.Count) {
                return -1;
            } else {
                return index;
            }
        }

        /// <summary>Returns the position of the frame, given its index.</summary>
        Point GetFramePos(int index) {
            Point result = new Point(-1, -1);

            if (index < 0 || index >= document.Frames.Count) {
                return result;
            }

            Size frameSize = GetScaledFrameSize();
            frameSize.Width += pad;
            frameSize.Height += pad;
            
            int rowSize = GetRowSize();
            result.Y = (index / rowSize) * frameSize.Height;
            result.X = (index % rowSize) * frameSize.Width;
            return result;
        }

        /// <returns>Returns the size of a frame after zooming and the like.</returns>
        Size GetScaledFrameSize() {
            return new Size(
                (document.Size.Width * 256 / zoom),
                (document.Size.Height * 256 / zoom)
            );
        }

        /// <returns>Returns the number of frames per row.</returns>
        int GetRowSize() {
            Size frameSize = GetScaledFrameSize();
            frameSize.Width += pad;
            frameSize.Height += pad;
            
            int rowSize = Width / frameSize.Width;
            return rowSize > 1 ? rowSize : 1;
        }

        int GetNumRows() {
            return GetRowSize() / document.Frames.Count;
        }

        Bitmap backBuffer;
        int zoom = 128;
        int pad = 1;
        int selectedFrame = 0;
        SpriteDocument document;
    }
}
