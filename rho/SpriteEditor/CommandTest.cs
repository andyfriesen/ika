using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace rho.SpriteEditor.Tests {
    [TestFixture]
    public class CommandTest {
        SpriteDocument document;

        [SetUp]
        public void SetUp() {
            document = new SpriteDocument();
        }

        [Test]
        public void TestInsertFrame() {
        }
    }
}
