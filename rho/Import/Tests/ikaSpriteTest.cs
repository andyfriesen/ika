using System;
using NUnit.Framework;

namespace rho.Import.Tests {
    [TestFixture]
    public class ikaSpriteTest {
        ikaSprite sprite;

        [SetUp]
        public void SetUp() {
            sprite = new ikaSprite();
        }

        [Test]
        public void TestNothing() {
            Assert.IsNotNull(sprite);
        }
    }
}
