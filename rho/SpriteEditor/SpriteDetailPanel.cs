using System;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using rho.Controls;

namespace rho.SpriteEditor {
    public delegate void SpriteHotSpotHandler(Rectangle HotSpot);
    public delegate void DataChangeHandler(string oldName, string newName, string newValue);
    public delegate void DataDeletedHandler(string[] names);

    public class SpriteDetailPanel : Panel {
        private static Label newLabel(string t) {
            Label l = new Label();
            l.Text = t;
            l.Width = l.PreferredWidth;
            return l;
        }

        public SpriteDetailPanel(SpriteDocument sprite) {
            editHotX.Enter += new EventHandler(BeginEdit);
            editHotY.Enter += new EventHandler(BeginEdit);
            editHotWidth.Enter += new EventHandler(BeginEdit);
            editHotHeight.Enter += new EventHandler(BeginEdit);

            editHotX.Leave += new EventHandler(ChangeHotX);
            editHotY.Leave += new EventHandler(ChangeHotY);
            editHotWidth.Leave += new EventHandler(ChangeHotWidth);
            editHotHeight.Leave += new EventHandler(ChangeHotHeight);

            editHotX.KeyPress += new KeyPressEventHandler(ValidateKey);
            editHotY.KeyPress += new KeyPressEventHandler(ValidateKey);
            editHotWidth.KeyPress += new KeyPressEventHandler(ValidateKey);
            editHotHeight.KeyPress += new KeyPressEventHandler(ValidateKey);

            splitter.Dock = DockStyle.Bottom;

            metaGrid.CaptionText = "Metadata";
            metaGrid.Dock = DockStyle.Bottom;

            animGrid.CaptionText = "Animation scripts";
            animGrid.Values = sprite.AnimScripts;

            animGrid.ValueAdded += new DictionaryValueAddedHandler(OnAnimScriptAdded);
            animGrid.ValueChanged += new DictionaryValueChangeHandler(OnAnimScriptChanged);
            animGrid.ValueDeleted += new DictionaryValueDeleteHandler(OnAnimScriptsDeleted);

            metaGrid.ValueAdded += new DictionaryValueAddedHandler(OnMetadataAdded);
            metaGrid.ValueChanged += new DictionaryValueChangeHandler(OnMetadataChanged);
            metaGrid.ValueDeleted += new DictionaryValueDeleteHandler(OnMetadataDeleted);

            Controls.AddRange(new Control[] {
                    xLabel,
                    editHotX,
                    yLabel,
                    editHotY,
                    widthLabel,
                    editHotWidth,
                    heightLabel,
                    editHotHeight,
                    animGrid,
                    splitter,
                    metaGrid,
                }
            );

            OnLayout(null);
            ClientSize = new System.Drawing.Size(animGrid.Right, animGrid.Bottom);
            UpdateDialog(sprite);
        }

        public event SpriteHotSpotHandler HotSpotChanged;
        public event DataChangeHandler AnimScriptChanged;
        public event DataDeletedHandler AnimScriptDeleted;
        public event DataChangeHandler MetadataChanged;
        public event DataDeletedHandler MetadataDeleted;

        protected override void OnLayout(LayoutEventArgs levent) {
            // UGH
            if (levent != null) {
                base.OnLayout(levent);
            }
            int w = heightLabel.PreferredWidth;
            int h = editHotX.Height;

            int x = 0;
            int y = 0;
            int yinc = h + 3;
            xLabel.Left = x;
            xLabel.Top = y;
            xLabel.Width = w;
            editHotX.Left = x + w;
            editHotX.Top = y;
            
            y += yinc;
            yLabel.Left = x;
            yLabel.Top = y;
            yLabel.Width = w;
            editHotY.Left = x + w;
            editHotY.Top = y;

            y += yinc;
            widthLabel.Left = x;
            widthLabel.Top = y;
            widthLabel.Width = w;
            editHotWidth.Left = x + w;
            editHotWidth.Top = y;

            y += yinc;
            heightLabel.Left = x;
            heightLabel.Top = y;
            heightLabel.Width = w;
            editHotHeight.Left = x + w;
            editHotHeight.Top = y;

            y += yinc;
            h = splitter.Location.Y;
            animGrid.Location = new Point(0, y);
            animGrid.Size = new Size(ClientRectangle.Width, h - y);
        }

        void UpdateDialog(SpriteDocument sprite) {
            hotSpot = sprite.HotSpot;
            editHotX.Text = hotSpot.X.ToString();
            editHotY.Text = hotSpot.Y.ToString();
            editHotWidth.Text = hotSpot.Width.ToString();
            editHotHeight.Text = hotSpot.Height.ToString();
        }

        void BeginEdit(object o, EventArgs e) {
            TextBox t = (TextBox)o;
            previousValue = ParseInt(t.Text);
        }

        void ChangeHotX(object sender, EventArgs e) {
            hotSpot.X = ParseInt(editHotX.Text);
            if (hotSpot.X != previousValue) {
                SendChange(hotSpot);
            }
        }

        void ChangeHotY(object sender, EventArgs e) {
            hotSpot.Y = ParseInt(editHotY.Text);
            if (hotSpot.Y != previousValue) {
                SendChange(hotSpot);
            }
        }

        void ChangeHotWidth(object sender, EventArgs e) {
            hotSpot.Width = ParseInt(editHotWidth.Text);
            if (hotSpot.Width != previousValue) {
                SendChange(hotSpot);
            }
        }

        void ChangeHotHeight(object sender, EventArgs e) {
            hotSpot.Height = ParseInt(editHotHeight.Text);
            if (hotSpot.Height != previousValue) {
                SendChange(hotSpot);
            }
        }

        void SendChange(Rectangle hotSpot) {
            if (HotSpotChanged != null) {
                HotSpotChanged(hotSpot);
            }
        }

        void OnAnimScriptAdded(string name, string value) {
            OnAnimScriptChanged(null, name, value);
        }

        void OnAnimScriptChanged(string oldName, string newName, string value) {
            if (AnimScriptChanged != null) {
                AnimScriptChanged(oldName, newName, value);
            }
        }

        void OnAnimScriptsDeleted(string name) {
            if (AnimScriptDeleted != null) {
                AnimScriptDeleted(new string[] { name });
            }
        }

        void OnMetadataAdded(string name, string value) {
            OnMetadataChanged(null, name, value);
        }

        void OnMetadataChanged(string oldName, string newName, string value) {
            if (MetadataChanged != null) {
                MetadataChanged(oldName, newName, value);
            }
        }

        void OnMetadataDeleted(string name) {
            if (MetadataDeleted != null) {
                MetadataDeleted(new string[] { name });
            }
        }


        static int ParseInt(string s) {
            try {
                return int.Parse(s);
            } catch (FormatException) {
                return 0;
            }
        }

        int previousValue;
        Rectangle hotSpot;

        readonly Label xLabel = newLabel("X");
        readonly Label yLabel = newLabel("Y");
        readonly Label widthLabel = newLabel("Width");
        readonly Label heightLabel = newLabel("Height");
        readonly TextBox editHotX = new TextBox();
        readonly TextBox editHotY = new TextBox();
        readonly TextBox editHotWidth = new TextBox();
        readonly TextBox editHotHeight = new TextBox();
        readonly DictionaryGrid animGrid = new DictionaryGrid();
        readonly DictionaryGrid metaGrid = new DictionaryGrid();
        readonly Splitter splitter = new Splitter();

        private void ValidateKey(object sender, KeyPressEventArgs e) {
            char c = e.KeyChar;
            if (!char.IsDigit(c) && !char.IsControl(c)) {
                e.Handled = true;
            }
        }
    }
}
