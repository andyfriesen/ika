using System;
using rho.Documents;
using rho.Commands;

namespace rho.TextEditor {
    public class TextDocument : AbstractDocument {
        public TextDocument(string name) : base(name) { }

        public override void DoSave(string name) {
            // blah?
            base.Save(name);
        }
    }
}
