#if DEBUG

using System;
using System.Windows.Forms;
using rho.Controls;

namespace rho.Controls.Tests {
    public struct SizerTest {
        public static void Main() {
            Form f = new Form();
            VerticalBoxSizer s = new VerticalBoxSizer();
            s.Dock = DockStyle.Fill;
            f.Controls.Add(s);

            Label l1 = new Label();
            l1.Text = "Name";

            Button b2 = new Button();
            b2.Text = "...";
            b2.Width = 32;

            s.Add(new ComboBox(), SizerFlags.Expand);
            s.Add(new Button(), 1, SizerFlags.Expand);
            s.Add(new TextBox());
            s.Add(new DataGrid(), 2);

            HorizontalBoxSizer s2 = new HorizontalBoxSizer();
            s2.Add(l1);
            s2.Add(new TextBox(), 1);

            s2.Add(b2);

            s.Add(s2, SizerFlags.Expand);

            Application.Run(f);
        }
    }
}

#endif
