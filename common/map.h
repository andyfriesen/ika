/*    map.h
    Header crap for map.cpp
    Written by tSB
    Copyright me and stuff, so ph33r.

    Some of this stuff is vecna's.  ph33r him too.
  
    Some of these structs are purely vestigal, for the importing of VERGE maps.
*/

#ifndef MAP_H
#define MAP_H

#include "types.h"
#include "fileio.h"
#include "rle.h"

// Legacy structs for loading v2 stuff
struct layer_r
{
    u8              pmultx,pdivx;                   // parallax multiplier/divisor for X
    u8              pmulty,pdivy;                   // parallax multiplier/divisor for Y
    u16             sizex, sizey;                   // layer dimensions.
    u8              trans;                          // transparency flag
    u8              hline;                          // hline (raster fx) (never actually implemented)
    char            filler[2];                      // make it 12 bytes, so bleh
};

struct zoneinfo_r
{
    char            name[40];                       // zone name/desc
    u16             script;                         // script to call thingy
    u16             percent;                        // chance of executing
    u16             delay;                          // step-delay
    u16             aaa;                            // Accept Adjacent Activation
    u16             entityscript;                   // script to call for entities
};

struct Sv2entity
{
    int             x, y;                           // x/y pixel position
    u16             tx, ty;                         // x/y tile position
    u8              facing;                         // direction entity is facing
    u8              moving, movecnt;                // direction entity is moving
    u8              frame;                          // bottom-line frame to display
    u8              specframe;                      // if nonzero, this overrides move scripts
    u8              chrindex, reset;                // CHR index | Reset animation
    u8              bCanobs,bIsobs;                 // can be obstructed | Is an obstruction
    u8              speed, speedct;                 // entity speed, speedcount :)
    u8              delayct;                        // animation frame-delay
    char            *animofs, *moveofs;             // anim script | move script
    u8              face, actm;                     // auto-face | activation mode
    u8              movecode, movescript;           // movement type | movement script
    u8              ctr, mode;                      // sub-tile move ctr, mode flag (internal)
    u16             step, delay;                    // step, delay
    u16             stepctr, delayctr;              // internal use counters
    u16             data1, data2, data3;            // misc junk :P
    u16             data4, data5, data6;            //
    int             actscript;                      // activation script
    int             expand1, expand2;               // STOOPID
    int             expand3, expand4;               // DUM DUM DUM
    char            desc[20];                       // Entity description. (ika interprets this field as the entity's name)
};

struct chrlist_r
{
    char            t[60];
};

struct movescript_r
{
    char            t[200];
};

// New stuff
struct SMapZone
{
    string_k        sName;                          // purely for the developer's benefit (for now)
    string_k        sDescription;                   // purely for the developer's benefit
    string_k        sActscript;                     // name of script to execute when teh zone is activated
    int             nActchance;                     // probability (out of 100) that the zone will activate when stepped upon
    int             nActdelay;                      // Number of steps before the actscript gets run
    bool            bAdjacentactivation;            // if true, the player entity has to activate the zone directly
    string_k        sEntactscript;                  // called if nonplayer entities step on the zone
    
    SMapZone() 
        : nActchance(0),nActdelay(0),
          bAdjacentactivation(false)
    {}
};

struct SMapEntity
{
    string_k        sName;                          // entity's name
    string_k        sDescription;                   // description (purely for the developer's use)
    int             x,y;                            // position
    int             direction;                      // the direction the entity is initially facing
    int             nSpeed;                         // speed, in pixels per second (100 is normal)
    bool            bMapobs;                        // if true, the entity is obstructed by the map
    bool            bEntobs;                        // if true, the entity is obstructed by other entities
    bool            bIsobs;                         // if true, the entity can block other entities
    string_k        sCHRname;                       // name of the spriteset to use
    bool            bAdjacentactivation;            // if true, the entity activates itself when it bumps into the player
    string_k        sActscript;                     // script to run when the entity is activated
    MoveCode        state;                          // what the entity is initially doing
    string_k        sMovescript;                    // initial move script (if state==mc_script)
    int             nWandersteps,nWanderdelay;      // wander stuff (if state==mc_wander*)
    Rect            wanderrect;                     // Rect the entity is restricted to (if state==mc_wanderrect)
    string_k        sZone;                          // name of zone to wander on if state==mc_wanderzone
    string_k        sChasetarget;                   // the entity that this entity wants to be close to (c'est amore!) if state==mc_chase
    int             nChasedist;                     // how close (if state==mc_chase)
    
    SMapEntity()    :
        x(0),y(0),direction(0),nSpeed(100),
        bMapobs(true),bEntobs(true),bIsobs(true),
        bAdjacentactivation(false),state(mc_nothing),
        nWandersteps(0),nWanderdelay(0),nChasedist(0)
    {
        wanderrect=Rect(0,0,0,0);
    }
};

struct SMapLayerInfo
{
    int            pmulx,pdivx;
    int            pmuly,pdivy;
    int            nTransmode;                      // for v2 maps

    SMapLayerInfo() {}
    SMapLayerInfo(const layer_r& l)
    :   pmulx(l.pmultx),
        pmuly(l.pmulty),
        pdivx(l.pdivx),
        pdivy(l.pdivy),
        nTransmode(l.trans)
    {}
};

// blah

class MapClip                                       // a chunk of map (that isn't part of a map itself)  Used for copy/paste operations
{
    friend class Map;
private:
    int             nLayers;
    std::vector<u32*> pData;                        // info on any tile layers we've grabbed up
    std::vector<bool> bUsed;                        // true if the specified layer has valid tile data
    char*           pObstruct;                      // obstruction data
    char*           pZone;                          // zone data
    
    int             nWidth,nHeight;
    
    bool            bObstruct;                      // true if we've grabbed from the obstruction layer
    bool            bZone;                          // true if we've grabbed from the zone layer
    
    bool            bMegaclip;                      // true if there's more than one layer in here
    
    void Free();
    
public:
    MapClip();
    ~MapClip();
    
    inline int Width() const { return nWidth; }
    inline int Height() const { return nHeight; }
};

class Map
{
private:   
    std::vector<SMapLayerInfo>    info;
    std::vector<u32*>        pData;
    
    int             nLayers;
    int             nWidth,nHeight;                 // map dimensions, in tiles
    
    u8*             pObstruct;
    u32*            pZone;
    
    string_k        sVSPname;
    string_k        sMusicname;
    string_k        sRenderstring;
    
    std::vector<SMapZone> zoneinfo;
    std::vector<SMapEntity> entity;
    
    int             nStartx,nStarty;                // unused (TODO: expose these to the scripter)
    bool            bWrap;                          // unused for now
    
    void Paste(MapClip &mc,int xs,int ys,int sourcelayer,int destlayer,bool transparent);
    
    bool Importv2Map(File& f);                              // Loads an old v2 VSP.
public:
    Map();
    Map(const char* fname);
    ~Map();
    
    u32  GetTile(int x,int y,int layer);                    // returns the tile at the specified layer
    void SetTile(int x,int y,int layer,u32 tile);           // sets the tile at the specified layer
    u32* GetDataPtr(int layer);                             // returns a pointer to the raw tile data (BE CAREFUL WITH THIS!  YOU CAN BREAK THINGS!)
    
    void Free();                                            // cleanup
    void New();                                             // creates a new 100x100, 1 layer map
    bool Load(const char *fname);                           // loads the specified .MAP file.
    bool Save(const char *fname);                           // saves it (der)
    bool Exportv2Map(const char* fname);
    
    // layer manipulation
    void Resize(int newx,int newy);                         // resizes all layers and all that
    int  NumLayers() { return nLayers; }                    // returns the number of layers the map uses
    void AddLayer(int pos);                                 // inserts a layer before pos
    void DeleteLayer(int pos);                              // deletes the pos-th layer
    void GetLayerInfo(SMapLayerInfo& nfo,int layidx);       // fills nfo with information about the specified layer
    void SetLayerInfo(const SMapLayerInfo&  nfo,int layidx);// sets the properties of the specified layer according to nfo
    
    // General map properties
    const string_k& GetRString(void);                       // returns the render string
    void            SetRString(const string_k& s);          // sets the render string
    const string_k& GetMusic(void);                         // returns the music played on this map
    void            SetMusic(const string_k& s);            // sets the music file used
    const string_k& GetVSPName(void);                       // returns the name of the VSP used on this map
    void            SetVSPName(const string_k& s);          // sets the VSP file used
    
    // CHR list
    int      CountCHRs(void);
    const char* GetCHREntry(int idx);                       // returns what's in the specified slot
    void  SetCHREntry(const char* newchr,int idx);          // sets the specified slot
    
    // move scripts
    int   CountMoveScripts(void);
    const char* GetMScript(int idx);                        // returns the movement script in the specified slot
    void  SetMScript(const char* newscript,int idx);        // sets a movement script
    
    void SetObs(int x,int y,bool mode);                     // either sets or unsets the obstruction value at the given tile coords
    bool IsObs(int x,int y);                                // returns true if the tile is obstructed
    
    // zone altering functions
    int  CountUsedZones(void);
    void SetZone(int x,int y,int z);                        // Sets the zone at the specified tile position
    int  GetZone(int x,int y);                              // returns the zone at the specified tile position
    void GetZoneInfo(SMapZone& info, int zonenum);          // fills info with information about the specified zone
    void SetZoneInfo(const SMapZone& newdat,int zonenum);   // sets the specified zone's properties based on newdat
    inline int NumZones() const                             // returns the number of used zones
    { return zoneinfo.size(); }
    
    int  EntityAt(int x,int y);                             // returns the index of the entity at the specified coords, -1 if no entity is there
    void GetEntInfo(SMapEntity& info,int entidx);           // fills info with information about the specified entity
    void SetEntInfo(const SMapEntity& info,int entidx);     // sets the specified entity's properties based on the contents of info
    int  CreateEntity(int x,int y);                         // creates a new entity at (x,y) and returns its index.
    void DestroyEntity(int entidx);                         // destroys the specified entity
    int  NumEnts()    const;                                // returns the number of entities on the map
    
    int  Width() { return nWidth; }                         // returns the width of the map (in tiles)
    int  Height() { return nHeight; }                       // returns the hieght of the map (in tiles)
    
    void Copy(MapClip& mc,Rect r,int layer);                // copies part of one layer to the clip
    void Copy(MapClip& mc,Rect r);                          // copies a chunk from all visible layers onto the map clip
    void Paste(MapClip& mc,int x,int y,int destlayer);      // mega-copier.
};

#endif
