#include <algorithm>
#include <stdexcept>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

#include "main.h"

#include "common/misc.h"
#include "timer.h"

#include "input.h"
#include "opengl/Driver.h"
//#include "soft32/Driver.h"
#include "keyboard.h"
void Engine::Sys_Error(const char* errmsg)
{
    CDEBUG("sys_error");

#if (defined WIN32)
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    HWND hWnd = SDL_GetWMInfo(&info) ? info.window : HWND_DESKTOP;

    if (strlen(errmsg))
        MessageBox(hWnd, errmsg, "Error", 0);
#else
    printf("%s", errmsg);
#endif

    Shutdown();
    exit(-1);
}

void Engine::Script_Error()
{
    CDEBUG("script_error");
    Shutdown();

    std::string err = script.GetErrorMessage();

#if (defined WIN32)
    if (!err.empty())
    {
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        HWND hWnd = SDL_GetWMInfo(&info) ? info.window : HWND_DESKTOP;

        MessageBox(hWnd, err.c_str(), "Script Error", 0);
    }
#else
    printf("%s", err.c_str());
#endif

    exit(-1);
}

void Engine::CheckMessages()
{
    CDEBUG("checkmessages");

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            the<Input>()->KeyDown(event.key.keysym.sym);
            // bottom line screenshot if F11 is pressed
//            if (event.key.keysym.sym==SDLK_F11 && event.key.state==SDL_PRESSED)
//                ScreenShot();

            // Alt-F4.  Quit.  NOW.
            if (event.key.keysym.sym == SDLK_F4 &&
                (SDL_GetModState() & (KMOD_LALT | KMOD_RALT)))
            {
                Shutdown();
                exit(0);
            }
            break;

        case SDL_KEYUP:
            the<Input>()->KeyUp(event.key.keysym.sym);
            break;

        case SDL_JOYAXISMOTION:
            the<Input>()->JoyAxisMove(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
            break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            the<Input>()->JoyButtonChange(event.jbutton.which, event.jbutton.button, event.jbutton.state == SDL_PRESSED);
            break;

        case SDL_QUIT:
            Shutdown();
            exit(0);
            break;
        }
    }
}

void Engine::MainLoop()
{
    CDEBUG("mainloop");
    static int numframes, t = 0, fps = 0;                           // frame counter stuff (Why do these need to be static?)

    CFont* font;
    try
    {
        font = new CFont("system.fnt", video);
    }
    catch (FontException)
    {
        font = 0;
        _showFramerate = false;
    }

    int now = GetTime();
    int lasttick = now;

    while (true)
    {
        CheckMessages();

        int skipcount = 0;

        for (int i = 0; (i < now - lasttick) && (++skipcount <= _frameSkip); i++)
        {
            GameTick();
        }

        lasttick = now;
        now = GetTime();

        Render();

        if (_showFramerate)
        {
            font->PrintString(0, 0, va("Fps: %i", video->GetFrameRate()));
        }

        video->ShowPage();

        // if we're on the fast track, give the OS a minute.
        if (now == GetTime()) SDL_Delay(10);
    }

    delete font;
}

void Engine::Startup()
// TODO: Make a nice happy GUI thingie for making a user.cfg
// This is ugly. :(
{
    CDEBUG("Startup");

    CConfigFile cfg("user.cfg");

    // init a few values
    _showFramerate  = cfg.Int("showfps") != 0;
    _frameSkip      = cfg.Int("frameskip");

    // Now the tricky stuff.
    try
    {
        if (cfg.Int("log"))
            Log::Init("ika.log");

        Log::Write("ika %s startup", IKA_VERSION);
        Log::Write("Built on " __DATE__);
        Log::Write("--------------------------");

        Log::Write("Initializing SDL");
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK
#ifndef _DEBUG
            | SDL_INIT_NOPARACHUTE
#endif
            );

        atexit(SDL_Quit);


#if (!defined _DEBUG)
        SDL_WM_SetCaption(va("ika %s", IKA_VERSION), 0);
#else
        SDL_WM_SetCaption(va("ika %s (debug)", IKA_VERSION), 0);
#endif

        Log::Write("Initializing Video");
        std::string driver = Lower(cfg["videodriver"]);

#if 0
        if (driver == "soft" || driver == "sdl") // disabled because it's unstable and scary.
        {
            Log::Write("Starting SDL video driver");
            video = new Soft32::Driver(
                cfg.Int("xres"), 
                cfg.Int("yres"), 
                cfg.Int("bitdepth"), 
                cfg.Int("fullscreen") != 0);
        }
        else if (driver == "opengl")
#endif
        {
            Log::Write("Starting OpenGL video driver");
            video = new OpenGL::Driver(
                cfg.Int("xres"), 
                cfg.Int("yres"), 
                cfg.Int("bitdepth"), 
                cfg.Int("fullscreen") != 0,
                cfg.Int("doublesize") != 0);
        }

#ifdef WIN32
        {
            SDL_SysWMinfo info;
            SDL_VERSION(&info.version);
            HWND hWnd = SDL_GetWMInfo(&info) ? info.window : 0;
            if (hWnd)
                SetClassLong(hWnd, GCL_HICON, (long)LoadIcon(GetModuleHandle(0), "AppIcon"));
        }
#endif

        Log::Write("Initializing Input");
        SDL_JoystickEventState(SDL_ENABLE);
        Input::GetInstance(); // force creation of the singleton instance.

        Log::Write("Initializing sound");
        Sound::Init(cfg.Int("nosound") != 0);
    }
    catch (Video::Exception)
    {
        video = 0;
        Sys_Error("Unable to set the video mode.\nAre you sure your hardware can handle the chosen settings?");
    }
    catch (Sound::Exception)
    {
        Log::Write("Sound initialization failed.  Disabling audio.");
    }
    catch (...)
    {
        Sys_Error("An unknown error occurred during initialization.");
    }

    srand(GetTime());

    Log::Write("Initing Python");
    script.Init(this);
    Log::Write("Executing system.py");
    bool result = script.LoadSystemScripts("system.py");
    if (!result)
        Script_Error();

    if (!_isMapLoaded)
        Sys_Error("");

    Log::Write("Startup complete");
}

void Engine::Shutdown()
{
    CDEBUG("shutdown");

#ifdef _DEBUG
    static bool blah = false;

    if (blah)
    {
        Log::Write("REDUNDANT CALLS TO SHUTDOWN!!!!!");
        return;
    }

    blah = true;
#endif

    Log::Write("---- shutdown ----");
    Sound::Shutdown();
    script.Shutdown();
    entities.clear();
    Input::Destroy();
    delete video;
    SDL_Quit();
}


void Engine::RenderEntity(const Entity* e)
{
    const Sprite* const s = e->sprite;
    const Map::Layer* layer = map.GetLayer(e->layerIndex);

    int x = e->x - xwin - s->nHotx + layer->x;
    int y = e->y - ywin - s->nHoty + layer->y;

    RenderEntity(e, x, y);
}

void Engine::RenderEntity(const Entity* e, int x, int y)
{
    const Sprite* s = e->sprite;

    uint frame = (e->specFrame != -1) ? e->specFrame : e->curFrame;
    if (frame >= s->Count()) frame = 0;

    video->BlitImage(s->GetFrame(frame), x, y);
}

// ------------------------------------------------------------------------------------------

namespace
{
    class CompareEntities
    {
    public:
        inline int operator () (const Entity* a, const Entity* b) const
        {
            return a->y < b->y;
        }
    };
};

/*
 * This could probably use some optimization.  Every time we draw the map,
 * we do n filter operations, then n sort operations.  huuurk.
 * Maybe keep the entity linked list y sorted at all times by reordering
 * whenever a y value changes.  That's only O(n) as opposed to O(nlogn) or
 * whatever unholy growth rate this is.
 */
void Engine::RenderEntities(uint layerIndex)
{
    CDEBUG("renderentities");

    std::vector<Entity*>     drawlist;
    const Point res = video->GetResolution();
    const Map::Layer* layer = map.GetLayer(layerIndex);

    // first, get a list of entities onscreen
    int width, height;
    for (EntityList::iterator i = entities.begin(); i != entities.end(); i++)
    {
        Entity* e = *i;
        const Sprite* sprite = e->sprite;

        if (e->layerIndex != layerIndex)    continue;   // wrong layer
        if (!sprite)                        continue;   // no sprite? @_x

        width = sprite->Width();
        height = sprite->Height();

        // get the coodinates at which the sprite would be drawn
        int x = e->x - sprite->nHotx + layer->x - xwin;
        int y = e->y - sprite->nHoty + layer->y - ywin;

        if (x + width > 0 && y + height > 0 &&
            x < res.x     && y < res.y      &&
            e->isVisible)
            drawlist.push_back(e);                                                          // the entity is onscreen, tag it.
    }

    if (!drawlist.size())
        return;                                                                             // nobody onscreen?  Easy out!

    // Sort them by y value. (see the CompareEntity functor above)
    std::sort(drawlist.begin(), drawlist.end(), CompareEntities());

    video->SetBlendMode(Video::Normal);
    for (std::vector<Entity*>::iterator j = drawlist.begin(); j != drawlist.end(); j++)
    {
        const Entity* e = *j;
        const Sprite* s = e->sprite;
        int x = e->x - xwin - s->nHotx + layer->x;
        int y = e->y - ywin - s->nHoty + layer->y;

        RenderEntity(e, x, y);
    }
}

void Engine::RenderLayer(uint layerIndex)
{
    CDEBUG("renderlayer");
    int        lenX, lenY;        // x/y run length
    int        firstX, firstY;        // x/y start
    int        adjustX, adjustY;    // sub-tile offset
    int        xw, yw;
    const Map::Layer* layer = map.GetLayer(layerIndex);

    int layerWidth = layer->Width() * tiles->Width();
    int layerHeight = layer->Height() * tiles->Height();

    xw = (xwin * layer->parallax.mulx / layer->parallax.divx) - layer->x;
    yw = (ywin * layer->parallax.mulx / layer->parallax.divy) - layer->y;

    firstX = xw / tiles->Width();
    firstY = yw / tiles->Height();

    adjustX = xw % tiles->Width();
    adjustY = yw % tiles->Height();

    const Point res = video->GetResolution();
    lenX = res.x / tiles->Width() + 1;
    lenY = res.y / tiles->Height() + 2;

    if (firstX < 0)
    {
        lenX -= -firstX;
        adjustX += firstX * tiles->Width();
        firstX = 0;
    }

    if (firstY < 0)
    {
        lenY -= -firstY;
        adjustY += firstY * tiles->Height();
        firstY = 0;
    }

    if ((uint)(firstX + lenX) > layer->Width())  lenX = layer->Width() - firstX;        // clip yo
    if ((uint)(firstY + lenY) > layer->Height()) lenY = layer->Height() - firstY;

    if (lenX < 1 || lenY < 1) return;   // not visible

    const uint*  t = layer->tiles.GetPointer(firstX, firstY);
    int xinc = layer->Width() - lenX;

    int curx = -adjustX;
    int cury = -adjustY;

    video->SetBlendMode(Video::Normal);
    for (int y = 0; y < lenY; y++)
    {
        for (int x = 0; x < lenX; x++)
        {
            //if (*t)
                video->BlitImage(tiles->GetTile(*t), curx, cury);

            curx += tiles->Width();
            t++;
        }
        cury += tiles->Height();
        curx = -adjustX;
        t += xinc;
    }
}

void Engine::Render()
{
    CDEBUG("render");
    const Point res = video->GetResolution();

    if (_recurseStop)
        Sys_Error("Calling Map.Render within a HookRetrace'd script is a Very, Very Bad Idea.  Don't do this!");

    if (!_isMapLoaded)    return;

    tiles->UpdateAnimation(GetTime());

    if (cameraTarget)
    {
        const Map::Layer* layer = map.GetLayer(cameraTarget->layerIndex);

        SetCamera(Point(
            cameraTarget->x - res.x / 2 + layer->x,
            cameraTarget->y - res.y / 2 + layer->y));
    }

    video->DrawRect(0, 0, res.x, res.y, RGBA(0, 0, 0), true);

    for (uint i = 0; i < map.NumLayers(); i++)
    {
        RenderLayer(i);
        RenderEntities(i);
    }

    _recurseStop = true;
    DoHook(_hookRetrace);
    _recurseStop = false;
}

// redundant.  gah.
void Engine::Render(const std::vector<uint>& list)
{
    CDEBUG("render");
    const Point res = video->GetResolution();

    if (_recurseStop)
        Sys_Error("Calling Map.Render within a HookRetrace'd script is a Very, Very Bad Idea.  Don't do this!");

    if (!_isMapLoaded) return;

    tiles->UpdateAnimation(GetTime());

    if (cameraTarget)
    {
        const Map::Layer* layer = map.GetLayer(cameraTarget->layerIndex);

        SetCamera(Point(
            cameraTarget->x - res.x / 2 + layer->x,
            cameraTarget->y - res.y / 2 + layer->y));
    }

    // Note that we do not clear the screen here.  This is intentional.

    for (uint i = 0; i < list.size(); i++)
    {
        uint j = list[i];
        if (j < map.NumLayers())
        {
            RenderLayer(j);
            RenderEntities(j);
        }
    }

    _recurseStop = true;
    DoHook(_hookRetrace);
    _recurseStop = false;
}

void Engine::DoHook(HookList& hooklist)
{
    hooklist.Flush(); // handle any pending insertions/deletions

    for (HookList::List::iterator i = hooklist.begin(); i != hooklist.end(); i++)
    {
        script.ExecObject(*i);
    }
}

// ----------------------------------------- AI -------------------------------------------------

void Engine::GameTick()
{
    CDEBUG("gametick");

    CheckKeyBindings();
    DoHook(_hookTimer);
    ProcessEntities();
}

void Engine::CheckKeyBindings()
{
    // The "queue" is only one element big.  Unless someone hit two buttons in the same instant,
    // nobody will notice. (hopefully)

    if (ScriptObject* func = the<Input>()->GetQueuedEvent())
    {
        // The key that triggered the event would be initially pressed if not for this.
        // This is not useful behaviour.
        the<Input>()->Unpress();
        script.ExecObject(*func);
        the<Input>()->Flush();
    }
}

void Engine::ProcessEntities()
{
    for (EntityList::iterator curEnt = entities.begin(); curEnt != entities.end(); curEnt++)
    {
        Entity* ent = *curEnt;
        ent->speedCount += ent->speed;
        while (ent->speedCount >= timeRate)
        {
            ent->Update();
            ent->speedCount -= timeRate;
        }
    }
}

// --------------------------------------- Entity Handling --------------------------------------

bool Engine::DetectMapCollision(int x, int y, int w, int h, uint layerIndex)
// returns true if there is an obstructed map square anywhere along a specified vertical or horizontal line
// Also TODO: think up a better obstruction system
{
    CDEBUG("detectmapcollision");
    int tx = tiles->Width();
    int ty = tiles->Height();
    Map::Layer* layer = map.GetLayer(layerIndex);

    int y2 = (y + h - 1) / ty;
    int x2 = (x + w - 1) / tx;
    x /= tiles->Width();
    y /= tiles->Height();

    for (int cy = y; cy <= y2; cy++)
        for(int cx = x; cx <= x2; cx++)
            if (layer->obstructions(cx, cy))
                return true;

    return false;
}

Entity* Engine::DetectEntityCollision(const Entity* ent, int x1, int y1, int w, int h, uint layerIndex, bool wantobstructable)
// returns the entity colliding with the specified entity, or 0 if none.
// Note that passing 0 for ent is valid, indicating that you simply want to know if there are any entities in a given area
{
    CDEBUG("detectentitycollision");

    for (EntityList::const_iterator i = entities.begin(); i != entities.end(); i++)
    {
        Entity* e = *i;
        const Sprite* s = e->sprite;

        if ((e->layerIndex != layerIndex) ||                    // wrong layer
            (wantobstructable && !e->obstructsEntities) ||      // obstructable entities only?
            (e == ent))                         continue;       // self collision isn't all that useful.

        if (x1              >= e->x+s->nHotw)    continue;
        if (y1              >= e->y+s->nHoth)    continue;
        if (x1 + w          <= e->x)            continue;
        if (y1 + h          <= e->y)            continue;

        return e;
    }
    return 0;
}

// Warning: Brute force.
Map::Layer::Zone* Engine::TestZoneCollision(const Entity* ent)
{
    Map::Layer* layer = map.GetLayer(ent->layerIndex);

    assert(ent);
    assert(ent->sprite);

    int x  = ent->x;
    int y  = ent->y;
    int x2 = x + ent->sprite->nHotw;
    int y2 = y + ent->sprite->nHoth;

    for (std::vector<Map::Layer::Zone>::iterator
        iter = layer->zones.begin();
        iter != layer->zones.end();
        iter++)
    {
        if (x >= iter->position.left &&
            y >= iter->position.top &&
            x2 <= iter->position.right &&
            y2 <= iter->position.bottom)
            return &*iter; // hurk*/
    }

    return 0;
}

void Engine::TestActivate(const Entity* player)
// checks to see if we're supposed to run some a script, due to the player's actions.
// Thus far, that's one of two things.
// 1) the player steps on a zone, or
// 2) the player activates to an entity.

// This sucks.  It uses static varaibles, so it's not useful at all for any entity other than the player, among other things.
{
    CDEBUG("testactivate");
    Sprite* sprite = player->sprite;

    int tx = (player->x + sprite->nHotw / 2) / tiles->Width();
    int ty = (player->y + sprite->nHoth / 2) / tiles->Height();

    if (player->isMoving)
    {
        Map::Layer::Zone* zone = TestZoneCollision(player);
        if (zone)
        {
            if (map.zones.count(zone->label))
            {
                Map::Zone& bluePrint = map.zones[zone->label];
                if (!bluePrint.scriptName.empty())
                    script.CallScript(bluePrint.scriptName);
            }
            else
            {
                Log::Write("No blueprint %s exists!", zone->label.c_str());
            }
        }
    }

    // adjacent activation
    bool b = the<Input>()->enter->Pressed();
    if (!b) return; // Don't check the rest unless enter was pressed.

    tx = player->x; ty = player->y;
    // entity activation
    switch(player->direction)
    {
    case face_up:        ty -= sprite->nHoth;    break;
    case face_down:      ty += sprite->nHoth;    break;
    case face_left:      tx -= sprite->nHotw;    break;
    case face_right:     tx += sprite->nHotw;    break;

    case face_upleft:    tx -= sprite->nHotw;    ty -= sprite->nHoth;    break;
    case face_upright:   tx += sprite->nHotw;    ty -= sprite->nHoth;    break;
    case face_downleft:  tx -= sprite->nHotw;    ty += sprite->nHoth;    break;
    case face_downright: tx += sprite->nHotw;    ty += sprite->nHoth;    break;
    }

    Entity* ent = DetectEntityCollision(0 , tx, ty, sprite->nHotw, sprite->nHoth, player->layerIndex);
    if (ent)
    {
        if (ent->activateScript)
        {
            the<Input>()->Unpress();
            script.ExecObject(ent->activateScript);
            the<Input>()->Flush();
            return;
        }
    }
}

Entity* Engine::SpawnEntity()
{
    Entity* e = new Entity(this);

    entities.push_back(e);

    return e;
}

void Engine::DestroyEntity(Entity* e)
{
    for (EntityList::iterator i = entities.begin(); i != entities.end(); i++)
        if (e == (*i))
        {
            sprite.Free(e->sprite);

            // important stuff, yo.  Need to find any existing pointers to this entity, and null them.
            if (cameraTarget == e)  cameraTarget = 0;
            if (player == e)       player = 0;

            // actually nuke it
            entities.remove(e);
            delete e;
            return;
        }

    // In a Perfect World, this will never execute.
    Log::Write("Attempt to unallocate invalid entity!!!");
}

// --------------------------------- Misc (interface with old file formats, etc...) ----------------------

void Engine::LoadMap(const std::string& filename)
// Most of the work involved here is storing the various parts of the v2-style map into memory under the new structure.
{
    CDEBUG("loadmap");

    try
    {
        Log::Write("Loading map \"%s\"", filename.c_str());

        std::string oldTileSetName = map.tileSetName;

        if (!map.Load(filename)) throw filename;                        // actually load the map

        // Only load the tileset if it's different
        if (map.tileSetName != oldTileSetName)
        {
            delete tiles;                                               // nuke the old tileset
            tiles = new CTileSet(map.tileSetName, video);               // load up them tiles
        }

        script.ClearEntityList();                                       // DEI

        std::map<const Map::Entity*, Entity*> entMap;                   // used so we know which is related to which, so we can properly gather objects from the map script. (once it's loaded)

        // for each layer, create entities
        for (uint curLayer = 0; curLayer < map.NumLayers(); curLayer++)
        {
            const Map::Layer* lay = map.GetLayer(curLayer);
            const std::vector<Map::Entity>& ents = lay->entities;

            for (uint curEnt = 0; curEnt < ents.size(); curEnt++)
            {
                Entity* ent = new Entity(this, ents[curEnt], curLayer);
                entities.push_back(ent);
                ent->sprite = sprite.Load(ent->spriteName, video);
                script.AddEntityToList(ent);

                entMap[&ents[curEnt]] = ent;
            }
        }

        xwin = ywin = 0;                                                // just in case
        _isMapLoaded = true;

        if (!script.LoadMapScripts(filename))
            Script_Error();

        // Now that the scripts have been loaded, it's time to get the movement and activation scripts for the entities
        for (std::map<const Map::Entity*, Entity*>::iterator
            iter = entMap.begin();
            iter != entMap.end();
            iter++)
        {
            Entity* ent = iter->second;
            iter->second->moveScript = script.GetObjectFromMapScript(iter->first->moveScript);
            iter->second->activateScript = script.GetObjectFromMapScript(iter->first->activateScript);
            iter->second->adjActivateScript = script.GetObjectFromMapScript(iter->first->adjActivateScript);
        }
    }
    catch (std::runtime_error err)  {   Sys_Error(va("LoadMap(\"%s\"): %s", filename.c_str(), err.what())); }
    catch (TileSetException)        {   Sys_Error(va("Unable to load tileset %s", map.tileSetName.c_str())); }
    catch (const char* msg)         {   Sys_Error(va("Failed to load %s", msg));                            }
    catch (const std::string& msg)  {   Sys_Error(va("Failed to load %s", msg.c_str()));                    }
    catch (...)                     {   Sys_Error(va("Unknown error loading map %s", filename.c_str()));    }
}

Point Engine::GetCamera()
{
    return Point(xwin, ywin);
}

void Engine::SetCamera(Point p)
{
    Point res = video->GetResolution();
    xwin = max<int>(0, min<int>(p.x, map.width - res.x - 1));
    ywin = max<int>(0, min<int>(p.y, map.height - res.y - 1));
}

Engine::Engine()
    : tiles(0)
    , video(0)
    , player(0)
    , cameraTarget(0)
    , _isMapLoaded(false)
    , _recurseStop(false)
{}

int main(int argc, char* args[])
{
    Engine engine;
    engine.Startup();
    engine.MainLoop();
    engine.Shutdown();

    return 0;
}
