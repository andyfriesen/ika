using System;
using System.Collections;
using System.Windows.Forms;

namespace rho.Controls {
    /// <summary>
    /// A simple wx-inspired sizer control.
    /// BoxSizer arranges its child controls somehow or other.
    /// <seealso cref="HorizontalBoxSizer"/>
    /// <seealso cref="VerticalBoxSizer"/>
    /// </summary>
    public abstract class BoxSizer : Panel {
#if true
        // Seems to work
        protected override void OnControlAdded(ControlEventArgs e) {
            base.OnControlAdded(e);
            proportions.Add(new Pair(e.Control, 0, SizerFlags.Default));
        }

        protected override void OnControlRemoved(ControlEventArgs e) {
            int i = 0;
            foreach (Pair p in proportions) {
                if (p.control == e.Control) {
                    
                    proportions.RemoveAt(i);
                    base.OnControlRemoved(e);
                    return;
                } else {
                    i++;
                }
            }
            System.Diagnostics.Debug.Fail("Internal error in BoxSizer: control to be removed is not present O_O");
        }
#endif

        /// <summary>
        /// Creates a label and adds it to the sizer.
        /// </summary>
        /// <param name="s">String of text to be displayed on the label</param>
        /// <returns>Self reference</returns>
        public BoxSizer Add(string s) {
            return Add(s, 0, SizerFlags.Default);
        }

        /// <summary>
        /// Creates a label and adds it to the sizer.
        /// </summary>
        /// <param name="s">String of text to be displayed on the label</param>
        /// <param name="proportion">
        /// Proportion... thing.  Used to calculate how much of
        /// the available space ought to be absorbed by the control.
        /// If 0, the control will not be resized.
        /// </param>
        /// <returns>Self reference</returns>
        public BoxSizer Add(string s, int proportion) {
            return Add(s, proportion, SizerFlags.Default);
        }

        /// <summary>
        /// Creates a label and adds it to the sizer.
        /// </summary>
        /// <param name="s">String of text to be displayed on the label</param>
        /// <param name="flags"><see cref="SizerFlags"/></param>
        /// <returns>Self reference</returns>
        public BoxSizer Add(string s, SizerFlags flags) {
            return Add(s, 0, flags);
        }

        /// <summary>
        /// Creates a label and adds it to the sizer.
        /// </summary>
        /// <param name="s">String of text to be displayed on the label</param>
        /// <param name="proportion">
        /// Proportion... thing.  Used to calculate how much of
        /// the available space ought to be absorbed by the control.
        /// If 0, the control will not be resized.
        /// </param>
        /// <param name="flags"><see cref="SizerFlags"/></param>
        /// <returns>Self reference</returns>
        public BoxSizer Add(string s, int proportion, SizerFlags flags) {
            Label l = new Label();
            l.Text = s;
            return Add(l, proportion, flags);
        }

        /// <summary>
        /// Adds a control to the sizer.
        /// </summary>
        /// <returns>Self reference</returns>
        public BoxSizer Add(Control c) {
            return Add(c, 0);
        }

        /// <summary>
        /// Creates a label and adds it to the sizer.
        /// </summary>
        /// <param name="c">Control to add</param>
        /// <param name="proportion">
        /// Proportion... thing.  Used to calculate how much of
        /// the available space ought to be absorbed by the control.
        /// If 0, the control will not be resized.
        /// </param>
        /// <returns>Self reference</returns>
        public BoxSizer Add(Control c, int proportion) {
            return Add(c, proportion, SizerFlags.Default);
        }

        /// <summary>
        /// Creates a label and adds it to the sizer.
        /// </summary>
        /// <param name="c">Control to add</param>
        /// <param name="proportion">
        /// Proportion... thing.  Used to calculate how much of
        /// the available space ought to be absorbed by the control.
        /// If 0, the control will not be resized.
        /// </param>
        /// <param name="flags"><see cref="SizerFlags"/></param>
        /// <returns>Self reference</returns>
        public BoxSizer Add(Control c, SizerFlags flags) {
            return Add(c, 0, flags);
        }

        /// <summary>
        /// Creates a label and adds it to the sizer.
        /// </summary>
        /// <param name="c">Control to add</param>
        /// <param name="proportion">
        /// Proportion... thing.  Used to calculate how much of
        /// the available space ought to be absorbed by the control.
        /// If 0, the control will not be resized.
        /// </param>
        /// <param name="flags"><see cref="SizerFlags"/></param>
        /// <returns>Self reference</returns>
        public BoxSizer Add(Control c, int proportion, SizerFlags flags) {
            Controls.Add(c);
            
            Pair p = (Pair)proportions[proportions.Count - 1];

            // tweak p so that things are right
            p.proportion = proportion;
            p.flags = flags;

            return this;
        }

        protected override void OnLayout(LayoutEventArgs levent) {
            base.OnLayout(levent);

            DoLayout();
        }

        protected abstract void DoLayout();

        /// FIXME: "Pair" is a horrible, horrible name for this thing.
        protected class Pair {
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
