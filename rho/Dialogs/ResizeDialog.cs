using System;
using System.Drawing;
using System.Windows.Forms;
using rho.Controls;

namespace rho.Dialogs {
    public class ResizeDialog : Form {
        public ResizeDialog() {
            widthEdit = new NumberTextBox();
            heightEdit = new NumberTextBox();
            Button okButton = new Button();
            Button cancelButton = new Button();

            AcceptButton = okButton;
            CancelButton = cancelButton;

            okButton.Text = "Ok";
            cancelButton.Text = "Cancel";

            okButton.Click += new EventHandler(OnOk);
            cancelButton.Click += new EventHandler(OnCancel);

            BoxSizer sizer = new VerticalBoxSizer()
                .Add(
                    new HorizontalBoxSizer()
                        .Add("New width")
                        .Add(widthEdit, SizerFlags.Expand)
                )
                .Add(
                    new HorizontalBoxSizer()
                        .Add("New height")
                        .Add(heightEdit, SizerFlags.Expand)
                )
                .Add(
                    new HorizontalBoxSizer()
                        .Add(okButton)
                        .Add(cancelButton)
                )
            ;

            Controls.Add(sizer);
            sizer.Dock = DockStyle.Fill;
        }

        void OnOk(object o, EventArgs e) {
            DialogResult = DialogResult.OK;
            Close();
        }

        void OnCancel(object o, EventArgs e) {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        public Size NewSize {
            get {
                return new Size(
                    Convert.ToInt32(widthEdit.Text),
                    Convert.ToInt32(heightEdit.Text)
                );
            }
        }

        TextBox widthEdit;
        TextBox heightEdit;

#if DEBUG

    public static void Main() {
        ResizeDialog dlg = new ResizeDialog();
        DialogResult result = dlg.ShowDialog();
        Console.WriteLine(result);
    }
    
#endif
    }
}
