
namespace rho
{
    interface IDocView
    {
        string FileName {   get;    set;    }
        void   Save();
        void   Save(string filename);
    }
}