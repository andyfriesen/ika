using System;
using System.Text;
using System.IO;
using NUnit.Framework;

namespace Aries.Tests {
    [TestFixture]
    public class AriesTest {
        [Test]
        public void TestReadWriteDocument() {
            DataNode root = new DataNode("root");

            root//.AddChild(new StringNode("wee!"))
                .AddChild(
                new DataNode("child")
                .AddChild("String data!")
                )
                .AddChild(
                new DataNode("child2")
                .AddChild(
                new DataNode("child3")
                .AddChild("nesting!")
                .AddChild("This is so hot.")
                )
                )
                .AddChild(new DataNode("empty-child"))
                .AddChild("YOU CAN'T DO THIS ON TV (or can you?!?!?!?!)")
                .AddChild("FEEL THE BURN");
            ;

            StringBuilder sb = new StringBuilder();
            StringWriter sw = new StringWriter(sb);
            root.Write(sw);

            StringReader sr = new StringReader(sb.ToString());
            DataNode n = Node.ReadDocument(sr);

            Assert.AreEqual(root.Children.Count, n.GetChild("root").Children.Count);
            Assert.AreEqual(root.GetString(), n.GetChild("root").GetString());
        }
    }
}
