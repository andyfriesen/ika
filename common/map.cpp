
#include <fstream>
#include <stdexcept>

#include "aries.h"
#include "base64.h"
#include "compression.h"
#include "common/log.h"
#include "map.h"
#include "oldbase64.h"
#include "utility.h"

using aries::newNode;
using aries::Node;
using aries::DataNode;
using aries::StringNode;
using aries::DataNodeList;
using aries::NodeList;

namespace {
    const char* dirNames[] = {
        "up",
        "down",
        "left",
        "right",
        "up-left",
        "up-right",
        "down-left",
        "down-right",
        "wtf"
    };
    const int numDirs = sizeof dirNames / sizeof dirNames[0];
};

Map::Map()
    : width(0)
    , height(0)
{}

Map::Map(const std::string& filename) {
    Load(filename);
}

Map::~Map() {
    for (uint i = 0; i < layers.size(); i++) {
        delete layers[i];
    }
}

bool Map::Load(const std::string& filename) {
    struct Local {
        static std::string getStringNode(DataNode* parent, const std::string& name) {
            DataNode* n = parent->getChild(name);
            if (!n) {
                throw std::runtime_error(std::string() + "Unable to find node " + name);
            }

            return n->getString();
        }

        static int getIntNode(DataNode* parent, const std::string& name) {
            return atoi(getStringNode(parent, name).c_str());
        }
    };

    // First, clean up:
    for (uint i = 0; i < layers.size(); i++) {
        delete layers[i];
    }
    layers.clear();
    zones.clear();
    wayPoints.clear();


    std::ifstream file(filename.c_str());
    DataNode* rootNode;
    file >> rootNode;
    file.close();

    try {
        DataNode* realRoot = rootNode->getChild("ika-map");

        const std::string ver = realRoot->getChild("version")->getString();
        if (ver == "1.0" || ver == "1.1") {
            Log::Write(
                "Warning: Version of %s is %s.  Expected 1.2.  You should\n"
                "         load and resave the map in the editor to update it.",
                filename.c_str(),
                ver.c_str()
            );
        } else if (ver != "1.2") {
            throw std::runtime_error(va("Invalid map version %s.  Was expecting version 1.0, 1.1, or 1.2", ver.c_str()));
        }

        {
            DataNode* infoNode = realRoot->getChild("information");

            title = Local::getStringNode(infoNode, "title");

            DataNode* metaNode = infoNode->getChild("meta");
            const NodeList& metaNodes = metaNode->getChildren();
            for (NodeList::const_iterator iter = metaNodes.begin(); iter != metaNodes.end(); iter++) {
                if (!(*iter)->isString()) {
                    DataNode* node = reinterpret_cast<DataNode*>(*iter);

                    std::string name = node->getName();
                    std::string value = node->getString();

                    metaData[name] = value;
                }
                // else warn?  String nodes should not be here, though they won't damage anything.
            }
        }

        {
            DataNode* headerNode = realRoot->getChild("header");
            DataNode* dimNode = headerNode->getChild("dimensions");
            width = Local::getIntNode(dimNode, "width");
            height = Local::getIntNode(dimNode, "height");

#if defined(MULTI_TILESETS)
            DataNodeList nodes = headerNode->getChildren("tileset");
            for (DataNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
                tileSetNames.push_back((*iter)->getString());
            }

            if (tileSetNames.empty()) {
                throw std::runtime_error("Map has no tilesets!!");
            }
#else
            DataNode* tileSetNode = headerNode->getChild("tileset");
            tileSetName = tileSetNode->getString();
#endif
        }

        {
            DataNode* zoneNode = realRoot->getChild("zones");

            DataNodeList nodes = zoneNode->getChildren("zone");
            for (DataNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
                Zone z;
                z.label = Local::getStringNode(*iter, "label");
                z.scriptName = Local::getStringNode(*iter, "script");
                zones[z.label] = z;
            }
        }

        {
            DataNode* wpNode = realRoot->getChild("waypoints");

            DataNodeList nodes = wpNode->getChildren("waypoint");
            for (DataNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
                WayPoint wp;
                wp.label = Local::getStringNode(*iter, "label");
                wp.x = Local::getIntNode(*iter, "x");
                wp.y = Local::getIntNode(*iter, "y");
                wayPoints[wp.label] = wp;
            }
        }

        {
            DataNode* layerNode = realRoot->getChild("layers");

            DataNodeList nodes = layerNode->getChildren("layer");
            for (DataNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
                Layer* lay = new Layer();
                DataNode* n;

                lay->label = Local::getStringNode(*iter, "label");

                if (!(*iter)->hasChild("type")) {
                    // Do we change this and forbid a default?  It would break backward compatibility.
                    Log::Write("Warning: layer lacking 'type' node.  Assuming it is a tile layer.");
                } else {
                    // do nothing for now.
                    // I think I want to implement vector layers.  No tiles, just textures
                    // and groups of vertices.  Lots and lots of editor work required for that. >_>
                    
                    //std::string type = Local::getStringNode(*iter, "type");
                }

                n = (*iter)->getChild("dimensions");
                int width = Local::getIntNode(n, "width");
                int height = Local::getIntNode(n, "height");

                n = (*iter)->getChild("position");
                lay->x = Local::getIntNode(n, "x");
                lay->y = Local::getIntNode(n, "y");

                n = (*iter)->getChild("parallax");
                lay->parallax.mulx = Local::getIntNode(n, "mulx");
                lay->parallax.muly = Local::getIntNode(n, "muly");
                lay->parallax.divx = Local::getIntNode(n, "divx");
                lay->parallax.divy = Local::getIntNode(n, "divy");

                n = (*iter)->getChild("wrap");
                lay->wrapx = Local::getStringNode(n, "x") == "true";
                lay->wrapy = Local::getStringNode(n, "y") == "true";

                {
                    DataNode* dataNode = (*iter)->getChild("data");
                    if (Local::getStringNode(dataNode, "format") != "zlib")
                        throw std::runtime_error("Unrecognized data format.");

                    std::string d64 = dataNode->getString();
                    ScopedArray<u8> compressed(new u8[d64.length()]);
                    int compressedSize;
                        
                    if (ver == "1.0") {

                        static bool warn1dot0 = false;
                        if (!warn1dot0) {
                            Log::Write("Warning: 1.0 tileset loading will be officially dropped in 0.61");
                            Log::Write("    Actually, they may still work, but this isn't guaranteed!");
                            Log::Write("    To convert the map to the current version, load and re-save");
                            Log::Write("    it in ikaMap");
                            warn1dot0 = true;
                        }

                        compressedSize = oldBase64::decode(d64, compressed.get(), d64.length());
                    } else if (ver == "1.1") {
                        std::string un64 = base64::decode(d64);
                        std::copy((u8*)un64.c_str(), (u8*)un64.c_str() + un64.length(), compressed.get());
                        compressedSize = un64.length();
                    } else {
                        // Don't know how to handle
                        assert(false);
                    }

                    ScopedArray<uint> tiles(new uint[width * height]);
                    Compression::decompress(compressed.get(), compressedSize, reinterpret_cast<u8*>(tiles.get()), width * height * sizeof(uint));
                    lay->tiles = Matrix<uint>(width, height, tiles.get());
                }
 {
                    DataNode* obsNode = (*iter)->getChild("obstructions");

                    std::string d64 = obsNode->getString();
                    ScopedArray<u8> compressed(new u8[d64.length()]);
                    int compressedSize;

                    if (ver == "1.0") {
                        compressedSize = oldBase64::decode(d64, compressed.get(), d64.length());
                    } else if (ver == "1.1") {
                        std::string un64 = base64::decode(d64);
                        std::copy((u8*)(un64.c_str()), (u8*)(un64.c_str() + un64.length()), compressed.get());
                        compressedSize = un64.length();
                    } else {
                        assert(false);
                    }

                    ScopedArray<u8> obs(new u8[width * height]);
                    Compression::decompress(compressed.get(), compressedSize, obs.get(), width * height);
                    lay->obstructions = Matrix<u8>(width, height, obs.get());
                }

                {
                    DataNode* entNode = (*iter)->getChild("entities");

                    DataNodeList nodes = entNode->getChildren("entity");
                    for (DataNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
                        Entity e;

                        e.x = Local::getIntNode(*iter, "x");
                        e.y = Local::getIntNode(*iter, "y");
                        e.label = Local::getStringNode(*iter, "label");
                        e.spriteName = Local::getStringNode(*iter, "sprite");

                        std::string dir = Local::getStringNode(*iter, "direction");
                        e.direction = face_down;
                        for (uint i = 0; i < numDirs; i++)
                            if (dir == dirNames[i]) {   e.direction = (Direction)i; break;  }

                        e.speed = Local::getIntNode(*iter, "speed");
                        e.moveScript = Local::getStringNode(*iter, "move_script");
                        e.obstructsEntities = Local::getStringNode(*iter, "obstructs_entities") == "true";
                        e.obstructedByEntities = Local::getStringNode(*iter, "obstructed_by_entities") == "true";
                        e.obstructedByMap = Local::getStringNode(*iter, "obstructed_by_map") == "true";
                        e.adjActivateScript = Local::getStringNode(*iter, "adj_activate_script");
                        e.activateScript = Local::getStringNode(*iter, "activate_script");

                        lay->entities.push_back(e);
                    }
                }

                {
                    DataNode* zoneNode = (*iter)->getChild("zones");

                    DataNodeList nodes = zoneNode->getChildren("zone");
                    for (DataNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
                    {
                        Layer::Zone z;
                        z.label = Local::getStringNode(*iter, "label");
                        z.position = Rect(
                            Local::getIntNode(*iter, "x"),
                            Local::getIntNode(*iter, "y"),
                            Local::getIntNode(*iter, "width"),
                            Local::getIntNode(*iter, "height"));

                        // rect constructor is x1y1 x2y2.  Not xy width height.  Deal with it.
                        z.position.right += z.position.left;
                        z.position.bottom += z.position.top;

                        lay->zones.push_back(z);
                    }
                }

                AddLayer(lay);
            }
        }

        delete rootNode;
        return true;
    }
    catch (std::runtime_error err) {
        delete rootNode;
        Log::Write("Map::Load(\"%s\"): %s", filename.c_str(), err.what());

        return false;
    }
}

void Map::Save(const std::string& filename) {
    DataNode* rootNode = newNode("ika-map");

    rootNode->addChild(newNode("version")->addChild("1.1"));

    {
        DataNode* infoNode = newNode("information")
                ->addChild(
                    newNode("title")->addChild(title)
                );

        rootNode->addChild(infoNode);

        DataNode* metaNode = newNode("meta");
        infoNode->addChild(metaNode);
        for (std::map<std::string, std::string>::iterator iter = metaData.begin(); iter != metaData.end(); iter++)
            metaNode->addChild(
                newNode(iter->first)
                    ->addChild(iter->second)
            );
    }

    {
#if defined(MULTI_TILESETS)
        DataNode* headerNode = newNode("header");
        headerNode->addChild(
                newNode("dimensions")
                    ->addChild(newNode("width")->addChild(width))
                    ->addChild(newNode("height")->addChild(height))
            );

        for (uint i = 0; i < tileSetNames.size(); i++)
            headerNode->addChild(
                newNode("tileset")->addChild(tileSetNames[i])
            );
#else
        rootNode->addChild(
        newNode("header")
            ->addChild(
                newNode("dimensions")
                    ->addChild(newNode("width")->addChild(width))
                    ->addChild(newNode("height")->addChild(height))
            )
            ->addChild(
                newNode("tileset")->addChild(tileSetName)
            )
        );
#endif
    }

    {
        DataNode* wpNode = newNode("waypoints");
        rootNode->addChild(wpNode);

        for (WayPointMap::iterator iter = wayPoints.begin(); iter != wayPoints.end(); iter++) {
            WayPoint& wp = iter->second;
            wpNode->addChild(
                newNode("waypoint")
                    ->addChild(newNode("label")->addChild(wp.label))
                    ->addChild(newNode("x")->addChild(wp.x))
                    ->addChild(newNode("y")->addChild(wp.y))
                );
        }
    }

    {
        DataNode* zoneNode = newNode("zones");
        rootNode->addChild(zoneNode);

        for (ZoneMap::iterator iter = zones.begin(); iter != zones.end(); iter++) {
            Zone& z = iter->second;
            zoneNode->addChild(newNode("zone")
                ->addChild(newNode("label")->addChild(z.label))
                ->addChild(newNode("script")->addChild(z.scriptName))
                );
        }
    }

    {
        DataNode* layerNode = newNode("layers");
        rootNode->addChild(layerNode);

        for (uint i = 0; i < layers.size(); i++) {
            Layer* lay = layers[i];

            DataNode* layNode = newNode("layer");
            layerNode->addChild(layNode);
            layNode
                ->addChild(newNode("label")->addChild(lay->label))
                ->addChild(newNode("type")->addChild("tile"))
                ->addChild(newNode("dimensions")
                    ->addChild(newNode("width")->addChild(lay->Width()))
                    ->addChild(newNode("height")->addChild(lay->Height()))
                    )
                ->addChild(newNode("position")
                    ->addChild(newNode("x")->addChild(lay->x))
                    ->addChild(newNode("y")->addChild(lay->y))
                    )
                ->addChild(newNode("parallax")
                    ->addChild(newNode("mulx")->addChild(lay->parallax.mulx))
                    ->addChild(newNode("muly")->addChild(lay->parallax.muly))
                    ->addChild(newNode("divx")->addChild(lay->parallax.divx))
                    ->addChild(newNode("divy")->addChild(lay->parallax.divy))
                    )
                ->addChild(newNode("wrap")
                    ->addChild(newNode("x")->addChild(lay->wrapx ? "true" : "false"))
                    ->addChild(newNode("y")->addChild(lay->wrapy ? "true" : "false"))
                    );

            {
                const int dataSize = lay->Width() * lay->Height() * sizeof(uint);
                ScopedArray<u8> compressed(new u8[dataSize]);
                int compressSize = Compression::compress(
                    reinterpret_cast<const u8*>(lay->tiles.GetPointer(0, 0)),
                    dataSize,
                    compressed.get(),
                    dataSize);

                //std::string d64 = base64::encode(reinterpret_cast<u8*>(compressed.get()), compressSize);
                std::string d64 = base64::encode(std::string(compressed.get(), compressed.get() + compressSize));

                layNode->addChild(newNode("data")
                    ->addChild(newNode("format")->addChild("zlib"))
                    ->addChild(d64)
                    );
            }

            {
                ScopedArray<u8> compressed(new u8[lay->Width() * lay->Height() * sizeof(uint)]);
                int compressSize = Compression::compress(
                    lay->obstructions.GetPointer(0, 0),
                    lay->Width() * lay->Height(),
                    compressed.get(),
                    lay->Width() * lay->Height());

                std::string d64 = base64::encode(std::string(compressed.get(), compressed.get() + compressSize));

                layNode->addChild(newNode("obstructions")
                    ->addChild(newNode("style")->addChild("tile"))
                    ->addChild(newNode("format")->addChild("zlib"))
                    ->addChild(d64)
                    );
            }

            {
                DataNode* entNode = newNode("entities");
                layNode->addChild(entNode);

                for (std::vector<Entity>::iterator iter = lay->entities.begin();
                    iter != lay->entities.end();
                    iter++) {
                    entNode->addChild(newNode("entity")
                        ->addChild(newNode("label")->addChild(iter->label))
                        ->addChild(newNode("x")->addChild(iter->x))
                        ->addChild(newNode("y")->addChild(iter->y))
                        ->addChild(newNode("sprite")->addChild(iter->spriteName))
                        ->addChild(newNode("speed")->addChild(iter->speed))
                        ->addChild(newNode("direction")->addChild(dirNames[(int)iter->direction]))
                        ->addChild(newNode("move_script")->addChild(iter->moveScript))
                        ->addChild(newNode("obstructs_entities")->addChild(iter->obstructsEntities ? "true" : "false"))
                        ->addChild(newNode("obstructed_by_entities")->addChild(iter->obstructedByEntities ? "true" : "false"))
                        ->addChild(newNode("obstructed_by_map")->addChild(iter->obstructedByMap ? "true" : "false"))
                        ->addChild(newNode("adj_activate_script")->addChild(iter->adjActivateScript))
                        ->addChild(newNode("activate_script")->addChild(iter->activateScript))
                        );
                }
            }

            {
                DataNode* zoneNode = newNode("zones");
                layNode->addChild(zoneNode);

                for (std::vector<Layer::Zone>::iterator iter = lay->zones.begin();
                    iter != lay->zones.end();
                    iter++)
                {
                    zoneNode->addChild(newNode("zone")
                        ->addChild(newNode("label")->addChild(iter->label))
                        ->addChild(newNode("x")->addChild(iter->position.left))
                        ->addChild(newNode("y")->addChild(iter->position.top))
                        ->addChild(newNode("width")->addChild(iter->position.Width()))
                        ->addChild(newNode("height")->addChild(iter->position.Height()))
                        );
                }
            }
        }
    }

    std::ofstream file(filename.c_str());
    file << rootNode;
    file.close();
    delete rootNode;
}

Map::Layer* Map::GetLayer(uint index) {
    assert(index >= 0 && index < layers.size());
    return layers[index];
}

uint Map::LayerIndex(Map::Layer* lay) const {
    for (uint i = 0; i < layers.size(); i++) {
        if (lay == layers[i])
            return i;
    }
    return (uint)-1;
}

uint Map::LayerIndex(const std::string& label) const {
    for (uint i = 0; i < layers.size(); i++) {
        if (layers[i]->label == label)
            return i;
    }
    return (uint)-1;
}

Map::Layer* Map::AddLayer(const std::string& name, uint width, uint height) {
    Map::Layer* lay = new Layer(name, width, height);
    AddLayer(lay);
    return lay;
}

void Map::AddLayer(Map::Layer* lay) {
    layers.push_back(lay);
}

void Map::InsertLayer(Map::Layer* lay, uint index) {
    assert(index <= NumLayers());

    if (index == NumLayers())
        AddLayer(lay);
    else
        layers.insert(layers.begin() + index, lay);
}

void Map::DestroyLayer(uint index) {
    assert(index >= 0 && index < layers.size());
    delete layers[index];
    layers.erase(layers.begin() + index);
}

void Map::DestroyLayer(Map::Layer* lay) {
    DestroyLayer(LayerIndex(lay));
}

void Map::SwapLayers(uint i, uint j) {
    assert(i >= 0 && i < layers.size());
    assert(j >= 0 && j < layers.size());

    Map::Layer* l = layers[i];
    layers[i] = layers[j];
    layers[j] = l;
}

uint Map::NumLayers() const {
    return layers.size();
}
