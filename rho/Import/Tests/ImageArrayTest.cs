using System;
using System.Drawing;
using NUnit.Framework;

namespace rho.Import.Tests {
    [TestFixture]
    public class ImageArrayTest {
        ImageArray array;

        [SetUp]
        public void SetUp() {
            array = new ImageArray(16, 24);
        }

        [Test]
        public void TestSize(){
            Assert.AreEqual(16, array.Width);
            Assert.AreEqual(24, array.Height);
            
            Size s = array.Size;
            Assert.AreEqual(16, s.Width);
            Assert.AreEqual(24, s.Height);
        }

        [Test]
        public void TestAdd() {
            array.Add(new Bitmap(16, 24));
            Assert.AreEqual(1, array.Count);
        }

        [Test]
        public void TestWrongSize() {
            try {
                array.Add(new Bitmap(16, 16));
                Assert.Fail("Adding of 16x16 bitmap should have failed!");
            } catch (Exception) {
            }
        }

        [Test]
        public void TestRemoveAt() {
            array.Add(new Bitmap(16, 24));
            Assert.AreEqual(1, array.Count);

            array.RemoveAt(0);
            Assert.AreEqual(0, array.Count);
        }

        //[Test, Ignore("LAAAAZY")]
        public void TestContains() {
        }

        [Test]
        public void TestIndexOf() {
            Bitmap magic = new Bitmap(16, 24);
            for (int i = 0; i < 4; i++) {
                array.Add(new Bitmap(16, 24));
            }
            array.Add(magic);
            for (int i = 0; i < 4; i++) {
                array.Add(new Bitmap(16, 24));
            }

            Bitmap magic2 = new Bitmap(16, 24);

            Assert.AreEqual(4, array.IndexOf(magic));
            Assert.AreEqual(-1, array.IndexOf(magic2));

            array.Add(magic2);
            Assert.AreEqual(9, array.IndexOf(magic2));
        }

        //[Test, Ignore("LAAAAZY")]
        public void TestInsert() {
        }

        //[Test, Ignore("LAAAAZY")]
        public void TestRemove() {
        }

        [Test]
        public void TestIndexer() {
            Bitmap magic = new Bitmap(16, 24);
            Bitmap magic2 = new Bitmap(16, 24);

            for (int i = 0; i < 4; i++) {
                array.Add(new Bitmap(16, 24));
            }
            array.Add(magic);

            for (int i = 0; i < 4; i++) {
                array.Add(new Bitmap(16, 24));
            }
            array.Add(magic2);

            Assert.AreSame(magic, array[4]);
            Assert.AreSame(magic2, array[9]);
        }

        [Test]
        public void TestResize() {
            for (int i = 0; i < 5; i++) {
                array.Add(new Bitmap(array.Width, array.Height));
            }

            array.Resize(16, 16);

            Assert.AreEqual(16, array.Width);
            Assert.AreEqual(16, array.Height);

            foreach (Bitmap bmp in array) {
                Assert.AreEqual(16, bmp.Width);
                Assert.AreEqual(16, bmp.Height);
            }
        }
    }
}
