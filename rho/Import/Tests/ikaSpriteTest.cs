using System;
using System.Drawing;
using System.IO;
using System.Text;
using NUnit.Framework;

namespace rho.Import.Tests {
    [TestFixture]
    public class ikaSpriteTest {
        const int width = 32;
        const int height = 32;
        ikaSprite sprite;

        [SetUp]
        public void SetUp() {
            sprite = new ikaSprite(width, height);
        }

        [Test]
        public void TestNothing() {
            Assert.IsNotNull(sprite);
        }

        [Test]
        public void TestWrite() {
            sprite.Frames.Add(new Bitmap(width, height));
            sprite.Frames.Add(new Bitmap(width, height));
            sprite.Frames.Add(new Bitmap(width, height));
            StringWriter sw = new StringWriter();
            sprite.Save(sw);
            Console.WriteLine(sw.ToString());
            // TODO: assert something... or something. :P
        }
    }
}
