
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

        TileSet::~TileSet()
        {
            delete changed;
        }

        void TileSet::Sync(int idx)
        {
            Bitmap* bmp=__try_cast<Bitmap*>(bitmaps->get_Item(idx));
            if (!bmp)   return;

            if (bmp->Width!=vsp->Width() || bmp->Height!=vsp->Height())
                throw new System::Exception("TileSet::Synch borkage");

            // Get the pixel data for both images
            BitmapData* bd=bmp->LockBits(Rectangle(0,0,bmp->Width,bmp->Height),ImageLockMode::ReadOnly,PixelFormat::Format32bppArgb);

            CPixelMatrix& tile=vsp->GetTile(idx);

            u32* src= (u32*)bd->Scan0.ToPointer();
            u32* dest=(u32*)tile.GetPixelData();

            // Then copy the bitmap into the pixel matrix.
            int y=tile.Height();
            while (y--)
            {
                memcpy(dest,src,tile.Width()*sizeof(u32));
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
            // Get the existing bitmap if there is one
            Bitmap* bmp=__try_cast<Bitmap*>(bitmaps->get_Item(idx));

            if (!bmp)       // If there isn't one, make it.
            {
                bmp = new Bitmap(vsp->Width(),vsp->Height(),PixelFormat::Format32bppArgb);

                BitmapData* bd=bmp->LockBits(Rectangle(0,0,vsp->Width(),vsp->Height()),ImageLockMode::WriteOnly,PixelFormat::Format32bppArgb);

                u32* dest=(u32*)bd->Scan0.ToPointer();
                u32* src=(u32*)vsp->GetTile(idx).GetPixelData();

                int y=vsp->Height();
                while (y--)
                {
                    memcpy(dest,src,vsp->Width()*sizeof(u32));
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

        void TileSet::Save(String* filename)
        {
            const char* fname=c_str(filename);

            Sync();

            vsp->Save(fname);
        }
    };
};