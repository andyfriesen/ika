using System;

namespace rho.Documents {
    public interface Command {
        void Do(Document d);
        void Undo(Document d);
    }
}
