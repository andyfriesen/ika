using System;
using NUnit.Framework;

namespace rho.Documents.Tests {
    [TestFixture]
    public class DocumentTest {
        class TestDocument : AbstractDocument {
            public bool wasSaved = false;
            public bool commandExecuted = false;
            public bool commandUndone = false;

            public TestDocument(string name)
                : base(name) {
                 
            }

            public override void DoSave(string fileName) {
                wasSaved = true;
            }
        }

        class TestCommand : Command {
            public void Do(Document doc) {
                TestDocument d = doc as TestDocument;
                Assert.IsFalse(d.commandExecuted);
                d.commandExecuted = true;
                d.commandUndone = false;
            }

            public void Undo(Document doc) {
                TestDocument d = doc as TestDocument;
                Assert.IsTrue(d.commandExecuted);
                d.commandExecuted = false;
                d.commandUndone = true;
            }
        }

        TestDocument document;

        [SetUp]
        public void SetUp() {
            document = new TestDocument("test");
        }

        [Test]
        public void TestName() {
            Assert.AreEqual("test", document.Name);
        }

        [Test]
        public void TestSendCommand() {
            document.SendCommand(new TestCommand());
            Assert.IsTrue(document.commandExecuted);
        }

        [Test]
        public void TestUndoRedo() {
            document.SendCommand(new TestCommand());
            Assert.IsTrue(document.commandExecuted);
            Assert.IsTrue(document.CanUndo());
            Assert.IsFalse(document.CanRedo());

            document.Undo();
            Assert.IsTrue(document.commandUndone);
            Assert.IsFalse(document.commandExecuted);
            Assert.IsTrue(document.CanRedo());

            document.Redo();
            Assert.IsTrue(document.commandExecuted);
            Assert.IsTrue(document.CanUndo());
            Assert.IsFalse(document.CanRedo());
        }

        [Test]
        public void TestSave() {
            Assert.IsFalse(document.wasSaved);
            document.Save();
            Assert.IsTrue(document.wasSaved);
        }

        [Test, Ignore("NYI")]
        public void TestSaveWithNewName() {
            Assert.IsFalse(document.wasSaved);
            document.Save("saved");
            Assert.IsTrue(document.wasSaved);
            Assert.AreEqual("saved", document.Name);
        }

        class Thingie {
            public bool wasCalled = false;
            public void OnDocumentChange() {
                wasCalled = true;
            }
        }

        [Test]
        public void TestChangedEvent() {
            Thingie thingie = new Thingie();
            document.Changed += new ChangeEventHandler(thingie.OnDocumentChange);

            document.SendCommand(new TestCommand());
            Assert.IsTrue(thingie.wasCalled);
        }
    }
}
