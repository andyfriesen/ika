
using System;
using System.Collections;
using System.Windows.Forms;

namespace rho.Controls {
    public class VerticalBoxSizer : BoxSizer {
        protected override void DoLayout() {
            // number of pixels that are not absorbed by non-resizing controls
            int availableHeight = ClientSize.Height;

            // Number of shares
            int sum = 0;

            foreach (Pair p in proportions) {
                sum += p.proportion;
                if (p.proportion == 0) {
                    availableHeight -= p.control.Height;
                }
            }

            int y = 0;
            int newWidth = 0;
            foreach (Pair p in proportions) {
                if (p.proportion != 0) {
                    System.Diagnostics.Debug.Assert(sum > 0);
                    p.control.Height = p.proportion * availableHeight / sum;
                }

                p.control.Location = new System.Drawing.Point(p.control.Location.X, y);
                y += p.control.Height;
                if (p.control.Right > newWidth) {
                    newWidth = p.control.Right;
                }

                if ((p.flags & SizerFlags.Expand) != SizerFlags.Default) {
                    p.control.Width = ClientSize.Width;
                }
            }

            Width = newWidth;
            Height = y;
        }

    }
}
