using System;
using System.Windows.Forms;

namespace rho.SpriteEditor {
    public class SpriteDetailPanel : Panel {
        private static Label newLabel(string t) {
            Label l = new Label();
            l.Text = t;
            l.Width = l.PreferredWidth;
            return l;
        }

        public SpriteDetailPanel() {
            Controls.AddRange(new Control[] {
                    xLabel,
                    editHotX,
                    yLabel,
                    editHotY,
                    widthLabel,
                    editHotWidth,
                    heightLabel,
                    editHotHeight,
                    metaGrid,
                    animGrid,
                }
            );
            OnLayout(null);
            ClientSize = new System.Drawing.Size(animGrid.Right, animGrid.Bottom);
        }

        protected override void OnLayout(LayoutEventArgs levent) {
            // UGH
            if (levent != null) {
                base.OnLayout(levent);
            }
            int w = heightLabel.PreferredWidth;
            int h = editHotX.Height;

            int x = 0;
            int y = 0;
            int yinc = h + 3;
            xLabel.Left = x;
            xLabel.Top = y;
            xLabel.Width = w;
            editHotX.Left = x + w;
            editHotX.Top = y;
            
            y += yinc;
            yLabel.Left = x;
            yLabel.Top = y;
            yLabel.Width = w;
            editHotY.Left = x + w;
            editHotY.Top = y;

            y += yinc;
            widthLabel.Left = x;
            widthLabel.Top = y;
            widthLabel.Width = w;
            editHotWidth.Left = x + w;
            editHotWidth.Top = y;

            y += yinc;
            heightLabel.Left = x;
            heightLabel.Top = y;
            heightLabel.Width = w;
            editHotHeight.Left = x + w;
            editHotHeight.Top = y;

            metaGrid.Dock = DockStyle.Bottom;
            metaGrid.CaptionText = "Metadata";

            animGrid.Dock = DockStyle.Bottom;
            animGrid.CaptionText = "Animation scripts";
        }

        readonly Label xLabel = newLabel("X");
        readonly Label yLabel = newLabel("Y");
        readonly Label widthLabel = newLabel("Width");
        readonly Label heightLabel = newLabel("Height");
        readonly TextBox editHotX = new TextBox();
        readonly TextBox editHotY = new TextBox();
        readonly TextBox editHotWidth = new TextBox();
        readonly TextBox editHotHeight = new TextBox();
        readonly DataGrid animGrid = new DataGrid();
        readonly DataGrid metaGrid = new DataGrid();
    }
}
