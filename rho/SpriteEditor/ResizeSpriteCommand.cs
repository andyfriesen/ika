using System;
using System.Drawing;
using rho.Documents;

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

            ((rho.Import.ImageArray)sprite.Frames).Resize(newSize.Width, newSize.Height);
        }

        public void Undo(Document doc) {
            
        }

        Size newSize;
        Size oldSize;
        Bitmap[] oldFrames;
    }
}
