using System;

namespace rho.SpriteEditor {
    public class FrameEventArgs : EventArgs {
        public FrameEventArgs(int idx) : base() {
            index = idx;
        }

        public int Index {
            get { return index; }
        }

        int index;
    }

    public delegate void FrameEventHandler(FrameEventArgs e);
}
