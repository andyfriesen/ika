using System;
using System.IO;
using System.Collections;
using System.Text;

namespace Aries {
    public abstract class Node {
        public abstract Node Clone();
        public abstract void Write(TextWriter writer);

        private static bool IsWhiteSpace(char c) {
            return c == ' ' || c == '\n' || c == '\t' || c == '\r';
        }

        private static string ReadIdentifier(TextReader stream) {
            string s = "";

            // do { ... } while (cond) { ... };
            do {
                int i = stream.Peek();
                if (i < 0 || IsWhiteSpace((char)i))    break;
                stream.Read();
                s += (char)i;
            } while (true);

            return s;
        }

        private static void EatWhiteSpace(TextReader stream) {
            do {
                int i = stream.Peek();
                if (i < 0 || !IsWhiteSpace((char)i))    break;
                stream.Read();
            } while (true);
        }

        private static string ReadQuotedString(TextReader stream) {
            // New as of 0.60: quoted strings behave like C string literals.
            // (except using single quotes instead of double)
            // Backslash is interpreted like in C.

            StringBuilder sb = new StringBuilder();

            while (true) {
                // These are defined as constants because they look too similar if used in the code directly.
                // It confuses my feeble mind. :(
                const char quote = '\'';
                const char backSlash = '\\';

                int i = stream.Read();
                if (i < 0) {
                    throw new Exception("Unterminated string literal data");
                }

                char c = (char)i;

                if (c == quote) {
                    break;

                } else if (c != backSlash) {
                    sb.Append(c);

                } else {
                    i = stream.Read();

                    if (i < 0) {
                        throw new Exception("Unterminated string literal data (unterminated escape sequence too!)");
                    }

                    c = (char)i;

                    if (c == 'n') {
                        sb.Append('\n');
                    } else if (c == 't') {
                        sb.Append('\t');
                    } else {
                        sb.Append(c);
                    }
                }
            }

            return sb.ToString();
        }

        /// <remarks>TODO: refactor.  This is nasty looking.</remarks>
        public static DataNode ReadDocument(TextReader stream) {
            DataNode root = new DataNode("root");
            Stack docStack = new Stack();
            docStack.Push(root);

            while (stream.Peek() >= 0) {
                char c = (char)stream.Read();

                if (IsWhiteSpace(c)) {
                    continue;
                } else if (c == '(') {
                    string nodeName = ReadIdentifier(stream).Trim();
                    DataNode newNode = new DataNode(nodeName);
                    ((DataNode)docStack.Peek()).AddChild(newNode);
                    docStack.Push(newNode);
                    EatWhiteSpace(stream);
                } else if (c == ')') {
                    if (docStack.Count < 2) {
                        throw new Exception("Malformed markup: ) without matching (");
                    } else {
                        docStack.Pop();
                    }
                } else if (c == '\'') {
                    // New as of 0.60: quoted strings behave like C string literals.
                    // (except using single quotes instead of double)
                    // Backslash is interpreted like in C.
                    string s = ReadQuotedString(stream);
                    ((DataNode)docStack.Peek()).AddChild(s);
                } else {
                    // Old style in-place string literals
                    string s = c.ToString();
                    do {
                        int i = stream.Peek();
                        c = (char)i;
                        if (i < 0 || c == '(' || c == ')' || c == '\'') {
                            break; 
                        }
                        stream.Read();
                        s += (char)i;
                    } while (true);

                    s = s.Trim();
                    if (s.Length > 0) {
                        ((DataNode)docStack.Peek()).AddChild(s.Trim());
                    }
                }
            }

            return root;
        }
        public static DataNode ReadDocument(string fname) {
            using (StreamReader stream = new StreamReader(fname)) {
                return ReadDocument(stream);
            }
        }
    }
}
