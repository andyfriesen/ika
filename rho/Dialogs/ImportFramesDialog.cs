
using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using rho.Controls;

namespace rho.Dialogs {
    public class ImportFramesDialog : Form {
        public ImportFramesDialog() {
            VerticalBoxSizer sizer = new VerticalBoxSizer();
            sizer.Dock = DockStyle.Fill;
            Controls.Add(sizer);

            Button browseButton = new Button();
            browseButton.Text = "...";
            browseButton.Width = browseButton.Height;
            browseButton.Click += new EventHandler(OnBrowse);

            Button okButton = new Button();
            okButton.Text = "Ok";
            okButton.Click += new EventHandler(OnOk);
            AcceptButton = okButton;

            Button cancelButton = new Button();
            cancelButton.Text = "Cancel";
            CancelButton = cancelButton;

            sizer
                .Add(
                    new HorizontalBoxSizer()
                        .Add("Filename")
                        .Add(fileNameEdit, 1)
                        .Add(browseButton),
                    SizerFlags.Expand
                )
                .Add(
                    new HorizontalBoxSizer()
                        .Add("Number of frames")
                        .Add(countEdit, 1),
                    SizerFlags.Expand
                )
                .Add(
                    new HorizontalBoxSizer()
                        .Add("Frame Width")
                        .Add(widthEdit, 1),
                    SizerFlags.Expand
                )
                .Add(
                    new HorizontalBoxSizer()
                        .Add("Frame Height")
                        .Add(heightEdit, 1),
                    SizerFlags.Expand
                )
                .Add(
                    new HorizontalBoxSizer()
                        .Add("Row Size")
                        .Add(rowSizeEdit, 1),
                    SizerFlags.Expand
                )
                .Add(
                    new HorizontalBoxSizer()
                        .Add("Pad")
                        .Add(padCheck, 1)
                )
                .Add(
                    new HorizontalBoxSizer()
                        .Add(okButton)
                        .Add(cancelButton)
                );
        }

        void OnOk(object o, EventArgs e) {
            DialogResult = DialogResult.OK;
            Close();
        }

        void OnCancel(object o, EventArgs e) {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        void OnBrowse(object o, EventArgs e) {
            using (OpenFileDialog dlg = new OpenFileDialog()) {
                dlg.Filter = FilterBuilder.Build(
                    "Image Files", "*.png;*.jpg;*.jpeg;*.gif;*.bmp",
                    "PNG Images", "*.png",
                    "JPEG Images", "*.jpg;*.jpeg",
                    "GIF Images", "*.gif",
                    "Bitmap Files", "*.bmp"
                );

                DialogResult result = dlg.ShowDialog();
                
                if (result == DialogResult.OK) {
                    fileNameEdit.Text = dlg.FileName;
                }
            }
        }

        /// <summary>Actually does the importing.</summary>
        public Bitmap[] ImportFrames() {
            ArrayList images = new ArrayList();

            using (Bitmap src = new Bitmap(ImageName)) {
                int pad = Pad ? 1 : 0;
                int x = pad;
                int y = pad;
                int width = FrameWidth;
                int height = FrameHeight;
                int count = FrameCount;
                
                for (int i = 0; i < count; i++) {
                    Bitmap frame = new Bitmap(width, height, PixelFormat.Format32bppArgb);

                    using (Graphics g = Graphics.FromImage(frame)) {
                        g.DrawImage(
                            src, -x, -y, src.Width, src.Height
                        );
                    }

                    images.Add(frame);

                    x += width + pad;
                    if (x >= src.Width) {
                        y += height + pad;
                        x = pad;
                    }
                }
            }

            return (Bitmap[])images.ToArray(typeof(Bitmap));
        }

        public string ImageName { get { return fileNameEdit.Text; } }
        public int RowCount     { get { return Convert.ToInt32(countEdit.Text); } }
        public int FrameWidth   { get { return Convert.ToInt32(widthEdit.Text); } }
        public int FrameHeight  { get { return Convert.ToInt32(heightEdit.Text); } }
        public int FrameCount   { get { return Convert.ToInt32(countEdit.Text); } }
        public bool Pad         { get { return padCheck.Checked; } }

        readonly TextBox fileNameEdit = new TextBox();
        readonly NumberTextBox countEdit = new NumberTextBox();
        readonly NumberTextBox widthEdit = new NumberTextBox();
        readonly NumberTextBox heightEdit = new NumberTextBox();
        readonly NumberTextBox rowSizeEdit = new NumberTextBox();
        readonly CheckBox padCheck = new CheckBox();

#if DEBUG
        public static void Main() {
            ImportFramesDialog dlg = new ImportFramesDialog();
            DialogResult result = dlg.ShowDialog();

            if (result == DialogResult.OK) {
                Console.WriteLine("OK!");
                Console.WriteLine(
                    "filename  {0}\n" +
                    "count     {1}\n" +
                    "width     {2}\n" +
                    "height    {3}\n" +
                    "rowsize   {4}\n" +
                    "pad       {5}",
                    dlg.ImageName,
                    dlg.FrameCount,
                    dlg.FrameWidth,
                    dlg.FrameHeight,
                    dlg.RowCount,
                    dlg.Pad
                );
            } else {
                Console.WriteLine("Canceled!");
            }
        }
#endif
    }
}
