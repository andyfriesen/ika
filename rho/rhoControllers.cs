// Templates would be nice.  This and ResourceController.cs are as close as we'll get with .NET. :\
// At least these classes are simple.
namespace rho
{
    using Import.ika;

    class TileSetController : ResourceController
    {
        public override object Load(string fname)
        {
            return TileSet.Load(fname);
        }
    }

    class MapController : ResourceController
    {
        public override object Load(string fname)
        {
            return Map.Load(fname);
        }
    }

}