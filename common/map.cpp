
#include <cppdom/cppdom.h>
#include "xmlutil.h"

#include "map.h"
#include "log.h"
#include "compression.h"
#include "base64.h"
#include "misc.h"

using namespace cppdom;

namespace
{
    const char* dirNames[] =
    {
        "up",
        "down",
        "left",
        "right",
        "up-left",
        "up-right",
        "down-left",
        "down_right",
        "wtf"
    };
    const int numDirs = sizeof dirNames / sizeof dirNames[0];
};

Map::Map()
    : width(0)
    , height(0)
{}

Map::Map(const std::string& filename)
{
    Load(filename);
}

Map::~Map()
{
    for (uint i = 0; i < layers.size(); i++)
        delete layers[i];
}

bool Map::Load(const std::string& filename)
{
    XMLContextPtr context(new XMLContext);
    XMLDocument document;

    try
    {
        document.load(std::ifstream(filename.c_str()), context);
        XMLNodePtr rootNode = document.getChild("ika-map");
        if (!rootNode.get())
            throw "No document root!";

        //<information>
        {
            XMLNodePtr infoNode = GetNode(rootNode, "information");

            title = Trim(GetCdata(infoNode->getChild("title")));

            // grab <information> elements
            XMLNodeList nodes = infoNode->getChildren("meta");
            for (XMLNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
            {
                std::string name = (*iter)->getAttribute("type");
                std::string value;

                XMLNodePtr cnode = (*iter)->getChildren().front();
                if (cnode.get() != 0 && cnode->getType() == xml_nt_cdata)
                    value = cnode->getCdata();

                if (!name.empty() && !value.empty())
                    metaData[name] = value;
            }
        }

        //<header>
        {
            XMLNodePtr headerNode = GetNode(rootNode, "header");
            
            //<dimensions>
            XMLNodePtr dimNode = GetNode(headerNode, "dimensions");
            width  = atoi(dimNode->getAttribute("width" ).getString().c_str());
            height = atoi(dimNode->getAttribute("height").getString().c_str());

            //<tileset>
            XMLNodePtr tileSetNode = GetNode(headerNode, "tileset");
            tileSetName = Trim(GetCdata(tileSetNode));
        }

        //<zones>
        {
            XMLNodePtr zoneNode = GetNode(rootNode, "zones");

            XMLNodeList nodes = zoneNode->getChildren("zone");
            for (XMLNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
            {
                Zone z;
                z.label = (*iter)->getAttribute("label").getString();
                XMLNodePtr scriptNode = GetNode((*iter), "script");
                z.scriptName = GetCdata(scriptNode);

                zones[z.label] = z;
            }
        }

        //<entities>
        {
            XMLNodePtr entNode = GetNode(rootNode, "entities");

            XMLNodeList nodes = entNode->getChildren("entity");
            for (XMLNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
            {
                XMLNodePtr n;
                Entity e;
                e.label = (*iter)->getAttribute("label").getString();

                n = GetNode(*iter, "sprite");
                e.spriteName = GetCdata(n);

                n = GetNode(*iter, "direction");
                std::string dir = GetCdata(n);
                for (int i = 0; i < numDirs; i++)
                    if (dir == dirNames[i])
                    {   e.direction = (Direction)i;    break;  }

                n = GetNode(*iter, "speed");
                e.speed = atoi(GetCdata(n).c_str());

                n = GetNode(*iter, "move_script");
                e.moveScript = GetCdata(n);

                n = GetNode(*iter, "obstructs");
                e.obstructsEntities = n->getAttribute("entities").getString() == "true";

                n = GetNode(*iter, "obstructed_by");
                e.obstructedByEntities = n->getAttribute("entities").getString() == "true";
                e.obstructedByMap      = n->getAttribute("map").getString() == "true";

                n = GetNode(*iter, "script");
                ((n->getAttribute("event").getString() == "direct") ?
                    e.activateScript : e.adjActivateScript) =
                    GetCdata(n);

                entities[e.label] = e;
            }
        }

        //<waypoints>
        {
            XMLNodePtr wpNode = GetNode(rootNode, "waypoints");

            XMLNodeList nodes = wpNode->getChildren("waypoint");
            for (XMLNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
            {
                WayPoint wp;
                wp.label = (*iter)->getAttribute("label").getString();
                wp.x = atoi((*iter)->getAttribute("x").getString().c_str());
                wp.y = atoi((*iter)->getAttribute("y").getString().c_str());
                wayPoints[wp.label] = wp;
            }
        }

        //<layers>
        {
            XMLNodePtr layNode = GetNode(rootNode, "layers");

            XMLNodeList nodes = layNode->getChildren("layer");
            for (XMLNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
            {
                XMLNodePtr n;
                Layer* lay = new Layer();

                lay->label = (*iter)->getAttribute("label");

                n = GetNode(*iter, "dimensions");
                int width = atoi(n->getAttribute("width").getString().c_str());
                int height = atoi(n->getAttribute("height").getString().c_str());

                n = GetNode(*iter, "position");
                lay->x = atoi(n->getAttribute("x").getString().c_str());
                lay->y = atoi(n->getAttribute("y").getString().c_str());

                n = GetNode(*iter, "parallax");
                lay->parallax.mulx = atoi(n->getAttribute("mulx").getString().c_str());
                lay->parallax.muly = atoi(n->getAttribute("muly").getString().c_str());
                lay->parallax.divx = atoi(n->getAttribute("divx").getString().c_str());
                lay->parallax.divy = atoi(n->getAttribute("divy").getString().c_str());

                n = GetNode(*iter, "wrap");
                lay->wrapx = n->getAttribute("x").getString() == "true";
                lay->wrapy = n->getAttribute("y").getString() == "true";

                //<data>
                {
                    n = GetNode(*iter, "data");
                    if (n->getAttribute("format").getString() != "zlib")
                        throw "Unrecognized data format";

                    std::string d64 = GetCdata(n);
                    ScopedArray<u8> compressed(new u8[d64.length()]);
                    int compressedSize = base64::decode(d64, compressed.get(), d64.length());
                    ScopedArray<uint> tiles(new uint[width * height]);
                    Compression::decompress(compressed.get(), compressedSize, reinterpret_cast<u8*>(tiles.get()), width * height * sizeof(uint));
                    lay->tiles = Matrix<uint>(width, height, tiles.get());
                }

                //<obstructions>
                {
                    n = GetNode(*iter, "obstructions");
                    if (n->getAttribute("format").getString() != "tile")
                        throw "Unrecognized obstruction style.";

                    std::string d64 = GetCdata(n);
                    ScopedArray<u8> compressed(new u8[d64.length()]);
                    int compressedSize = base64::decode(d64, compressed.get(), d64.length());
                    ScopedArray<u8> obs(new u8[width * height]);
                    Compression::decompress(compressed.get(), compressedSize, obs.get(), width * height);
                    lay->obstructions = Matrix<u8>(width, height, obs.get());
                }

                //<entities>
                {
                    n = GetNode(*iter, "entities");
                    
                    XMLNodeList nodes = n->getChildren("entity");
                    for (XMLNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
                    {
                        Layer::Entity o;
                        o.bluePrint = (*iter)->getAttribute("blueprint");
                        o.label = (*iter)->getAttribute("label").getString();
                        o.x = atoi((*iter)->getAttribute("x").getString().c_str());
                        o.y = atoi((*iter)->getAttribute("y").getString().c_str());
                        lay->entities.push_back(o);
                    }
                }

                //<zones>
                {
                    n = GetNode(*iter, "zones");
                    
                    XMLNodeList nodes = n->getChildren("zone");
                    for (XMLNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
                    {
                        Layer::Zone o;
                        o.label = (*iter)->getAttribute("label").getString();
                        o.x = atoi((*iter)->getAttribute("x").getString().c_str());
                        o.x = atoi((*iter)->getAttribute("y").getString().c_str());
                        o.width = atoi((*iter)->getAttribute("width").getString().c_str());
                        o.height = atoi((*iter)->getAttribute("height").getString().c_str());
                        lay->zones.push_back(o);
                    }
                }

                AddLayer(lay);
            }
        }
        return true;
    }
    catch (const char* msg)
    {
        throw std::runtime_error(msg);
    }
}

void Map::Save(const std::string& filename)
{
    struct Local
    {
        static void AddCdataNode(XMLContextPtr context, XMLNodePtr parent, const std::string& name, const std::string& cdata)
        {
            XMLNodePtr m(new XMLNode(context));
            m->setName(name);
            m->addChild(CData(context, cdata));
            parent->addChild(m);
        }
    };

    XMLContextPtr context(new XMLContext);
    XMLDocument document;
    XMLNodePtr rootNode(new XMLNode(context));
    rootNode->setName("ika-map");
    rootNode->setAttribute("version", "1.0");

    try
    {
        //<information>
        {
            XMLNodePtr infoNode(new XMLNode(context));
            infoNode->setName("information");

            XMLNodePtr n(new XMLNode(context));
            n->setName("title");
            n->addChild(CData(context, title));

            infoNode->addChild(n);

            for (std::map<std::string, std::string>::iterator iter = metaData.begin(); iter != metaData.end(); iter++)
                infoNode->addChild(MetaNode(context, iter->first, iter->second));

            rootNode->addChild(infoNode);
        }

        //<header>
        {
            XMLNodePtr headerNode(new XMLNode(context));
            headerNode->setName("header");

            {
                XMLNodePtr n(new XMLNode(context));
                n->setName("dimensions");
                n->setAttribute("width", va("%i", width));
                n->setAttribute("height", va("%i", height));
                headerNode->addChild(n);
            }

            {
                XMLNodePtr n(new XMLNode(context));
                n->setName("tileset");
                n->addChild(CData(context, tileSetName));
                headerNode->addChild(n);

                rootNode->addChild(headerNode);
            }
        }

        //<zones>
        {
            XMLNodePtr zoneNode(new XMLNode(context));
            zoneNode->setName("zones");

            for (std::map<std::string, Zone>::iterator iter = zones.begin(); iter != zones.end(); iter++)
            {
                XMLNodePtr n(new XMLNode(context));
                n->setName("zone");
                n->setAttribute("label", iter->second.label);
                
                XMLNodePtr m(new XMLNode(context));
                m->setName("script");
                m->addChild(CData(context, iter->second.scriptName));
                n->addChild(m);

                zoneNode->addChild(n);
            }

            rootNode->addChild(zoneNode);
        }

        //<entities>
        {
            XMLNodePtr entNode(new XMLNode(context));
            entNode->setName("entities");

            for (std::map<std::string, Entity>::iterator iter = entities.begin(); iter != entities.end(); iter++)
            {
                XMLNodePtr n(new XMLNode(context));
                n->setName("entity");
                n->setAttribute("label", iter->second.label);

                Local::AddCdataNode(context, n, "sprite", iter->second.spriteName);
                Local::AddCdataNode(context, n, "direction", dirNames[(int)iter->second.direction]);
                Local::AddCdataNode(context, n, "speed", va("%i", iter->second.speed));
                Local::AddCdataNode(context, n, "move_script", iter->second.moveScript);
                
                {
                    XMLNodePtr m(new XMLNode(context));
                    m->setName("obstructs");
                    m->setAttribute("entities", iter->second.obstructsEntities ? "true" : "false");
                    n->addChild(m);
                }

                {
                    XMLNodePtr m(new XMLNode(context));
                    m->setName("obstructed_by");
                    m->setAttribute("entities", iter->second.obstructedByEntities ? "true" : "false");
                    m->setAttribute("map", iter->second.obstructedByMap ? "true" : "false");
                    n->addChild(m);
                }

                {
                    XMLNodePtr m(new XMLNode(context));
                    m->setName("script");
                    m->setAttribute("event", "direct");
                    m->addChild(CData(context, iter->second.activateScript));
                    n->addChild(m);
                }

                entNode->addChild(n);
            }

            rootNode->addChild(entNode);
        }

        //<waypoints>
        {
            XMLNodePtr wp(new XMLNode(context));
            wp->setName("waypoints");

            for (std::map<std::string, WayPoint>::iterator iter = wayPoints.begin(); iter != wayPoints.end(); iter++)
            {
                XMLNodePtr n(new XMLNode(context));
                n->setName("waypoint");
                n->setAttribute("label", iter->second.label);
                n->setAttribute("x", va("%i", iter->second.x));
                n->setAttribute("y", va("%i", iter->second.y));
                wp->addChild(n);
            }

            rootNode->addChild(wp);
        }

        //<layers>
        {
            XMLNodePtr layerNode(new XMLNode(context));
            layerNode->setName("layers");

            for (std::vector<Layer*>::iterator iter = layers.begin(); iter != layers.end(); iter++)
            {
                Layer& layer = **iter;

                XMLNodePtr lay(new XMLNode(context));
                lay->setName("layer");
                lay->setAttribute("label", layer.label);

                //<dimensions>
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("dimensions");
                    n->setAttribute("width", va("%i", layer.Width()));
                    n->setAttribute("height", va("%i", layer.Height()));
                    lay->addChild(n);
                }

                //<position>
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("position");
                    n->setAttribute("x", va("%i", layer.x));
                    n->setAttribute("y", va("%i", layer.y));
                    lay->addChild(n);
                }

                //<parallax>
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("parallax");
                    n->setAttribute("mulx", va("%i", layer.parallax.mulx));
                    n->setAttribute("muly", va("%i", layer.parallax.muly));
                    n->setAttribute("divx", va("%i", layer.parallax.divx));
                    n->setAttribute("divy", va("%i", layer.parallax.divy));
                    lay->addChild(n);
                }

                //<wrap>
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("wrap");
                    n->setAttribute("x", layer.wrapx ? "true" : "false");
                    n->setAttribute("y", layer.wrapy ? "true" : "false");
                    lay->addChild(n);
                }

                //<data>
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("data");
                    n->setAttribute("format", "zlib");

                    const int dataSize = layer.Width() * layer.Height() * sizeof(uint);
                    ScopedArray<u8> compressed(new u8[dataSize]);
                    int compressSize = Compression::compress(
                        reinterpret_cast<const u8*>(layer.tiles.GetPointer(0, 0)), 
                        dataSize,
                        compressed.get(), 
                        dataSize);

                    std::string d64 = base64::encode(reinterpret_cast<u8*>(compressed.get()), compressSize);

                    n->addChild(CData(context, d64));
                    lay->addChild(n);
                }

                //<obstructions>
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("obstructions");
                    n->setAttribute("format", "tile");

                    ScopedArray<u8> compressed(new u8[layer.Width() * layer.Height() * sizeof(uint)]);
                    int compressSize = Compression::compress(layer.obstructions.GetPointer(0, 0), layer.Width() * layer.Height(), compressed.get(), layer.Width() * layer.Height());

                    std::string d64 = base64::encode(compressed.get(), compressSize);
                    n->addChild(CData(context, d64));
                    lay->addChild(n);
                }

                //<entities>
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("entities");

                    for (std::vector<Layer::Entity>::iterator i = layer.entities.begin(); i != layer.entities.end(); i++)
                    {
                        XMLNodePtr m(new XMLNode(context));
                        m->setName("entity");
                        m->setAttribute("blueprint", i->bluePrint);
                        m->setAttribute("label", i->label);
                        m->setAttribute("x", va("%i", i->x));
                        m->setAttribute("y", va("%i", i->y));
                        n->addChild(m);
                    }

                    lay->addChild(n);
                }

                //<zones>
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("zones");
                    
                    for (std::vector<Layer::Zone>::iterator i = layer.zones.begin(); i != layer.zones.end(); i++)
                    {
                        XMLNodePtr m(new XMLNode(context));
                        m->setName("entity");
                        m->setAttribute("label", i->label);
                        m->setAttribute("x", va("%i", i->x));
                        m->setAttribute("y", va("%i", i->y));
                        m->setAttribute("width", va("%i", i->width));
                        m->setAttribute("height", va("%i", i->height));
                        n->addChild(m);
                    }

                    lay->addChild(n);
                }

                layerNode->addChild(lay);
            }
            rootNode->addChild(layerNode);
        }

        document.addChild(rootNode);
        document.save(std::ofstream(filename.c_str()));
    }
    catch (XMLError)
    {
        throw std::runtime_error(va("Unable to write %s", filename.c_str()));
    }
    catch (const char* msg)
    {
        throw std::runtime_error(va("Map::Save(%s): %s", filename.c_str(), msg));
    }
}

Map::Layer& Map::GetLayer(uint index)
{
    assert(index >= 0 && index < layers.size());
    return *layers[index];
}

uint Map::LayerIndex(Map::Layer& lay) const
{
    for (uint i = 0; i < layers.size(); i++)
    {
        if (&lay == layers[i])
            return i;
    }
    return -1;
}

uint Map::LayerIndex(const std::string& label) const
{
    for (uint i = 0; i < layers.size(); i++)
    {
        if (layers[i]->label == label)
            return i;
    }
    return -1;
}

Map::Layer& Map::AddLayer(const std::string& name, uint width, uint height)
{
    Map::Layer* lay = new Layer(name, width, height);
    AddLayer(lay);
    return *lay;
}

void Map::AddLayer(Map::Layer* lay)
{
    layers.push_back(lay);
}

void Map::InsertLayer(Map::Layer* lay, uint index)
{
    assert(index <= NumLayers());

    if (index == NumLayers())
        AddLayer(lay);
    else
        layers.insert(layers.begin() + index, lay);
}

void Map::DestroyLayer(uint index)
{
    assert(index >= 0 && index < layers.size());
    delete layers[index];
    layers.erase(layers.begin() + index);
}

void Map::DestroyLayer(Map::Layer& lay)
{
    DestroyLayer(LayerIndex(lay));
}

void Map::SwapLayers(uint i, uint j)
{
    assert(i >= 0 && i < layers.size());
    assert(j >= 0 && j < layers.size());

    Map::Layer* l = layers[i];
    layers[i] = layers[j];
    layers[j] = l;
}

uint Map::NumLayers() const
{
    return layers.size();
}