using System;
using System.Collections;
using System.IO;

namespace Aries {
    public class DataNode : Node {
        public DataNode(string name) {
            _name = name;
        }

        public string Name {
            get { return _name; }
            set { _name = value; }
        }
        
        public IList Children {
            get {
                return _children;
            }
        }

        public Node[] ChildArray() {
            return (Node[])_children.ToArray(typeof(Node));
        }

        public override Node Clone() {
            DataNode dn = new DataNode(_name);
            dn._children.AddRange(_children);
            return dn;
        }

        /// <summary>
        /// Finds a child node
        /// </summary>
        /// <param name="name">Name of child to search for</param>
        /// <param name="def">Node to be returned if the child is not found</param>
        /// <returns>The child</returns>
        public DataNode GetChild(string name, DataNode def) {
            foreach (Node n in _children) {
                try {
                    DataNode dn = (DataNode)n;
                    if (dn._name == name)
                        return dn;
                }
                catch (InvalidCastException) {}
            }

            return def;
        }

        /// <summary>
        /// Finds a child node.  If the child is not found, an Exception is thrown.
        /// </summary>
        /// <param name="name">The name of the child to search for.</param>
        /// <returns>The child</returns>
        public DataNode GetChild(string name) {
            DataNode n = GetChild(name, null);
            if (n == null) {
                throw new Exception(String.Format("Node {0} not found found within data node {1}", name, _name));
            } else {
                return n;
            }
        }

        /// <summary>
        /// Returns an array of all child nodes with the specified name.
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public DataNode[] GetChildren(string name) {
            ArrayList list = new ArrayList();

            foreach (Node n in _children) {
                DataNode dn = n as DataNode;
                if (dn != null) {
                    if (dn._name == name)
                        list.Add(dn);
                }
            }

            return (DataNode[])list.ToArray(typeof(DataNode));
        }

        /// <summary>
        /// Returns the contents of the first child node that is a string.
        /// Returns the empty string if the node has no string children.
        /// </summary>
        /// <returns>The contents of the first string child node</returns>
        public string GetString() {
            foreach (Node n in _children) {
                StringNode sn = n as StringNode;
                if (sn != null) {
                    return sn.ToString();
                }
            }

            return "";
        }
        
        /// <summary>
        /// Sets the string argument passed to the contents of the first child 
        /// node that is a string, or the empty string if no such node exists.
        /// </summary>
        /// <param name="data">Recieves the string</param>
        /// <returns>Self reference</returns>
        public DataNode GetString(out string data) {
            data = GetString();
            return this;
        }

        /// <summary>
        /// Reads the first string child node, tries to convert it to an 
        /// integer, then returns it in 'data'.  An exception is thrown if
        /// the conversion cannot be done for whatever reason.
        /// </summary>
        /// <param name="data">Recieves the thingie</param>
        /// <returns>Self reference</returns>
        public DataNode GetString(out int data) {
            string s = GetString();
            if (s == "")
                throw new InvalidCastException();
            data = Convert.ToInt32(s);
            return this;
        }

        /// <summary>
        /// Adds a string node to the node's child list.
        /// </summary>
        /// <param name="data"></param>
        /// <returns>Self reference.</returns>
        public DataNode AddChild(string data) {
            _children.Add(new StringNode(data));
            return this;
        }

        /// <summary>
        /// Adds the node to the child list.
        /// </summary>
        /// <param name="n"></param>
        /// <returns>Self reference.</returns>
        public DataNode AddChild(Node n) {
            _children.Add(n);
            return this;
        }

        public override void Write(TextWriter writer) {
            Write(writer, 0);
        }

        public void Write(TextWriter writer, int indentLevel) {
            indentLevel += 1;

            writer.Write("(" + _name + " ");

            if (_children.Count == 1 && _children[0] is StringNode) {
                // When there's just one string node, no newlines.  It's ugly looking.
                ((Node)_children[0]).Write(writer);
                writer.Write(")");
            } else {
                for (int i = 0; i < _children.Count; i++) {
                    writer.Write("\n" + new String('\t', indentLevel));

                    if (_children[i] is StringNode) {
                        ((StringNode)_children[i]).Write(writer);
                    } else {
                        ((DataNode)_children[i]).Write(writer, indentLevel);
                    }
                }
                writer.Write("\n" + new String('\t', indentLevel - 1) + ")");
            }
        }


        private string _name;
        private readonly ArrayList _children = new ArrayList();
    }
}
