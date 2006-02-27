using System;
using WeifenLuo.WinFormsUI;

using rho.Documents;
using rho.Import;

namespace rho.SpriteEditor {
    public class SpriteFactory : DocumentFactory {
        public Document NewFile() {
            return new SpriteDocument();
        }

        public Document OpenFile(string fileName) {
            return new SpriteDocument(fileName);
        }

        public DockContent OpenWindow(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;
            return new SpriteEditor(sprite);
        }

        public string DocumentName {
            get { 
                return "Sprite";
            }
        }

        public string DocumentFilter {
            get {
                return "*.ika-sprite";
            }
        }

        public Type DocumentType {
            get {
                return typeof(SpriteDocument);
            }
        }
    }
}
