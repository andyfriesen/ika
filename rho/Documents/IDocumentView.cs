
namespace rho.Documents {
    interface IDocumentView {
        string FileName { get; set; }

        void Save();
        void Save(string filename);
    }
}