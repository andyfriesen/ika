using System;
using System.IO;
using System.Text;
using System.Collections;
using NUnit.Framework;

namespace Aries.Tests {
    [TestFixture]
    public class DataNodeTest {
        DataNode node;

        [SetUp]
        public void SetUp() {
            node = new DataNode("test");
        }

        [Test]
        public void TestNew() {
            Assert.IsNotNull(node);
        }

        [Test]
        public void TestName() {
            Assert.AreEqual("test", node.Name);

            node.Name = "something";
            Assert.AreEqual("something", node.Name);
        }

        [Test]
        public void TestChildren() {
            IList children = node.Children;
            Assert.IsNotNull(children);
        }

        [Test]
        public void TestChildArray() {
            Node[] children = node.ChildArray();
            Assert.IsNotNull(children);
        }

        [Test]
        public void TestClone() {
            Node clone = node.Clone();
            Assert.IsNotNull(clone);
            Assert.IsTrue(clone is DataNode);

            DataNode dn = clone as DataNode;
            Assert.AreSame(clone, dn);
            Assert.AreEqual(dn.Name, node.Name);
        }

        [Test]
        public void TestGetChild() {
            DataNode child1 = new DataNode("child1");
            DataNode child2 = new DataNode("child2");
            node.AddChild(child1).AddChild(child2);
            
            Assert.AreSame(child1, node.GetChild("child1"));
            Assert.AreSame(child2, node.GetChild("child2"));
            Assert.IsNull(node.GetChild("does_not_exist", null));
            
            try {
                node.GetChild("does_not_exist");
                Assert.Fail("Node.GetChild(\"does_not_exist\") expected to raise an exception");
            } catch (Exception) {
                // no-op
            }
        }

        [Test]
        public void TestGetString() {
            Assert.AreEqual("", node.GetString());

            const string testValue = "42";

            node.AddChild(new DataNode("node1"))
                .AddChild(testValue)
                .AddChild(new DataNode("node2"))
                ;

            Assert.AreEqual(testValue, node.GetString());

            string result;
            node.GetString(out result);
            Assert.AreEqual(result, testValue);

            int intResult;
            node.GetString(out intResult);
            Assert.AreEqual(intResult, Int32.Parse(testValue));
        }

        [Test]
        public void TestAddNode() {
            DataNode result = node.AddChild(new DataNode("child"));
            Assert.AreSame(node, result);
            Assert.IsTrue(node.Children.Count == 1);
        }

        [Test]
        public void TestAddString() {
            DataNode result = node.AddChild("child");
            Assert.AreSame(node, result);
            Assert.IsTrue(node.Children.Count == 1);
        }

    }
}
