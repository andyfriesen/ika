using System;
using System.Drawing;
using rho.Documents;
using rho.Import;

namespace rho.SpriteEditor {
    public class ResizeSpriteCommand : Command {
        public ResizeSpriteCommand(Size newSize) {
            this.newSize = newSize;
        }

        public void Do(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;

            oldSize = sprite.Size;

            oldFrames = new Bitmap[sprite.Frames.Count];
            for (int i = 0; i < sprite.Frames.Count; i++) {
                Bitmap frame = (Bitmap)sprite.Frames[i];
                oldFrames[i] = new Bitmap(frame);
            }

            sprite.Frames.Resize(newSize.Width, newSize.Height);
            sprite.FireChanged();
        }

        public void Undo(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;

            ImageArray frames = (ImageArray)sprite.Frames;
            frames.Clear();
            frames.Resize(oldSize.Width, oldSize.Height);
            frames.AddRange(oldFrames);
            sprite.FireChanged();
        }

        Size newSize;
        Size oldSize;
        Bitmap[] oldFrames;
    }
}
