
#include "map.h"
#include "ikaMap.h"
#include "Strings.h"

namespace Import
{
    namespace ika
    {
        Map* Map::Load(String* filename)
        {
            const char* c=c_str(filename);

            ::Map* m=new ::Map();
            if (!m->Load(c))
            {
                delete m;
                m=0;
                return 0;
            }
            return new Map(m);
        }

        Map::Map(::Map* m) : map(m)
        {}

        Map::Map(int x,int y)
        {
            map=new ::Map();
            map->Resize(x,y);
        }

        Map::~Map()
        {
            delete map;
        }

        String* Map::get_RenderString()
        {
            return net_str(map->GetRString().c_str());
        }
        void Map::set_RenderString(String* val)
        {
            map->SetRString(c_str(val));
        }

        String* Map::get_Music()
        {
            return net_str(map->GetMusic().c_str());
        }

        void Map::set_Music(String* s)
        {
            map->SetMusic(c_str(s));
        }

        String* Map::get_TileSetName()
        {
            return net_str(map->GetVSPName().c_str());
        }

        void Map::set_TileSetName(String* s)
        {
            map->SetVSPName(c_str(s));
        }

        int Map::get_NumLayers()
        {
            return map->NumLayers();
        }

        int Map::get_NumEntities()
        {
            return map->NumEnts();
        }

        int Map::get_Width()
        {
            return map->Width();
        }

        int Map::get_Height()
        {
            return map->Height();
        }

        int Map::get_Tile(int x,int y,int lay)
        {
            if (lay<0 || lay>=NumLayers)
                return 0;

            return (int)map->GetTile(x,y,lay);
        }

        void Map::set_Tile(int x,int y,int lay,int idx)
        {
            map->SetTile(x,y,lay,(u32)idx);
        }

        bool Map::get_Obs(int x,int y)
        {
            return map->IsObs(x,y);
        }

        void Map::set_Obs(int x,int y,bool b)
        {
            map->SetObs(x,y,b);
        }

        int Map::get_Zone(int x,int y)
        {
            return map->GetZone(x,y);
        }

        void Map::set_Zone(int x,int y,int z)
        {
            map->SetZone(x,y,z);
        }

        LayerInfo* Map::get_LayerInfo(int idx)
        {
            ::Import::ika::LayerInfo* li=new ::Import::ika::LayerInfo;
            ::SMapLayerInfo l;
            map->GetLayerInfo(l,idx);

            li->pmulx=l.pmulx;
            li->pdivx=l.pdivx;
            li->pmuly=l.pmuly;
            li->pdivy=l.pdivy;

            return li;
        }

        void Map::set_LayerInfo(int idx,::Import::ika::LayerInfo* li)
        {
            ::SMapLayerInfo l;

            l.pmulx=li->pmulx;
            l.pmuly=li->pmuly;
            l.pdivx=li->pdivx;
            l.pdivy=li->pdivy;

            map->SetLayerInfo(l,idx);
        }

        ::Import::ika::ZoneInfo* Map::get_ZoneInfo(int idx)
        {
            ::Import::ika::ZoneInfo* zi=new ::Import::ika::ZoneInfo;
            ::SMapZone z;
            map->GetZoneInfo(z,idx);

            zi->name=z.sName.c_str();
            zi->description=z.sDescription.c_str();
            zi->actscript=z.sActscript.c_str();
            zi->actchance=z.nActchance;
            zi->actdelay=z.nActdelay;
            zi->adjacentactivation=z.bAdjacentactivation;   // I would have chosen a shorter name, but Visual Studio completes it for me anyway. >:D
            zi->entactscript=z.sEntactscript.c_str();
            return zi;
        }

        void Map::set_ZoneInfo(int idx,::Import::ika::ZoneInfo* zi)
        {
            ::SMapZone z;
            z.sName=c_str(zi->name);
            z.sDescription=c_str(zi->description);
            z.sActscript=c_str(zi->actscript);
            z.nActchance=zi->actchance;
            z.nActdelay=zi->actdelay;
            z.bAdjacentactivation=zi->adjacentactivation;
            z.sEntactscript=c_str(zi->entactscript);

            map->SetZoneInfo(z,idx);
        }

        String* Map::get_MoveScript(int idx)
        {
            return net_str(map->GetMScript(idx));
        }

        void Map::set_MoveScript(int idx,String* s)
        {
            map->SetMScript(c_str(s),idx);
        }

        EntityInfo* Map::get_Entity(int idx)
        {
            ::Import::ika::EntityInfo* ei=new ::Import::ika::EntityInfo;
            ::SMapEntity ent=map->GetEntity(idx);

            // VS.NET has some nifty advanced editing features.
            // I didn't have to type this out manually. ;)
            // Some regex and clever copy/paste.
            ei->name=ent.sName.c_str();
            ei->description=ent.sDescription.c_str();
            ei->x=ent.x;
            ei->y=ent.y;
            ei->direction=ent.direction;
            ei->speed=ent.nSpeed;
            ei->mapobs=ent.bMapobs;
            ei->entobs=ent.bEntobs;
            ei->isobs=ent.bIsobs;
            ei->CHRname=ent.sCHRname.c_str();
            ei->adjacentactivation=ent.bAdjacentactivation;
            ei->actscript=ent.sActscript.c_str();
            ei->state=(::Import::ika::MoveCode)ent.state;
            ei->movescript=ent.sMovescript.c_str();
            ei->wandersteps=ent.nWandersteps,
            ei->wanderdelay=ent.nWanderdelay;
            ei->wanderrect=Rectangle(
                ent.wanderrect.left,
                ent.wanderrect.top,
                ent.wanderrect.Width(),
                ent.wanderrect.Height()
                );
            ei->zone=ent.sZone.c_str();
            ei->chasetarget=ent.sChasetarget.c_str();
            ei->chasedist=ent.nChasedist;

            return ei;
        }

        void Map::set_Entity(int idx,EntityInfo* ei)
        {
            ::SMapEntity ent;
            ent.sName=c_str(ei->name);
            ent.sDescription=c_str(ei->description);
            ent.x=ei->x;
            ent.y=ei->y;
            ent.direction=ei->direction;
            ent.nSpeed=ei->speed;
            ent.bMapobs=ei->mapobs;
            ent.bEntobs=ei->entobs;
            ent.bIsobs=ei->isobs;
            ent.sCHRname=c_str(ei->CHRname);
            ent.bAdjacentactivation=ei->adjacentactivation;
            ent.sActscript=c_str(ei->actscript);
            ent.state=(::MoveCode)ent.state;
            ent.sMovescript=c_str(ei->movescript);
            ent.nWandersteps=ei->wandersteps,
            ent.nWanderdelay=ei->wanderdelay;
            ent.wanderrect=::Rect(
                ei->wanderrect.Left,
                ei->wanderrect.Top,
                ei->wanderrect.Right,
                ei->wanderrect.Bottom
                );
            ent.sZone=c_str(ei->zone);
            ent.sChasetarget=c_str(ei->chasetarget);
            ent.nChasedist=ei->chasedist;
        }

        EntityInfo* Map::CreateEntity(int& idx)
        {
            idx=map->CreateEntity(0,0);
            return get_Entity(idx);
        }

        void Map::DestroyEntity(int idx)
        {
            map->DestroyEntity(idx);
        }

        int Map::EntityAt(int x,int y)
        {
            return map->EntityAt(x,y);
        }

        int Map::AddLayer()
        {
            map->AddLayer(map->NumLayers()-1);
            return map->NumLayers()-1;
        }

        void Map::RemoveLayer(int idx)
        {
            map->DeleteLayer(idx);
        }

        Array* Map::Copy(int x,int y,int width,int height,int layer)
        {
            throw new System::Exception(S"Not yet implemented!");

            return 0;
        }

        void Map::Paste(Array* src,int x,int y,int layer)
        {
            throw new System::Exception(S"Not yet implemented!");
        }
    };
};