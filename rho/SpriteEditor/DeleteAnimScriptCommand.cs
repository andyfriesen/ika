using System;
using System.Collections.Specialized;
using rho.Documents;

namespace rho.SpriteEditor {
    public class DeleteAnimScriptCommand : Command {
        public DeleteAnimScriptCommand(string name) 
        : this(new string[] { name }) {
        }

        public DeleteAnimScriptCommand(string[] names) {
            this.names = names;
            oldValues = new string[names.Length];
        }

        public void Do(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;
            StringDictionary scripts = sprite.AnimScripts;

            for (int i = 0; i < names.Length; i++) {
                oldValues[i] = scripts[names[i]];
                scripts.Remove(names[i]);
            }
            sprite.FireChanged();
        }

        public void Undo(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;
            StringDictionary scripts = sprite.AnimScripts;

            for (int i = 0; i < names.Length; i++) {
                scripts[names[i]] = oldValues[i];
            }
            sprite.FireChanged();
        }

        string[] names;
        string[] oldValues;
    }
}
