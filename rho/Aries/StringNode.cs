using System;
using System.IO;

namespace Aries {
    public class StringNode : Node {
        public StringNode(string data) {
            _data = data;
        }

        public override string ToString() {
            return _data;
        }

        public override void Write(TextWriter writer) {
            // insert escape things before important chars
            string temp = _data
                .Replace("\'", @"\'")
                .Replace("(", @"\(")
                .Replace(")", @"\)")
            ;

            writer.Write("'");
            writer.Write(temp);
            writer.Write("'");
        }

        public override Node Clone() {
            return new StringNode(_data);
        }

        private string _data;
    }

}
