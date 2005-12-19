
#pragma once

#include "common/utility.h"
#include "types.h"
#include "matrix.h"

#include <string>
#include <map>

// Partial tile obstruction stuff.
enum {
    ob_none             = 0x00,
    ob_top              = 0x01,
    ob_bottom           = 0x02,
    ob_left             = 0x04,
    ob_right            = 0x08,
    // diagonals
    ob_upleft_downright = 0x10,
    ob_upright_downleft = 0x20,

    ob_all = ob_top | ob_bottom | ob_left | ob_right,
};

struct Map {

    struct Zone {
        std::string label;
        std::string scriptName;
    };

    struct Entity {
        int x, y;

        std::string label;
        std::string spriteName;
        Direction direction;
        int speed;
        std::string moveScript;
        bool obstructsEntities;
        bool obstructedByEntities;
        bool obstructedByMap;
        std::string adjActivateScript;  // called when the entity touches another entity
        std::string activateScript;     // called when the entity is activated. (talked to, etc)
        Entity()
            : direction(face_up)
            , speed(100)
            , obstructedByEntities(true)
            , obstructedByMap(true)
            , obstructsEntities(true) {}
    };

    struct WayPoint {
        int x, y;
        std::string label;
        WayPoint()
            : x(0)
            , y(0) 
        {}
    };

    struct Layer {
        struct Zone {
            Rect position;
            std::string label;
        };

        struct ParallaxInfo {
            int mulx, muly;
            int divx, divy;

            ParallaxInfo()
                : mulx(1)   , muly(1)
                , divx(1)   , divy(1)
            {}
        };

        std::string label;
        int x, y;
        ParallaxInfo parallax;

        bool wrapx;
        bool wrapy;

        double scaleX;
        double scaleY;

        std::vector<Entity> entities;
        std::vector<Zone> zones;

        RGBA tintColour;

        // TODO:
        // std::vector<LineSegments> obstructionVectors;

        Matrix<u8> obstructions;
        Matrix<uint> tiles;

        Layer(const std::string& l = "", uint width = 0, uint height = 0)
            : label(l)
            , x(0)          , y(0)
            , wrapx(false)  , wrapy(false)
            , scaleX(1.0)   , scaleY(1.0)
            , tiles(width, height)
            , obstructions(width, height)
            , tintColour(255, 255, 255)
        {}

        Layer(const Layer& l)
            : label(l.label)
            , x(l.x)        , y(l.y)
            , wrapx(l.wrapx), wrapy(l.wrapy)
            , scaleX(l.scaleX), scaleY(l.scaleY)
            , tiles(l.tiles)
            , obstructions(l.obstructions)
            , tintColour(255, 255, 255)
        {}

        // note to self: copy constructor

        uint Width() const  { return tiles.Width();  }
        uint Height() const { return tiles.Height(); }
        void Resize(uint newx, uint newy) {
            tiles.Resize(newx, newy);
            obstructions.Resize(newx, newy);
        }
    };

    uint width, height;
    std::string tilesetName;
    //std::vector<std::string> tilesetNames;

    typedef std::map<std::string, Zone> ZoneMap;
    typedef std::map<std::string, WayPoint> WayPointMap;

    ZoneMap             zones;
    WayPointMap         wayPoints;
private:
    std::vector<Layer*> layers;
public:

    std::string title;
    std::map<std::string, std::string> metaData;

    Map();
    Map(const std::string& filename);
    ~Map();

    // bool to make iked's template controller thing happy.  always returns
    // true.  Throws an exception if something went wrong.
    bool Load(const std::string& filename);
    void Save(const std::string& filename);

    Layer* GetLayer(uint index);
    uint LayerIndex(Layer* lay) const;
    uint LayerIndex(const std::string& label) const;

    Map::Layer* AddLayer(const std::string& label = "", uint width = 0, uint height = 0);
    void AddLayer(Layer* lay);                  // Assumes ownership of lay.  Careful.
    void InsertLayer(Layer* lay, uint index);   // Also assumes ownership.  The layer is inserted BEFORE index.
    void DestroyLayer(uint index);
    void DestroyLayer(Layer* lay);
    void SwapLayers(uint i, uint j);
    uint NumLayers() const;
};
