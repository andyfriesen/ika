using System;
using System.Drawing;
using rho.Documents;

namespace rho.SpriteEditor {
    public class ChangeHotSpotCommand : Command {
        public ChangeHotSpotCommand(Rectangle rect) {
            newHotSpot = rect;
        }

        public void Do(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;
            oldHotSpot = sprite.HotSpot;
            sprite.HotSpot = newHotSpot;
            sprite.FireChanged();
        }

        public void Undo(Document doc) {
            SpriteDocument sprite = (SpriteDocument) doc;
            sprite.HotSpot = oldHotSpot;
            sprite.FireChanged();
        }

        Rectangle newHotSpot;
        Rectangle oldHotSpot;
    }
}
