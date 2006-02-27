using System;
using WeifenLuo.WinFormsUI;

namespace rho.Documents {
    public interface DocumentFactory {
        Document NewFile();
        Document OpenFile(string fileName);

        DockContent OpenWindow(Document doc);

        string DocumentName { get; }
        string DocumentFilter { get; }
        Type DocumentType { get; }
    }
}
