using System;
using System.Drawing;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI;
using rho.Documents;

namespace rho.SpriteEditor {
    /// <summary>
    /// FIXME: scrollPanel breaks all this.
    /// </summary>
    public class SpriteEditor : DockContent, IDocumentView {
        public SpriteEditor(SpriteDocument doc) {
            document = doc;

            detailPanel = new SpriteDetailPanel();
            detailPanel.Dock = DockStyle.Left;
            Controls.Add(detailPanel);

            framePanel = new FramePanel(doc);

            scrollPanel = new Panel();
            scrollPanel.AutoScroll = true;
            scrollPanel.Controls.Add(framePanel);
            Controls.Add(scrollPanel);

            framePanel.FrameSelected += new FrameEventHandler(FrameSelected);

            MinimumSize = new Size(detailPanel.Width, MinimumSize.Height);
        }

        public string FileName {
            get { return fileName; }
            set { fileName = value; }
        }

        public void Save() {
        }

        public void Save(string filename) {
        }

        void FrameSelected(FrameEventArgs e) {
            MessageBox.Show(this, string.Format("Frame {0} selected", e.Index));
        }

        void ResizeFramePanel() {
            int frameWidth = (document.Size.Width * 256 / zoom) + pad;
            int frameHeight = (document.Size.Height * 256 / zoom) + pad;
            
            framePanel.Width = scrollPanel.ClientSize.Width;
            int cols = framePanel.Width / frameWidth;

            if (cols > 0) {
                int rows = (document.Frames.Count + cols - 1) / cols;
                if (rows == 0) {
                    rows = 1;
                }

                framePanel.Height = rows * frameHeight;
            }
        }

        void DrawFrames(object sender, PaintEventArgs e) {
        }

        protected override void OnLayout(LayoutEventArgs e) {
            base.OnLayout(e);
            if (scrollPanel != null && detailPanel != null) {
                scrollPanel.Location = new System.Drawing.Point(detailPanel.Right, 0);
                scrollPanel.Size = new Size(Width - detailPanel.Width, Height);
                ResizeFramePanel();

                Console.Write("{0}    {1}", scrollPanel.Location, scrollPanel.Size);
            }
        }

        // zoom factor is 256::zoom
        int zoom = 128;
        // pixels of padding between each frame
        int pad = 1;

        string fileName;
        SpriteDocument document;
        readonly ScrollableControl scrollPanel; // contains the framePanel
        readonly SpriteDetailPanel detailPanel;
        readonly FramePanel framePanel;
    }
}
