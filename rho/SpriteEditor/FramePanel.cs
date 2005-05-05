using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Diagnostics;

namespace rho.SpriteEditor {
    public class FramePanel : UserControl {
        public FramePanel(SpriteDocument doc) {
            document = doc;
            backBuffer = new Bitmap(Size.Width, Size.Height);
            document.Changed += new rho.Documents.ChangeEventHandler(OnSpriteChanged);
        }

        public int Zoom {
            get { 
                return zoom; 
            }
            set { 
                zoom = value; 
                Refresh();
            }
        }

        public int Pad {
            get { 
                return pad;
            }
            set { 
                pad = value;
                Refresh();
            }
        }

        /// <summary>
        /// Returns the index of the selected frame.
        /// If no frame is selected at all, the value is -1
        /// </summary>
        public int SelectedFrame {
            get { 
                Debug.Assert(
                    -1 <= selectedFrame && 
                    selectedFrame < document.Frames.Count
                    );
                return selectedFrame;
            }
            set {
                if (value < 0) value = 0;
                if (value >= document.Frames.Count) value = document.Frames.Count - 1;
                selectedFrame = value;
                Refresh();
            }
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
                int w = Width > backBuffer.Width ? Width : backBuffer.Width;
                int h = Height > backBuffer.Height ? Height : backBuffer.Height;

                backBuffer.Dispose();
                backBuffer = new Bitmap(w, h);
            }

            Invalidate();

            base.OnResize(e);
        }

        protected override void OnLayout(LayoutEventArgs levent) {
            Console.WriteLine("{0},{1}  {2},{3}", Location.X, Location.Y, Width, Height);
            int frameWidth = (document.Size.Width * 256 / zoom) + pad;
            int frameHeight = (document.Size.Height * 256 / zoom) + pad;
            
            // cols = max(1, Width / frameWidth); // :(
            int cols = Width / frameWidth;
            if (cols < 1) {
                cols = 1;
            }

            int rows = (document.Frames.Count + cols - 1) / cols;
            if (rows < 1) {
                rows = 1;
            }

            Height = rows * frameHeight;

            base.OnLayout (levent);
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

        protected override bool IsInputKey(Keys keyData) {
            Keys[] keys = {
                Keys.Up, Keys.Down, Keys.Left, Keys.Right, Keys.PageUp, Keys.PageDown, Keys.Home, Keys.End
            };
            foreach (Keys k in keys) {
                if (keyData == k) {
                    return true;
                }
            }

            return base.IsInputKey (keyData);
        }

        protected override void OnKeyDown(KeyEventArgs e) {
            //Console.WriteLine(e.KeyCode);
            switch (e.KeyCode) {
                case Keys.Left: {
                    SelectedFrame -= 1;
                    break;
                }
                case Keys.Right: {
                    SelectedFrame += 1;
                    break;
                }
                case Keys.Up: {
                    SelectedFrame -= GetRowSize();
                    break;
                }
                case Keys.Down: {
                    SelectedFrame += GetRowSize();
                    break;
                }

#if false
                // Won't work because the FramePanel has no idea how big one page is.
                case Keys.PageUp: {
                    SelectedFrame -= GetRowSize() * GetNumRows();
                    break;
                }
                case Keys.PageDown: {
                    SelectedFrame += GetRowSize() * GetNumRows();
                    break;
                }
#endif

                case Keys.Home: {
                    SelectedFrame = 0;
                    break;
                }
                case Keys.End: {
                    SelectedFrame = document.Frames.Count - 1;
                    break;
                }
                default: {
                    base.OnKeyDown (e);
                    return;
                }
            }

            OnFrameSelected(new FrameEventArgs(SelectedFrame));
            e.Handled = true;
        }

        protected override void OnGotFocus(EventArgs e) {
            base.OnGotFocus (e);
            Refresh();
        }

        protected override void OnLostFocus(EventArgs e) {
            base.OnLostFocus (e);
            Refresh();
        }


        protected virtual void OnRightClick(MouseEventArgs e) {
            Point cursor = PointToClient(Cursor.Position);
            int index = GetFrameAtPos(cursor);
            OnFrameRightClicked(new FrameEventArgs(index));
        }

        protected virtual void OnFrameSelected(FrameEventArgs e) {
            if (FrameSelected != null) {
                FrameSelected(e);
            }
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
            Pen pen = null;

            if (Focused) {
                pen = new Pen(Color.White);
            } else {
                pen = new Pen(new HatchBrush(HatchStyle.Percent50, Color.White, Color.Black));
            }
            
            try {
                g.DrawRectangle(pen, framePos.X, framePos.Y, frameSize.Width, frameSize.Height);
            } finally {
                pen.Dispose();
            }
        }

        void OnSpriteChanged() {
            Refresh();
        }

        /// <returns>
        /// The preferred size for the control, given its present width.
        /// </returns>
        public Size GetPreferredSize() {
            return new Size(0,0);
        }

        /// <summary>Returns the sprite frame under the point given.</summary>
        public int GetFrameAtPos(Point p) {
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
        public Point GetFramePos(int index) {
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
        public Size GetScaledFrameSize() {
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
            int s = GetRowSize();
            return (document.Frames.Count + s - 1) / s; // round up
        }

        Bitmap backBuffer;
        int zoom = 128;
        int pad = 1;
        int selectedFrame = 0;
        SpriteDocument document;
    }
}
