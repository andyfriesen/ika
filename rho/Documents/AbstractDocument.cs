using System;
using System.Collections;

namespace rho.Documents {
    public abstract class AbstractDocument : Document {
        protected AbstractDocument(string name) {
            fileName = name;
        }

        public string Name { 
            get {
                return fileName;
            }
            set {
                throw new NotImplementedException("Document renaming is not yet implemented!");
            }
        }

        public void SendCommand(Command cmd) {
            redoList.Clear();
            cmd.Do(this);
            FireChanged();
            undoList.Push(cmd);
        }

        protected abstract void DoSave(string name);

        public void Save(string name) {
            DoSave(name);

#if false
            if (name != fileName) {
                // raise event indicating that the name of the document has been changed.
                throw new NotImplementedException("Document renaming is not yet implemented!");
            }
#endif

            fileName = name;
        }

        public void Save() {
            Save(fileName);
        }

        public bool CanUndo() {
            return undoList.Count > 0;
        }

        public bool CanRedo() {
            return redoList.Count > 0;
        }

        public void Undo() {
            if (CanUndo()) {
                Command cmd = undoList.Pop();
                cmd.Undo(this);
                FireChanged();
                redoList.Push(cmd);
            }
        }

        public void Redo() {
            if (CanRedo()) {
                Command cmd = redoList.Pop();
                cmd.Do(this);
                FireChanged();
                undoList.Push(cmd);
            }
        }

        public void FireChanged() {
            if (Changed != null) {
                Changed();
            }
        }

        public event ChangeEventHandler Changed;

        string fileName;
        readonly CommandStack undoList = new CommandStack();
        readonly CommandStack redoList = new CommandStack();
    }
}
