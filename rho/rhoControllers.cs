#if false
// Templates would be nice.  This and ResourceController.cs are as close as we'll get with .NET. :\
// At least these classes are simple.
namespace rho
{
    using ika = Import.ika;

    class TilesetController : ResourceController
    {
        public override object Load(string fname)
        {
            return ika.Tileset.Load(fname);
        }
    }

}
#endif