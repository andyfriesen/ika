using System;
using System.Collections;
using System.Collections.Specialized;
using System.Drawing;
using rho.Documents;
using rho.Commands;

namespace rho.SpriteEditor {
    public class SpriteDocument : AbstractDocument {
        public SpriteDocument(string name) : base(name) {            
            sprite = new rho.Import.ikaSprite(name);
        }

        public override void DoSave(string name) {

        }

        public IList Frames {
            get { return sprite.Frames; }
        }

        public Size Size {
            get { return sprite.Size; }
        }

        rho.Import.ikaSprite sprite;
        
    }
}
