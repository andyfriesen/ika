using System;
using System.Drawing;
using rho.Documents;

namespace rho.SpriteEditor {
    public class ReplaceFrameCommand : Command {
        public ReplaceFrameCommand(int position, Bitmap newFrame) {
            this.position = position;
            this.newFrame = newFrame;
        }

        public void Do(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;

            oldFrame = (Bitmap)sprite.Frames[position];
            sprite.Frames[position] = newFrame;
            sprite.FireChanged();
        }

        public void Undo(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;

            sprite.Frames[position] = oldFrame;
            sprite.FireChanged();
        }

        int position;
        Bitmap newFrame;
        Bitmap oldFrame;
    }
}
