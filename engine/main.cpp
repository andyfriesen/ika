#include <algorithm>
#include <stdexcept>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

#include "main.h"

#include "timer.h"
#include "common/fileio.h"

#include "opengl/Driver.h"
#include "soft32/Driver.h"

void CEngine::Sys_Error(const char* errmsg)
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

void CEngine::Script_Error()
{
    CDEBUG("script_error");
    Shutdown();

    std::string err;
    
    File f;
    if (f.OpenRead("pyout.log"))
        err = f.ReadAll();
    else
        err = "Nonspecific script error.  Truly, the Tao is not with us in these dark times.";

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

void CEngine::CheckMessages()
{
    CDEBUG("checkmessages");

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            input.KeyDown(event.key.keysym.sym);
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
            input.KeyUp(event.key.keysym.sym);
            break;
           
        case SDL_QUIT:
            bKillFlag = true;
            Shutdown();
            exit(0);
            break;
        }
    }
}

void CEngine::MainLoop()
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

        for (int i = 0; (i < now - lasttick) && (++skipcount <= nFrameskip); i++)
        {
            if (bKillFlag)
                return;

            GameTick();
        }

        lasttick = now;
        now = GetTime();

        Render();

        if (_showFramerate)
        {
            font->PrintString(0, 0, va("FPS: %i", video->GetFrameRate()));
        }

        video->ShowPage();

        // if we're on the fast track, give the OS a minute.
        //if (now == GetTime()) SDL_Delay(10);
    }

    delete font;
}

void CEngine::Startup()
// TODO: Make a nice happy GUI thingie for making a user.cfg
// This is ugly. :(
{
    CDEBUG("Startup");
    
    CConfigFile cfg("user.cfg");

    if (!cfg.Good())
        Sys_Error("No user.cfg found.");

    // init a few values
    _showFramerate        = cfg.Int("showfps") != 0 ;
    nFrameskip      = cfg.Int("frameskip");

    // Now the tricky stuff.
    try
    {
        if (cfg.Int("log"))
            Log::Init("ika.log");
       
        Log::Write("ika " VERSION " startup");
        Log::Write("Built on " __DATE__);
        Log::Write("--------------------------");

        Log::Write("Initializing SDL");
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER
#ifndef _DEBUG
            | SDL_INIT_NOPARACHUTE
#endif
            );

        atexit(SDL_Quit);
          
        Log::Write("Initializing Video");
        std::string driver = Lower(cfg["videodriver"]);

        if (0)//driver == "soft" || driver == "sdl") // disabled because it's unstable and scary.
        {
            Log::Write("Using SDL video driver");
            video = new Soft32::Driver(cfg.Int("xres"), cfg.Int("yres"), cfg.Int("bpp"), cfg.Int("fullscreen") != 0);
        }
        else // if (driver == "opengl")
        {
            Log::Write("Using OpenGL video driver");
            video = new OpenGL::Driver(cfg.Int("xres"), cfg.Int("yres"), cfg.Int("bpp"), cfg.Int("fullscreen") != 0);
        }

#if (!defined _DEBUG)
        SDL_WM_SetCaption("ika " VERSION, 0);
#else
        SDL_WM_SetCaption("ika " VERSION " (debug)", 0);
#endif

#ifdef WIN32
        {
            SDL_SysWMinfo info;
            SDL_VERSION(&info.version);
            HWND hWnd = SDL_GetWMInfo(&info) ? info.window : 0;
            if (hWnd)
                SetClassLong(hWnd, GCL_HICON, (long)LoadIcon(GetModuleHandle(0), "AppIcon"));
        }
#endif

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
    
    if (!bMaploaded)
        Sys_Error("");
    
    Log::Write("Startup complete");
}

void CEngine::Shutdown()
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

    delete video;
}

// ------------------------------------------------------------------------------------------

namespace
{
    class CompareEntities
    {
    public:
        int operator () (const Entity* a, const Entity* b)
        {
            return a->y < b->y;
        }
    };
};

void CEngine::RenderEntities(uint layerIndex)
{
    CDEBUG("renderentities");

    std::vector<Entity*>     drawlist;
    const Point res = video->GetResolution();
    const Map::Layer* layer = &map.GetLayer(layerIndex);
   
    // first, get a list of entities onscreen
    int width, height;
    for (EntityList::iterator i = entities.begin(); i != entities.end(); i++)
    {    
        Entity* e = *i;
        const CSprite* sprite = e->sprite;

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
        CSprite* s = e->sprite;
        
        int frame = (e->specFrame != -1) ? e->specFrame : e->curFrame;

        int x = e->x - xwin - s->nHotx + layer->x;
        int y = e->y - ywin - s->nHoty + layer->y;
       
        video->BlitImage(s->GetFrame(frame), x, y);
    }
}

void CEngine::RenderLayer(uint layerIndex)
{
    CDEBUG("renderlayer");
    int        lenX, lenY;        // x/y run length
    int        firstX, firstY;        // x/y start
    int        adjustX, adjustY;    // sub-tile offset
    int        xw, yw;
    const Map::Layer* layer = &map.GetLayer(layerIndex);

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
    
    if (firstX + lenX > layer->Width())  lenX = layer->Width() - firstX;        // clip yo
    if (firstY + lenY > layer->Height()) lenY = layer->Height() - firstY;

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

void CEngine::Render()
{
    CDEBUG("render");
    const Point res = video->GetResolution();
    
    if (!bMaploaded)    return;
    
    tiles->UpdateAnimation(GetTime());
    
    if (cameraTarget)
    {        
        const Map::Layer* layer = &map.GetLayer(cameraTarget->layerIndex);

        SetCamera(Point(
            cameraTarget->x - res.x / 2 + layer->x,
            cameraTarget->y - res.y / 2 + layer->y));
    }
    
    for (uint i = 0; i < map.NumLayers(); i++)
    {
        RenderLayer(i);
        RenderEntities(i);
    }
//    DoHook(_hookRetrace);
}

void CEngine::Render(const std::vector<uint>& list)
{

}

void CEngine::DoHook(HookList& hooklist)
{
    bool result;
    hooklist.Flush(); // handle any pending insertions/deletions

    for (HookList::List::iterator i = hooklist.begin(); i != hooklist.end(); i++)
    {
        result = script.ExecObject(*i);

        if (!result)
            Script_Error();
    }
}

// ----------------------------------------- AI -------------------------------------------------

void CEngine::GameTick()
{
    CDEBUG("gametick");
    
    CheckKeyBindings();
    DoHook(_hookTimer);
    ProcessEntities();
}

void CEngine::CheckKeyBindings()
{
    // This isn't really optimal, but I dunno if anybody will notice.
    // Pop whatever's on the top of the queue, and just handle that one.
    // Flush the rest of the queue.
    
    if (void* func = input.GetNextControlEvent())
    {
        // I don't like this, but if I don't, then the key triggerings start to do weird things.
        // Like, the key that triggered the hook will always initially be pressed. (not useful behaviour)
        input.Unpress();
        script.ExecObject(func);
        input.ClearEventQueue();
    }
}

void CEngine::ProcessEntities()
{
    for (EntityList::iterator curEnt = entities.begin(); curEnt != entities.end(); curEnt++)
    {
        Entity* ent = *curEnt;
        ent->speedCount += ent->speed;
        while (ent->speedCount >= 100)
        {
            ent->Update();
            ent->speedCount -= 100;
        }
    }
}

// --------------------------------------- Entity Handling --------------------------------------

bool CEngine::DetectMapCollision(int x, int y, int w, int h, uint layerIndex)
// returns true if there is an obstructed map square anywhere along a specified vertical or horizontal line
// Also TODO: think up a better obstruction system
{
    CDEBUG("detectmapcollision");
    int tx = tiles->Width();
    int ty = tiles->Height();
    Map::Layer* layer = &map.GetLayer(layerIndex);
    
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

Entity* CEngine::DetectEntityCollision(const Entity* ent, int x1, int y1, int w, int h, uint layerIndex, bool wantobstructable)
// returns the entity colliding with the specified entity, or 0 if none.
// Note that passing 0 for ent is valid, indicating that you simply want to know if there are any entities in a given area
{
    CDEBUG("detectentitycollision");

    for (EntityList::const_iterator i = entities.begin(); i != entities.end(); i++)
    {
        Entity* e = *i;
        const CSprite* s = e->sprite;

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

void CEngine::TestActivate(const Entity* player)
// checks to see if we're supposed to run some VC, due to the player's actions.
// Thus far, that's one of three things.
// 2) the player steps on a zone
// 1) the player talks to an entity
// 3) the player activates a zone whose aaa (has nothing to do with AA) property is set.

// This sucks.  It uses static varaibles, so it's not useful at all for any entity other than the player, among other things.
{
    CDEBUG("testactivate");
    static int    nOldtx = -1;
    static int    nOldty = -1;
    CSprite* sprite = player->sprite;
    
    int tx = (player->x + sprite->nHotw / 2) / tiles->Width();
    int ty = (player->y + sprite->nHoth / 2) / tiles->Height();
    
    /*int n = map.GetZone(tx, ty);
    // stepping on a zone?
    if ((tx != nOldtx || ty != nOldty) && n)                        // the player is not on the same zone it was before, check for activation
    {
        nOldtx = tx; nOldty = ty;                            // if we don't do this, the next processentities will cause the zone to be activated again and again and again...
        const SMapZone& zone = map.GetZoneInfo(map.GetZone(tx, ty));
        if (((rand()%100) < zone.nActchance) && zone.sActscript.length())
            script.CallScript(zone.sActscript.c_str());
    }*/
    
    nOldtx = tx; nOldty = ty;
    
    // adjacent activation
    
    if (!input.Enter().Pressed()) return;                           // From this point on, the only time we'd have to check this crap is if enter was pressed.
    //input.Unpress();
    
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
    
    Entity* pEnt = DetectEntityCollision(0 , tx, ty, sprite->nHotw, sprite->nHoth, player->layerIndex);
    if (pEnt)
    {
        if (!pEnt->activateScript.length() != 0)
        {
            script.CallScript(pEnt->activateScript.c_str());
            input.Flush();
            return;
        }
    }
    
    // Activating a zone?
    /*uint z = map.GetZone(tx / tiles->Width(), ty / tiles->Height());
    
    if (z >= map.NumZones())
        return; // invalid zone

    const SMapZone& zone = map.GetZoneInfo(z);
    
    if (zone.bAdjacentactivation)
    {
        script.CallScript(zone.sActscript.c_str());
        input.Flush();
    }*/
}

Entity* CEngine::SpawnEntity()
{
    Entity* e = new Entity(this);
    
    entities.push_back(e);
    
    return e;
}

void CEngine::DestroyEntity(Entity* e)
{
    for (EntityList::iterator i = entities.begin(); i != entities.end(); i++)
        if (e == (*i))
        {
            sprite.Free(e->sprite);
            
            // important stuff, yo.  Need to find any existing pointers to this entity, and null them.
            if (cameraTarget == e)   cameraTarget = 0;
            if (pPlayer == e)         pPlayer = 0;
            
            /*// O(n**2) I think.  Gay.
            for (EntityList::iterator ii = entities.begin(); ii != entities.end(); ii++)
            {
                Entity& e=*(*ii);
                
                if (e.pChasetarget == (*i))
                    e.pChasetarget = 0, e.movecode = mc_nothing;
                
                // TODO: add others, if needed
            }*/
            
            // actually nuke it
            entities.remove(e);
            delete e;
            return;
        }
        
    // In a Perfect World, this will never execute.
    Log::Write("Attempt to unallocate invalid entity!!!");
}

// --------------------------------- Misc (interface with old file formats, etc...) ----------------------

void CEngine::LoadMap(const std::string& filename)
// Most of the work involved here is storing the various parts of the v2-style map into memory under the new structure.
{
    CDEBUG("loadmap");
    
    try
    {
        Log::Write("Loading map \"%s\"", filename.c_str());
        
        if (!map.Load(filename)) throw filename;                        // actually load the map
        
        delete tiles;                                                   // nuke the old tileset
        tiles = new CTileSet(map.tileSetName, video);                   // load up them tiles
        
        script.ClearEntityList();                                       // DEI

        // for each layer, create entities
        for (uint curLayer = 0; curLayer < map.NumLayers(); curLayer++)
        {
            const Map::Layer* lay = &map.GetLayer(curLayer);
            const std::vector<Map::Entity>& ents = lay->entities;

            for (uint curEnt = 0; curEnt < ents.size(); curEnt++)
            {
                Entity* ent = new Entity(this, ents[curEnt], curLayer);
                entities.push_back(ent);
                ent->sprite = sprite.Load(ent->spriteName, video);
                script.AddEntityToList(ent);
            }
        }
        
        xwin = ywin = 0;                                                // just in case
        bMaploaded = true;
        
        if (!script.LoadMapScripts(filename))
            Script_Error();
    }
    catch (std::runtime_error err)  {   Sys_Error(va("LoadMap(\"%s\"): %s", filename.c_str(), err.what())); }
    catch (const char* msg)         {   Sys_Error(va("Failed to load %s", msg));                            }
    catch (const std::string& msg)  {   Sys_Error(va("Failed to load %s", msg.c_str()));                    }
    catch (...)                     {   Sys_Error(va("Unknown error loading map %s", filename));            }
}

Point CEngine::GetCamera()
{
    return Point(xwin, ywin);
}

void CEngine::SetCamera(Point p)
{
    Point res = video->GetResolution();
    xwin = clamp<int>(p.x, 0, map.width  * tiles->Width()  - res.x - 1);   // (tile width * number of tiles) - resolution - 1
    ywin = clamp<int>(p.y, 0, map.height * tiles->Height() - res.y - 1);
}

CEngine::CEngine()
    : tiles(0)
    , video(0)
    , pPlayer(0)
    , cameraTarget(0)
    , bMaploaded(false)
    , bKillFlag(false)
{}

int main(int argc, char* args[])
{
    CEngine engine;
    engine.Startup();
    engine.MainLoop();
    engine.Shutdown();
    
    return 0;
}
