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

namespace rho.Controls
{

    public class ScrollWindow : ScrollableControl
    {
        // win32 constants.
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

        protected override void WndProc(ref Message msg)
        {
            // more than a little hackish. -_-
            // At least it's contained, though.
            if (msg.Msg==WM_VSCROLL)
            {
                int position=(msg.WParam.ToInt32()>>16);
                ScrollEventArgs sea=new ScrollEventArgs(GetEventType(msg.WParam),position);
			
                DoVScroll(ref position,sea);			
                sea.NewValue=position;
			
                OnVScroll(this,sea);
                return;
            }
            else if (msg.Msg==WM_HSCROLL)
            {
                int position=(msg.WParam.ToInt32()>>16);
                ScrollEventArgs sea=new ScrollEventArgs(GetEventType(msg.WParam),position);
			
                DoHScroll(ref position,sea);
                sea.NewValue=position;
			
                OnHScroll(this,new ScrollEventArgs(GetEventType(msg.WParam),position));
                return;
            }
		
            // process all other messages as if nothing had happened
            base.WndProc(ref msg);
        }
	
        // most basic action; update the scrollbar position
        void DoScroll(ScrollEventArgs e,ref int curpos,int largeincsize,int max)
        {
            switch (e.Type)
            {
                case ScrollEventType.EndScroll:	return;
			
                case ScrollEventType.ThumbPosition:
                    goto case ScrollEventType.ThumbTrack;
			
                case ScrollEventType.ThumbTrack:		curpos=e.NewValue;		break;
                case ScrollEventType.SmallDecrement:	curpos--;				break;			
                case ScrollEventType.SmallIncrement:	curpos++;				break;
                case ScrollEventType.LargeDecrement:	curpos-=largeincsize;	break;
                case ScrollEventType.LargeIncrement:	curpos+=largeincsize;	break;
                case ScrollEventType.First:				curpos=0;				break;
                case ScrollEventType.Last:				curpos=max-largeincsize;break;
            }
        }
	
        void DoHScroll(ref int x,ScrollEventArgs e)
        {
            x=-AutoScrollPosition.X;
            DoScroll(e,ref x,ClientSize.Width,AutoScrollMinSize.Width);

            Point p=AutoScrollPosition;
            p.X=x;
            p.Y=-p.Y;
            AutoScrollPosition=p;
        }
	
        void DoVScroll(ref int y,ScrollEventArgs e)
        {		
            y=-AutoScrollPosition.Y;
            DoScroll(e,ref y,ClientSize.Height,AutoScrollMinSize.Height);
		
            Point p=AutoScrollPosition;
            p.X=-p.X;
            p.Y=y;
            AutoScrollPosition=p;
        }
	
        public event ScrollEventHandler OnHScroll;
        public event ScrollEventHandler OnVScroll;
	
        public void ScrollTo(int x,int y)
        {
            Point p=new Point(x,y);
            AutoScrollPosition=p;
        }
	
        static ScrollEventType GetEventType(IntPtr wparam)
        {
            int i=wparam.ToInt32()&0xFFFF;
		
            switch (i)
            {
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
                    throw new System.ArgumentException(String.Format("{0} isn't a valid scroll event type.",wparam),"wparam");
            }
        }
    }

}