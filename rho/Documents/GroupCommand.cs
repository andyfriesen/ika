using System;
using System.Collections;

namespace rho.Documents {
    public class GroupCommand : Command {
        public GroupCommand() {
        }

        public GroupCommand(Command[] cmd) {
            commands.AddRange(cmd);
        }

        public void Do(Document d) {
            foreach (Command cmd in commands) {
                cmd.Do(d);
            }
        }

        public void Undo(Document d) {
            for (int i = commands.Count - 1; i >= 0; i--) {
                ((Command)commands[i]).Undo(d);
            }
        }

        public void AddCommand(Command cmd) {
            commands.Add(cmd);
        }

        readonly ArrayList commands = new ArrayList();
    }
}
