using System;
using rho.Commands;

namespace rho.Documents {
    public delegate void ChangeEventHandler();

    public interface Document {
        string Name { get; set; }

        void SendCommand(Command cmd);
        void Save(string fileName);

        bool CanUndo();
        bool CanRedo();

        void Undo();
        void Redo();

        event ChangeEventHandler Changed;
    }
}
