
#pragma once

class VSP;

#include <set>

#pragma managed

#using <mscorlib.dll>
#using <System.Drawing.dll>

namespace Import
{
    namespace ika
    {
        using System::String;
        using System::Drawing::Bitmap;
        using System::Collections::ArrayList;
        using System::Collections::SortedList;
        using System::Reflection::DefaultMemberAttribute;

        [DefaultMember("Item")]
        public __gc class TileSet : public System::IDisposable
        {
            VSP* vsp;

            ArrayList* bitmaps;             // array of bitmaps copied from the underlying CPixelMatrices*/
            std::set<int>* changed;         // YES.  I can do this! :D C# containers suck compared to C++.
                                            // anyway, this is a set of ideces of changed tiles.  When we synch, we clear it.

            void Sync(int idx);             // syncs the specified tile
            void Sync();                    // syncs them all

            TileSet(VSP* v);

        public:

            __gc class AnimStrand
            {
            public:
                int start;
                int end;
                int delay;
            };

            static TileSet* Load(String* filename);  // This can fail, and return 0.

            TileSet();
            ~TileSet();

            void Dispose();

            __property Bitmap* get_Item(int idx);
            __property void    set_Item(int idx,Bitmap* bmp);

            __property int get_Width();
            __property int get_Height();
            __property int get_NumTiles();

            void Save(String* filename);
        };
    };
};