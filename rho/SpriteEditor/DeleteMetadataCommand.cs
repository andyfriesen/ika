using System;
using System.Collections.Specialized;
using rho.Documents;

namespace rho.SpriteEditor {
    public class DeleteMetadataCommand : Command {
        public DeleteMetadataCommand(string name) 
            : this(new string[] { name }) {
        }

        public DeleteMetadataCommand(string[] names) {
            this.names = names;
            oldValues = new string[names.Length];
        }

        public void Do(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;
            StringDictionary scripts = sprite.Metadata;

            for (int i = 0; i < names.Length; i++) {
                oldValues[i] = scripts[names[i]];
                scripts.Remove(names[i]);
            }
        }

        public void Undo(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;
            StringDictionary scripts = sprite.Metadata;

            for (int i = 0; i < names.Length; i++) {
                scripts[names[i]] = oldValues[i];
            }
        }

        string[] names;
        string[] oldValues;
    }
}
