using System;
using System.Collections;

namespace rho.Documents {
    public class CommandStack : Stack {
        public new Command Pop() {
            return (Command)base.Pop();
        }

        public void Push(Command cmd) {
            base.Push(cmd);
        }
    }
}
