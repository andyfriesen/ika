// Templates would be nice.  This and ResourceController.cs are as close as we'll get with .NET. :\
// At least these classes are simple.
namespace rho
{
    using ika=Import.ika;

    class TileSetController : ResourceController
    {
        public override object Load(string fname)
        {
            return ika.TileSet.Load(fname);
        }
    }

}