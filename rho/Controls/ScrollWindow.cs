/*
 * I wanted a simple control with a pair of scrollbars on it.  It seems that winforms lacks such a simple
 * facility. :x
 *
 * So I had to write my own.  Joy.  This is kinda gay in that it goes right to the core;
 * it directly tweaks the control through raw win32.
 */

using System;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace rho.Controls {

    public class ScrollWindow : Control {
        [Flags]
        enum SetScrollFlags {
            Range           = 0x0001,
            Page            = 0x0002,
            Pos             = 0x0004,
            DisableNoScroll = 0x0008,
            TrackPos        = 0x0010,
            all             = Range | Page | Pos | DisableNoScroll | TrackPos
        }

        [StructLayout(LayoutKind.Sequential)]
        class ScrollInfo {
            public uint size;
            public SetScrollFlags flags;
            public int min;
            public int max;
            public uint page;
            public int pos;
            public int trackpos;

            public ScrollInfo() {
                size = page = 0;
                min = max = pos = trackpos = 0;
                flags = 0;
            }
        }

        [DllImport("user32")]
        static extern int GetScrollInfo(IntPtr hwnd, int bar, [MarshalAs(UnmanagedType.LPStruct)] ScrollInfo si);
        [DllImport("user32")]
        static extern int SetScrollInfo(IntPtr hwnd, int bar, [MarshalAs(UnmanagedType.LPStruct)] ScrollInfo si, bool redraw);

        // win32 constants.
        const int WS_VSCROLL      =0x00200000;
        const int WS_HSCROLL      =0x00100000;
        const int WM_VSCROLL      =277;
        const int WM_HSCROLL      =276;
        const int SB_LINEUP       =0;
        const int SB_LINEDOWN     =1;
        const int SB_LINELEFT     =0;
        const int SB_LINERIGHT    =1;
        const int SB_PAGEUP       =2;
        const int SB_PAGEDOWN     =3;
        const int SB_PAGELEFT     =2;
        const int SB_PAGERIGHT    =3;
        const int SB_THUMBPOSITION        =4;
        const int SB_THUMBTRACK   =5;
        const int SB_ENDSCROLL    =8;
        const int SB_LEFT         =6;
        const int SB_RIGHT        =7;
        const int SB_BOTTOM       =7;
        const int SB_TOP          =6;
        const int SB_HORIZ        =0;
        const int SB_VERT         =1;

        ScrollInfo si = new ScrollInfo();  // just put here so we don't create new ones all the time.
        Size scrollsize;
        Size pagesize;

        public ScrollWindow() : base() {
            ScrollSize = new Size(100, 100); // default size to init the scrollsize member
            PageSize = new Size(10, 10);
        }

        protected override CreateParams CreateParams {
            get {
                CreateParams cp = base.CreateParams;
                cp.Style |= WS_HSCROLL | WS_VSCROLL;
                return cp;
            }
        }

        protected override void WndProc(ref Message msg) {
            // Intercept and override the scroll messages.
            if (msg.Msg == WM_VSCROLL) {
                int position = (msg.WParam.ToInt32() >> 16);
                ScrollEventArgs sea=new ScrollEventArgs(GetEventType(msg.WParam), position);

                DoVScroll(ref position, sea);
                sea.NewValue = position;

                OnVScroll(this, sea);
                return;
            }
            else if (msg.Msg == WM_HSCROLL) {
                int position = (msg.WParam.ToInt32() >> 16);
                ScrollEventArgs sea = new ScrollEventArgs(GetEventType(msg.WParam), position);

                DoHScroll(ref position, sea);
                sea.NewValue = position;

                OnHScroll(this, new ScrollEventArgs(GetEventType(msg.WParam), position));
                return;
            }

            // process all other messages as if nothing had happened
            base.WndProc(ref msg);
        }

        // most basic action; update the scrollbar position
        void DoScroll(ScrollEventArgs e, ref int curpos, int largeincsize, int max) {
            switch (e.Type) {
                case ScrollEventType.EndScroll:
                    break;

                case ScrollEventType.ThumbPosition:
                    goto case ScrollEventType.ThumbTrack;

                case ScrollEventType.ThumbTrack:
                    curpos = e.NewValue;
                    break;
                case ScrollEventType.SmallDecrement:
                    curpos--;
                    break;
                case ScrollEventType.SmallIncrement:
                    curpos++;
                    break;
                case ScrollEventType.LargeDecrement:
                    curpos -= largeincsize;
                    break;
                case ScrollEventType.LargeIncrement:
                    curpos += largeincsize;
                    break;
                case ScrollEventType.First:
                    curpos = 0;
                    break;
                case ScrollEventType.Last:
                    curpos = max - largeincsize;
                    break;

                default:
                    System.Diagnostics.Debug.Assert(false, string.Format("Unknown scroll event type {0}", e.Type));
                    break;
            }
        }

        void DoHScroll(ref int x, ScrollEventArgs e) {
            GetScrollInfo(Handle, SB_HORIZ, si);

            DoScroll(e, ref si.pos, ClientSize.Width, si.max);

            si.flags = SetScrollFlags.Pos;
            SetScrollInfo(Handle, SB_HORIZ, si, true);
        }

        void DoVScroll(ref int y, ScrollEventArgs e) {
            GetScrollInfo(Handle, SB_VERT, si);

            DoScroll(e, ref si.pos, ClientSize.Height, si.max);

            si.flags = SetScrollFlags.Pos;
            SetScrollInfo(Handle, SB_VERT, si, true);
        }

        public Size ScrollSize {
            get {   return scrollsize;  }
            set {
                scrollsize = value;
                si.flags = SetScrollFlags.Range;

                si.max = value.Width;
                SetScrollInfo(Handle, SB_HORIZ, si, true);

                si.max = value.Height;
                SetScrollInfo(Handle, SB_VERT, si, true);
            }
        }

        public Size PageSize {
            get {   return pagesize;    }
            set {
                pagesize = value;
                si.flags = SetScrollFlags.Page;

                si.page = (uint)value.Width;
                SetScrollInfo(Handle, SB_HORIZ, si, true);

                si.page = (uint)value.Height;
                SetScrollInfo(Handle, SB_VERT, si, true);
            }
        }

        public event ScrollEventHandler OnHScroll;
        public event ScrollEventHandler OnVScroll;

        public void ScrollTo(int x, int y) {
            si.flags = SetScrollFlags.Pos;
            si.pos = x;
            SetScrollInfo(Handle, SB_HORIZ, si, true);

            si.pos = y;
            SetScrollInfo(Handle, SB_VERT, si, true);
        }

        static ScrollEventType GetEventType(IntPtr wparam) {
            int i=wparam.ToInt32()&0xFFFF;

            switch (i) {
                case SB_BOTTOM:			return ScrollEventType.Last;
                case SB_ENDSCROLL: 		return ScrollEventType.EndScroll;
                case SB_LINEDOWN: 		return ScrollEventType.SmallIncrement;
                case SB_LINEUP: 		return ScrollEventType.SmallDecrement;
                case SB_PAGEDOWN: 		return ScrollEventType.LargeIncrement;
                case SB_PAGEUP: 		return ScrollEventType.LargeDecrement;
                case SB_THUMBPOSITION: 	return ScrollEventType.ThumbPosition;
                case SB_THUMBTRACK: 	return ScrollEventType.ThumbTrack;
                case SB_TOP: 			return ScrollEventType.First;
                default:
                    throw new System.ArgumentException(String.Format("{0} isn't a valid scroll event type.", wparam), "wparam");
            }
        }
    }

}