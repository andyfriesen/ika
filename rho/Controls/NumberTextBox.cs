using System;
using System.Windows.Forms;

namespace rho.Controls {
    /// <summary>
    /// A text box that only permits numeric inputs.
    /// </summary>
    public class NumberTextBox : TextBox {
        protected override void OnKeyPress(KeyPressEventArgs e) {
            char c = e.KeyChar;

            if (!char.IsDigit(c) && !char.IsControl(c)) {
                e.Handled = true;
            } else {
                base.OnKeyPress (e);
            }            
        }

    }
}
