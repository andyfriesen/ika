using System;
using System.Drawing;
using rho.Documents;

namespace rho.SpriteEditor {
    public class DeleteFrameCommand : Command {
        public DeleteFrameCommand(int position) {
            this.position = position;
        }

        public void Do(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;
            
            oldFrame = (Bitmap)sprite.Frames[position];
            sprite.Frames.RemoveAt(position);
            sprite.FireChanged();
        }

        public void Undo(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;

            sprite.Frames.Insert(position, oldFrame);
            sprite.FireChanged();
        }

        int position;
        Bitmap oldFrame;
    }
}
