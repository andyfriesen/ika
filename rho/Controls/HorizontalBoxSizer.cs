
using System;
using System.Collections;
using System.Windows.Forms;

namespace rho.Controls {
    public class HorizontalBoxSizer : BoxSizer {
        protected override void DoLayout() {
            // number of pixels that are not absorbed by non-resizing controls
            int availableWidth = ClientSize.Width;

            // Number of shares
            int sum = 0;

            foreach (Pair p in proportions) {
                sum += p.proportion;
                if (p.proportion == 0) {
                    availableWidth -= p.control.Width;
                }
            }

            int x = 0;
            int newHeight = 0;
            foreach (Pair p in proportions) {
                if (p.proportion != 0) {
                    System.Diagnostics.Debug.Assert(sum > 0);
                    p.control.Width = p.proportion * availableWidth / sum;
                }

                p.control.Location = new System.Drawing.Point(x, p.control.Location.Y);
                x += p.control.Width;
                if (p.control.Bottom > newHeight) {
                    newHeight = p.control.Bottom;
                }

                if ((p.flags & SizerFlags.Expand) != SizerFlags.Default) {
                    p.control.Height = ClientSize.Height;
                }
            }

            Height = newHeight;
        }

    }
}
