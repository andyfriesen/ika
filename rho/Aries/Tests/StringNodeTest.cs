using System;
using System.IO;
using System.Text;
using NUnit.Framework;

namespace Aries.Tests {
    [TestFixture]
    public class StringNodeTest {
        StringNode node;

        [SetUp]
        public void SetUp() {
            node = new StringNode("test");
        }

        [Test]
        public void TestNew() {
            Assert.IsNotNull(node);
            Assert.AreEqual("test", node.ToString());
        }

        [Test]
        public void TestWrite() {
            StringBuilder sb = new StringBuilder();
            StringWriter sw = new StringWriter(sb);
            node.Write(sw);
            Assert.AreEqual("'test'", sb.ToString());
        }

        [Test]
        public void TestEscape() {
            StringBuilder sb = new StringBuilder();
            StringWriter sw = new StringWriter(sb);

            StringNode n = new StringNode("'Apostraphes' are cool (parenths are too)");

            n.Write(sw);
            Assert.AreEqual(@"'\'Apostraphes\' are cool \(parenths are too\)'", sb.ToString());
        }

        [Test]
        public void TestClone() {
            Node clone = node.Clone();
            Assert.IsFalse(clone == node);
            Assert.IsTrue(clone is StringNode);
            Assert.AreEqual(clone.ToString(), node.ToString());
        }
    }
}
