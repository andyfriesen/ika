
#include "VSP.h"
#include "ikaTileSet.h"
#include "Strings.h"

namespace Import
{
    namespace ika
    {
        using namespace System;
        using namespace System::Drawing;
        using namespace System::Drawing::Imaging;

        TileSet::TileSet(VSP* v) : vsp(v)
        {
            bitmaps=new ArrayList;
            changed=new std::set<int>;
        }

        TileSet* TileSet::Load(String* fname)
        {
            VSP* v=new VSP;
            if (!v->Load(c_str(fname)))
            {
                delete v;
                return 0;
            }

            return new TileSet(v);
        }

        TileSet::TileSet()
        {
            vsp=new VSP;
            bitmaps=new ArrayList;
            changed=new std::set<int>;
        }

        TileSet::~TileSet() { Dispose(); }

        void TileSet::Dispose()
        {
            delete vsp;
            delete changed;
            vsp=0;
            changed=0;

            for (int i=0; i<bitmaps->Count; i++)
                if (bitmaps->get_Item(i))
                    __try_cast<IDisposable*>(bitmaps->get_Item(i))->Dispose();
        }

        void TileSet::Sync(int idx)
        {
            if (idx>=bitmaps->Count)
                idx=0;

            Bitmap* bmp=__try_cast<Bitmap*>(bitmaps->get_Item(idx));
            if (!bmp)   return;

            if (bmp->Width!=vsp->Width() || bmp->Height!=vsp->Height())
                throw new System::Exception("TileSet::Synch borkage");

            // Get the pixel data for both images
            BitmapData* bd=bmp->LockBits(Rectangle(0,0,bmp->Width,bmp->Height),ImageLockMode::ReadOnly,PixelFormat::Format32bppArgb);

            CPixelMatrix& tile=vsp->GetTile(idx);

            BGRA* src= (BGRA*)bd->Scan0.ToPointer();
            RGBA* dest=(RGBA*)tile.GetPixelData();

            // Then copy the bitmap into the pixel matrix. (taking pixel format differences into account)
            int y=tile.Height();
            while (y--)
            {
                for (int x=0; x<tile.Width(); x++)
                {
                    dest[x].r=src[x].r;
                    dest[x].g=src[x].g;
                    dest[x].b=src[x].b;
                    dest[x].a=src[x].a;
                }
                
                dest+=tile.Width();
                src+=bd->Stride/sizeof(u32);
            }

            // Cleanup the bitmap.  No cleanup needed for CPixelMatrix
            bmp->UnlockBits(bd);

            // Done
        }

        void TileSet::Sync()
        {
            std::set<int>::iterator i;
            for (i=changed->begin(); i!=changed->end(); i++)
                Sync(*i);

            changed->clear();
        }

        Bitmap* TileSet::get_Item(int idx)
        {
            if (idx<0 || idx>=vsp->NumTiles())
                return 0;

            // make sure the ArrayList is long enough
            while (bitmaps->Count<vsp->NumTiles())
                bitmaps->Add(0);

            // Get the existing bitmap if there is one
            Bitmap* bmp=__try_cast<Bitmap*>(bitmaps->get_Item(idx));

            if (!bmp)       // If there isn't one, make it.
            {
                bmp = new Bitmap(vsp->Width(),vsp->Height(),PixelFormat::Format32bppArgb);

                BitmapData* bd=bmp->LockBits(Rectangle(0,0,vsp->Width(),vsp->Height()),ImageLockMode::WriteOnly,PixelFormat::Format32bppArgb);

                BGRA* dest=(BGRA*)bd->Scan0.ToPointer();
                RGBA* src =(RGBA*)vsp->GetTile(idx).GetPixelData();

                int y=vsp->Height();
                while (y--)
                {
                    for (int x=0; x<vsp->Width(); x++)
                        dest[x]=src[x];

                    dest+=bd->Stride/sizeof(u32);
                    src+=vsp->Width();
                }

                bmp->UnlockBits(bd);
                bitmaps->set_Item(idx,bmp);
            }

            return bmp;
        }

        void TileSet::set_Item(int idx,Bitmap* bmp)
        {
            if (bmp->Width!=vsp->Width() || bmp->Height!=vsp->Height())
                throw new System::Exception("TileSet::set_Item : Bitmap passed was of incorrect size");

            bitmaps->set_Item(idx,bmp);
            changed->insert(idx);
        }

        int TileSet::get_Width()
        {
            return vsp->Width();
        }

        int TileSet::get_Height()
        {
            return vsp->Height();
        }

        int TileSet::get_NumTiles()
        {
            return vsp->NumTiles();
        }

        void TileSet::Save(String* filename)
        {
            const char* fname=c_str(filename);

            Sync();

            vsp->Save(fname);
        }
    };
};