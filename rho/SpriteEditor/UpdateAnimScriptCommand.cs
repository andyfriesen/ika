using System;
using System.Collections.Specialized;
using rho.Documents;

namespace rho.SpriteEditor {
    public class UpdateAnimScriptCommand : Command {
        public UpdateAnimScriptCommand(string newName, string newValue)
        : this(newName, newName, newValue) {
        }

        public UpdateAnimScriptCommand(string oldName, string newName, string newValue) {
            this.oldName = oldName;
            this.newName = newName;
            this.newValue = newValue;
        }

        public void Do(Document doc) {
            Console.WriteLine(string.Format("Updating.  oldName={0}  newName={1}  newValue={2}", oldName, newName, newValue));

            SpriteDocument sprite = (SpriteDocument)doc;
            StringDictionary scripts = sprite.AnimScripts;

            if (!scripts.ContainsKey(oldName)) {
                oldName = null;
            }

            if (scripts.ContainsKey(newName)) {
                oldName = newName;
            }

            if (oldName != null) {
                oldValue = scripts[oldName];
            }
            scripts[newName] = newValue;

            if (oldName != null && oldName != newName) {
                scripts.Remove(oldName);
            }
            sprite.FireChanged();
        }

        public void Undo(Document doc) {
            SpriteDocument sprite = (SpriteDocument)doc;
            StringDictionary scripts = sprite.AnimScripts;

            scripts.Remove(newName);
            if (oldName != null) {
                scripts[oldName] = oldValue;
            }
            sprite.FireChanged();
        }

        string oldName;
        string oldValue;
        string newName;
        string newValue;
    }
}
