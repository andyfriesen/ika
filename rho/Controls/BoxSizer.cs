using System;
using System.Collections;
using System.Windows.Forms;

namespace rho.Controls {
    public class BoxSizer : Panel {
#if false
        // Broken.
        protected override void OnControlAdded(ControlEventArgs e) {
            proportions.Add(new Pair(e.Control, 0, SizerFlags.Default));
        }

        protected override void OnControlRemoved(ControlEventArgs e) {
            int i = 0;
            foreach (Pair p in proportions) {
                if (p.control == e.Control) {
                    proportions.RemoveAt(i);
                    return;
                } else {
                    i++;
                }
            }
        }
#endif

        public BoxSizer Add(string s) {
            Label l = new Label();
            l.Text = s;
            return Add(l);
        }

        public BoxSizer Add(Control c) {
            return Add(c, 0);
        }

        public BoxSizer Add(Control c, int proportion) {
            return Add(c, proportion, SizerFlags.Default);
        }

        public BoxSizer Add(Control c, SizerFlags flags) {
            return Add(c, 0, flags);
        }

        public BoxSizer Add(Control c, int proportion, SizerFlags flags) {
            proportions.Add(new Pair(c, proportion, flags));
            Controls.Add(c);
            return this;
        }

        /// FIXME: "Pair" is a horrible, horrible name for this thing.
        protected struct Pair {
            public Pair(Control c, int p, SizerFlags f) {
                control = c;
                proportion = p;
                flags = f;
            }

            public Control control;
            public int proportion;
            public SizerFlags flags;
        }

        protected readonly ArrayList proportions = new ArrayList();
    }
}
