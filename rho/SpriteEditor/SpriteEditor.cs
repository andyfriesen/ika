using System;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI;

using rho.Documents;
using rho.Dialogs;

namespace rho.SpriteEditor {

    public class SpriteEditor : DockContent, IDocumentView {
        public SpriteEditor(SpriteDocument doc) {
            document = doc;

            Splitter splitter = new Splitter();
            detailPanel = new SpriteDetailPanel(document);
            framePanel = new FramePanel(doc);
            scrollPanel = new Panel();

            detailPanel.Dock = DockStyle.Left;
            detailPanel.HotSpotChanged += new SpriteHotSpotHandler(HotSpotChanged);
            detailPanel.AnimScriptChanged += new DataChangeHandler(AnimScriptChanged);
            detailPanel.AnimScriptDeleted += new DataDeletedHandler(AnimScriptsRemoved);
            detailPanel.MetadataChanged += new DataChangeHandler(MetadataChanged);
            detailPanel.MetadataDeleted += new DataDeletedHandler(MetadataRemoved);

            splitter.Dock = DockStyle.Left;
            splitter.SplitPosition = 256;

            scrollPanel.BackColor = Color.Black;
            scrollPanel.AutoScroll = true;
            
            scrollPanel.Controls.Add(framePanel);

            Controls.AddRange(new Control[] { splitter, detailPanel, scrollPanel });

            MinimumSize = new Size(detailPanel.Width, MinimumSize.Height);

            framePanel.FrameSelected += new FrameEventHandler(FrameSelected);
            framePanel.FrameRightClicked += new FrameEventHandler(FrameRightClicked);
            framePanel.Location = new Point(0, 0);

            contextMenu = new ContextMenu(
                new MenuItem[] {
                    new MenuItem("Cut", new EventHandler(CutFrame)),
                    new MenuItem("Copy", new EventHandler(CopyFrame)),
                    new MenuItem("-"),
                    new MenuItem("Paste Over", new EventHandler(PasteOverFrame)),
                    new MenuItem("Insert and Paste", new EventHandler(InsertAndPasteFrame)),
                }
            );

            MenuItem fileMenu = new MenuItem("&File", new MenuItem[] {
                    MenuBuilder.menu(2, "-"),
                    MenuBuilder.menu(3, "&Save", new EventHandler(Save), Shortcut.CtrlS),
                    MenuBuilder.menu(4, "Save &As...", new EventHandler(SaveAs), Shortcut.CtrlShiftS),
                }
            );
            fileMenu.MergeType = MenuMerge.MergeItems;
            fileMenu.MergeOrder = 1;

            MenuItem editMenu = new MenuItem("&Edit", new MenuItem[] {
                    MenuBuilder.menu("&Undo", new EventHandler(Undo), Shortcut.CtrlZ),
                    MenuBuilder.menu("&Redo", new EventHandler(Redo), Shortcut.CtrlY),
                    MenuBuilder.separator(),
                    MenuBuilder.menu("Cu&t", new EventHandler(CutFrame), Shortcut.CtrlX),
                    MenuBuilder.menu("&Copy", new EventHandler(CopyFrame), Shortcut.CtrlC),
                    MenuBuilder.menu("&Paste over", new EventHandler(PasteOverFrame), Shortcut.CtrlShiftV),
                    MenuBuilder.menu("Insert and p&aste", new EventHandler(InsertAndPasteFrame), Shortcut.CtrlV),
                    MenuBuilder.separator(),
                    MenuBuilder.menu("Re&size...", new EventHandler(SpriteResized)),
                    MenuBuilder.menu("&Import frames from image...", new EventHandler(ImportFramesFromImage)),
                }
            );
            editMenu.MergeType = MenuMerge.MergeItems;
            editMenu.MergeOrder = 2;

            Menu = new MainMenu(new MenuItem[] { fileMenu, editMenu });
        }

        public string FileName {
            get { return document.Name; }
            set { Text = document.Name = value; }
        }

        public void Save() {
            document.Save();
        }

        public void Save(string filename) {
            document.Save(filename);
        }

        void Save(object o, EventArgs e) {
            if (FileName == null) {
                SaveAs(o, e);
            } else {
                Save();
            }
        }

        void SaveAs(object o, EventArgs e) {
            using (SaveFileDialog fd = new SaveFileDialog()) {
                fd.AddExtension = true;
                fd.DefaultExt = "ika-sprite";
                fd.Filter = "ika Sprites (*.ika-sprite)|*.ika-sprite";
                fd.RestoreDirectory = true;
            
                if (fd.ShowDialog(this) == DialogResult.OK) {
                    Save(fd.FileName);
                }

                Text = fd.FileName;
            }        
        }
        
        void Undo(object o, EventArgs e) {
            document.Undo();
        }

        void Redo(object o, EventArgs e) {
            document.Redo();
        }

        void FrameSelected(FrameEventArgs e) {
            framePanel.SelectedFrame = e.Index;
            Console.WriteLine("Index = {0}", e.Index);
        }

        void FrameRightClicked(FrameEventArgs e) {
            framePanel.SelectedFrame = e.Index;
            contextMenu.Show(this, PointToClient(Cursor.Position));
        }

        void CutFrame(object o, EventArgs e) {
            CopyFrame(o, e);
            document.SendCommand(new DeleteFrameCommand(framePanel.SelectedFrame));
        }

        void CopyFrame(object o, EventArgs e) {
            int index = framePanel.SelectedFrame;
            if (index != -1) {
                Clipboard.SetDataObject(document.Frames[index], true);
            }
        }

        void PasteOverFrame(object o, EventArgs e) {
            IDataObject ido = Clipboard.GetDataObject();

            if (!ido.GetDataPresent("System.Drawing.Bitmap", true)) {
                MessageBox.Show(this, "There doesn't look to be an image in the clipboard");
                return;
            }

            Bitmap frame = (Bitmap)ido.GetData("System.Drawing.Bitmap");

            if (frame.Size != document.Size) {
                MessageBox.Show(this, "Can't paste things that are the wrong size yet.  Sorry!");
                return;
            }

            document.SendCommand(new ReplaceFrameCommand(framePanel.SelectedFrame, frame));
        }

        void InsertAndPasteFrame(object o, EventArgs e) {
            IDataObject ido = Clipboard.GetDataObject();
            Bitmap frame;

            if (ido.GetDataPresent("System.Drawing.Bitmap")) {
                frame = (Bitmap) ido.GetData("System.Drawing.Bitmap");
            } else if (ido.GetDataPresent("Bitmap")) {
                frame = (Bitmap) ido.GetData("Bitmap");
            } else {
                MessageBox.Show(this, "There doesn't look to be an image in the clipboard");
                return;
            }

            if (frame.Size != document.Size) {
                MessageBox.Show(this, "Can't paste things that are the wrong size yet.  Sorry!");
                return;
            }

            document.SendCommand(new InsertFrameCommand(framePanel.SelectedFrame, frame));
        }

        void ImportFramesFromImage(object o, EventArgs e) {
            using (ImportFramesDialog dlg = new ImportFramesDialog()) {
                DialogResult result = dlg.ShowDialog();

                if (result == DialogResult.OK) {
                    Bitmap[] frames = dlg.ImportFrames();
                    
                    // Build monster command thing
                    GroupCommand cmd = new GroupCommand();
                    for (int i = 0; i < frames.Length; i++) {
                        cmd.AddCommand(new InsertFrameCommand(document.Frames.Count + i, frames[i]));
                    }

                    document.SendCommand(cmd);
                }
            }
        }

        void SpriteResized(object o, EventArgs e) {
            using (ResizeDialog dlg = new ResizeDialog()) {
                
                DialogResult result = dlg.ShowDialog();
                
                if (result == DialogResult.OK) {
                    document.SendCommand(new ResizeSpriteCommand(dlg.NewSize));
                }
            }
        }

        void HotSpotChanged(Rectangle newHotSpot) {
            document.SendCommand(new ChangeHotSpotCommand(newHotSpot));
        }

        void AnimScriptChanged(string oldName, string newName, string newValue) {
            document.SendCommand(new UpdateAnimScriptCommand(oldName, newName, newValue));
        }

        void AnimScriptsRemoved(string[] names) {
            document.SendCommand(new DeleteAnimScriptCommand(names));
        }

        void MetadataChanged(string oldName, string newName, string newValue) {
            document.SendCommand(new UpdateMetadataCommand(oldName, newName, newValue));
        }

        void MetadataRemoved(string[] names) {
            document.SendCommand(new DeleteMetadataCommand(names));
        }

        void SpriteChanged() {
            Refresh();
        }

        protected override void OnLayout(LayoutEventArgs e) {
            base.OnLayout(e);
            // Ugly hack to work around the lack of good sizer logic. :P
            if (scrollPanel != null && detailPanel != null) {
                scrollPanel.Location = new System.Drawing.Point(detailPanel.Right, 0);
                scrollPanel.Size = new Size(Width - detailPanel.Right, ClientSize.Height);
                framePanel.Width = scrollPanel.ClientSize.Width;
            }
        }

        SpriteDocument document;
        readonly ScrollableControl scrollPanel; // contains the framePanel
        readonly SpriteDetailPanel detailPanel;
        readonly FramePanel framePanel;
        readonly ContextMenu contextMenu;
    }
}
