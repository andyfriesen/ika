using System;
using System.Diagnostics;
using System.Drawing;
using rho.Documents;

namespace rho.SpriteEditor {
    public class InsertFrameCommand : Command {
        public InsertFrameCommand(int position, Bitmap bitmap) {
            this.position = position;
            this.bitmap = bitmap;
        }

        public void Do(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;

            Console.WriteLine(sprite.Size);

            sprite.Frames.Insert(position, bitmap);
            sprite.FireChanged();
        }

        public void Undo(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;

            sprite.Frames.RemoveAt(position);
            sprite.FireChanged();
        }

        int position;
        Bitmap bitmap;
    }
}
