using System;

namespace rho.MapEditor
{
    using System.Windows.Forms;

	/// <summary>
	/// Represents the current state of the map editor.
	/// </summary>
	interface IState
	{
        void MouseDown(MouseEventArgs e);
        void MouseUp(MouseEventArgs e);
        void MouseWheel(MouseEventArgs e);
        void MouseMove(MouseEventArgs e);

        void KeyDown(KeyEventArgs e);
        void KeyUp(KeyEventArgs e);
    }
}
