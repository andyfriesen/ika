
#pragma once

class Map;

#pragma managed

#using <mscorlib.dll>
#using <System.Drawing.dll>

using System::Drawing::Rectangle;
using System::Array;
using System::String;

namespace Import
{
    namespace ika
    {
        public __value enum MoveCode
        {
            Nothing=0,										// entity just stands there
            Wander,											// entity wanders around like an idiot
            WanderRect,										// ditto, but the entity is restricted to a rectangular region
            WanderZone,										// ditto again, but the entity is restricted to a particular zone
            Script,											// entity follows its script
            Chase,											// the entity is following another entity
        };

        public __gc class LayerInfo
        {
        public:
            int pmulx,pmuly;
            int pdivx,pdivy;
        };

        public __gc class ZoneInfo
        {
        public:
            String*       name;                                     // purely for the developer's benefit (for now)
            String*       description;                              // purely for the developer's benefit
            String*       actscript;                                // name of script to execute when teh zone is activated
            int           actchance;                                // probability (out of 100) that the zone will activate when stepped upon
            int           actdelay;                                 // Number of steps before the actscript gets run
            bool          adjacentactivation;                       // if true, the player entity has to activate the zone directly
            String*       entactscript;                             // called if nonplayer entities step on the zone
        };

        public __gc class EntityInfo
        {
        public:
            String*     name;                                      // entity's name
            String*     description;                               // description (purely for the developer's use)
            int         x,y;                                        // position
            int         direction;                                  // the direction the entity is initially facing
            int         speed;                                     // speed, in pixels per second (100 is normal)
            bool        mapobs;                                    // if true, the entity is obstructed by the map
            bool        entobs;                                    // if true, the entity is obstructed by other entities
            bool        isobs;                                     // if true, the entity can block other entities
            String*     CHRname;                                   // name of the spriteset to use
            bool        adjacentactivation;                        // if true, the entity activates itself when it bumps into the player
            String*     actscript;                                 // script to run when the entity is activated
            MoveCode    state;                                      // what the entity is initially doing
            String*     movescript;                                // initial move script (if state==mc_script)
            int         wandersteps,wanderdelay;                  // wander stuff (if state==mc_wander*)
            Rectangle   wanderrect;                                 // Rect the entity is restricted to (if state==mc_wanderrect)
            String*     zone;                                      // name of zone to wander on if state==mc_wanderzone
            String*     chasetarget;                               // the entity that this entity wants to be close to (c'est amore!) if state==mc_chase
            int         chasedist;                                 // how close (if state==mc_chase)
        };

        public __gc class Map;

        public __delegate void LayerChangedHandler(Map* m,int lay,Rectangle r);
        public __delegate void EntityChangedHandler(Map* m,int idx);
        public __delegate void MapChangedHandler(Map* m);

        public __gc class Map
        {
            ::Map* map;

        public:
            static Map* Load(String* filename);  // this can fail and return null, so it's not just a constructor
        private:
            Map(::Map* m);

        public:
            
            Map(int x,int y);
            ~Map();

            void Save(String* fname);

#define PROPERTY(name,type)                 \
    __property type get_##name();           \
    __property void set_##name(type val);

            PROPERTY(RenderString,String*)
            PROPERTY(Music,String*)
            PROPERTY(TileSetName,String*)

#undef PROPERTY

            __property int get_NumLayers();
            __property int get_NumEntities();
            __property int get_Width();
            __property int get_Height();

            __property int  get_Tile(int x,int y,int lay);
            __property void set_Tile(int x,int y,int lay,int idx);

            __property bool get_Obs(int x,int y);
            __property void set_Obs(int x,int y,bool b);

            __property int  get_Zone(int x,int y);
            __property void set_Zone(int x,int y,int z);

            __property ::Import::ika::LayerInfo* get_LayerInfo(int idx);
            __property void set_LayerInfo(int idx,::Import::ika::LayerInfo* li);

            __property ::Import::ika::ZoneInfo* get_ZoneInfo(int idx);
            __property void set_ZoneInfo(int idx,::Import::ika::ZoneInfo* zi);

            __property String* get_MoveScript(int idx);
            __property void set_MoveScript(int idx,String* s);

            __property EntityInfo* get_Entity(int idx);
            __property void set_Entity(int idx,EntityInfo* ei);

            EntityInfo* CreateEntity(int& idx);         // idx is assigned to the index of the new entity.  The return value is the entity's properties.
            void DestroyEntity(int idx);
            int  EntityAt(int x,int y);                 // returns the index of the entity under the coordinates specified, or -1 if there isn't one there.

            int AddLayer();
            void RemoveLayer(int idx);

            // I'm returning an Array* and not an int __gc[,] because of a bug in the VC++.NET compiler.  Die MS, die.
            Array* Copy(int x,int y,int width,int height,int layer);    // copies tiles from the specified layer, and returns an int[,] containing them.
            void Paste(Array* src,int x,int y,int layer);               // pastes tiles from the passed int[,], on the specified layer, at the specified position

            __event LayerChangedHandler*    LayerChanged;
            __event EntityChangedHandler*   EntityChanged;
            __event MapChangedHandler*      PropertiesChanged;
        };
    };
};