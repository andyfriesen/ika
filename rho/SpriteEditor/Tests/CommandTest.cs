using System;
using System.Drawing;
using NUnit.Framework;
using rho.Documents;

namespace rho.SpriteEditor.Tests {
    [TestFixture]
    public class CommandTest {
        SpriteDocument document;

        [SetUp]
        public void SetUp() {
            document = new SpriteDocument();
        }

        [Test]
        public void TestInsertNewFrame() {
            Bitmap newFrame = new Bitmap(document.Size.Width, document.Size.Height);
            Command cmd = new InsertFrameCommand(0, newFrame);

            cmd.Do(document);
            Assert.AreEqual(1, document.Frames.Count);

            // While we're at it, assert the RIGHT frame is in there
            Assert.AreSame(newFrame, document.Frames[0]);

            cmd.Undo(document);
            Assert.AreEqual(0, document.Frames.Count);
        }

        [Test]
        public void TestDeleteFrame() {
            const int COUNT = 4;
            Bitmap[] frame = new Bitmap[COUNT];

            for (int i = COUNT - 1; i >= 0; i--) {
                frame[i] = new Bitmap(document.Size.Width, document.Size.Height);
                document.SendCommand(new InsertFrameCommand(0, frame[i]));
            }

            document.SendCommand(new DeleteFrameCommand(2));
            Assert.AreSame(document.Frames[1], frame[1]);
            Assert.AreSame(document.Frames[2], frame[3]);

            document.Undo();
            Assert.AreSame(document.Frames[1], frame[1]);
            Assert.AreSame(document.Frames[2], frame[2]);
            Assert.AreSame(document.Frames[3], frame[3]);
        }

        [Test]
        public void TestReplaceFrame() {
            Bitmap frame1 = new Bitmap(document.Size.Width, document.Size.Height);
            Bitmap frame2 = new Bitmap(document.Size.Width, document.Size.Height);

            document.SendCommand(new InsertFrameCommand(0, frame1));
            Assert.AreSame(frame1, document.Frames[0]);
            document.SendCommand(new ReplaceFrameCommand(0, frame2));
            Assert.AreSame(frame2, document.Frames[0]);
            document.Undo();
            Assert.AreSame(frame1, document.Frames[0]);
        }
        [Test]
        public void TestChangeHotSpot() {
            Rectangle hot0 = document.HotSpot;
            Rectangle hot1 = Rectangle.FromLTRB(0, 0, 16, 16);
            Rectangle hot2 = Rectangle.FromLTRB(0, 8, 16, 24);

            document.SendCommand(new ChangeHotSpotCommand(hot1));
            Assert.AreEqual(hot1, document.HotSpot);

            document.SendCommand(new ChangeHotSpotCommand(hot2));
            Assert.AreEqual(hot2, document.HotSpot);

            document.Undo();
            Assert.AreEqual(hot1, document.HotSpot);

            document.Undo();
            Assert.AreEqual(hot0, document.HotSpot);
        }

        [Test]
        public void TestUpdateAnimScript() {
            const string SCRIPT_NAME = "walk-right";
            const string SCRIPT_VALUE = "F0 W10 F1 W10";

            Assert.IsFalse(document.AnimScripts.ContainsKey(SCRIPT_NAME));
            
            document.SendCommand(new UpdateAnimScriptCommand(SCRIPT_NAME, SCRIPT_VALUE));
            Assert.IsTrue(document.AnimScripts.ContainsKey(SCRIPT_NAME));
            Assert.AreEqual(SCRIPT_VALUE, document.AnimScripts[SCRIPT_NAME]);

            document.Undo();
            Assert.IsFalse(document.AnimScripts.ContainsKey(SCRIPT_NAME));
        }

        [Test]
        public void TestDeleteAnimScript() {
            Assert.IsFalse(document.AnimScripts.ContainsKey("test"));
            Assert.IsFalse(document.AnimScripts.ContainsKey("test2"));
            Assert.IsFalse(document.AnimScripts.ContainsKey("test3"));

            document.SendCommand(new UpdateAnimScriptCommand("test", "value1"));
            document.SendCommand(new UpdateAnimScriptCommand("test2", "value2"));
            document.SendCommand(new UpdateAnimScriptCommand("test3", "value3"));

            document.SendCommand(new DeleteAnimScriptCommand(new string[] { "test", "test2" }));
            Assert.IsFalse(document.AnimScripts.ContainsKey("test"));
            Assert.IsFalse(document.AnimScripts.ContainsKey("test2"));
            Assert.IsTrue(document.AnimScripts.ContainsKey("test3"));

            document.Undo();
            Assert.IsTrue(document.AnimScripts.ContainsKey("test"));
            Assert.IsTrue(document.AnimScripts.ContainsKey("test2"));
            Assert.IsTrue(document.AnimScripts.ContainsKey("test3"));
        }

        [Test]
        public void TestUpdateMedadata() {
            const string DATUM_NAME = "author";
            const string DATUM_VALUE = "andy";

            Assert.IsFalse(document.Metadata.ContainsKey(DATUM_NAME));
            
            document.SendCommand(new UpdateMetadataCommand(DATUM_NAME, DATUM_VALUE));
            Assert.IsTrue(document.Metadata.ContainsKey(DATUM_NAME));
            Assert.AreEqual(DATUM_VALUE, document.Metadata[DATUM_NAME]);

            document.Undo();
            Assert.IsFalse(document.Metadata.ContainsKey(DATUM_NAME));
        }

        [Test]
        public void TestDeleteMetadata() {
            Assert.IsFalse(document.Metadata.ContainsKey("test"));
            Assert.IsFalse(document.Metadata.ContainsKey("test2"));
            Assert.IsFalse(document.Metadata.ContainsKey("test3"));

            document.SendCommand(new UpdateMetadataCommand("test", "value1"));
            document.SendCommand(new UpdateMetadataCommand("test2", "value2"));
            document.SendCommand(new UpdateMetadataCommand("test3", "value3"));

            document.SendCommand(new DeleteMetadataCommand(new string[] { "test", "test2" }));
            Assert.IsFalse(document.Metadata.ContainsKey("test"));
            Assert.IsFalse(document.Metadata.ContainsKey("test2"));
            Assert.IsTrue(document.Metadata.ContainsKey("test3"));

            document.Undo();
            Assert.IsTrue(document.Metadata.ContainsKey("test"));
            Assert.IsTrue(document.Metadata.ContainsKey("test2"));
            Assert.IsTrue(document.Metadata.ContainsKey("test3"));
        }

        [Test]
        public void TestResizeSprite() {
            Size oldSize = document.Size;

            document.SendCommand(new ResizeSpriteCommand(new Size(oldSize.Width, oldSize.Height * 2)));
            Assert.AreEqual(oldSize.Height * 2, document.Size.Height);
            
            Assert.IsTrue(document.CanUndo());
            document.Undo();
            Assert.AreEqual(oldSize.Height, document.Size.Height);
        }
    }
}
