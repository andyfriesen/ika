#include <algorithm>
#include <SDL/SDL.h>

#include "main.h"
#include <SDL/SDL_syswm.h>

#include "timer.h"

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
    map.Free();
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
        int operator () (const CEntity* a, const CEntity* b)
        {
            return a->y < b->y;
        }
    };
};

void CEngine::RenderEntities()
{
    CDEBUG("renderentities");
    std::vector < CEntity*>     drawlist;
    const Point res = video->GetResolution();
    
    // first, get a list of entities onscreen
    int width, height;
    for (EntityIterator i = entities.begin(); i != entities.end(); i++)
    {    
        CEntity& e=**i;
        CSprite& sprite=*e.pSprite;
        
        width =sprite.Width();
        height = sprite.Height();
        
        // get the coodinates at which the sprite would be drawn
        int x = e.x-sprite.nHotx;
        int y = e.y-sprite.nHoty;
        
        if (x+width-xwin > 0 && y+height-ywin > 0 &&
            x-xwin < res.x   && y-ywin < res.y    &&
            e.bVisible)
            drawlist.push_back(&e);                                                         // the entity is onscreen, tag it.
    }
    
    if (!drawlist.size())
        return;                                                                             // nobody onscreen?  Easy out!

    // Sort them by y value. (see the CompareEntity functor above)
    std::sort(drawlist.begin(), drawlist.end(), CompareEntities());

    video->SetBlendMode(Video::Normal);
    for (std::vector < CEntity*>::iterator j = drawlist.begin(); j != drawlist.end(); j++)
    {
        const CEntity& e=**j;
        CSprite& s = *e.pSprite;
        
        int frame = e.nSpecframe ? e.nSpecframe : e.nCurframe;
       
        video->BlitImage(s.GetFrame(frame), e.x - xwin - s.nHotx, e.y - ywin - s.nHoty);
    }
}

void CEngine::RenderLayer(uint lay, bool transparent)
{
    CDEBUG("renderlayer");
    int        xl, yl;        // x/y run length
    int        xs, ys;        // x/y start
    int        xofs, yofs;    // sub-tile offset
    int        xw, yw;
    SMapLayerInfo    linf;
    
    if (lay > map.NumLayers())
        return;
    
    map.GetLayerInfo(linf, lay);
    
    xw = (xwin * linf.pmulx) / linf.pdivx;
    yw = (ywin * linf.pmulx) / linf.pdivy;
    
    xs = xw / tiles->Width();
    ys = yw / tiles->Height();
    
    xofs =- (xw % tiles->Width());
    yofs =- (yw % tiles->Height());
    
    const Point res = video->GetResolution();
    xl = res.x / tiles->Width() + 1;
    yl = res.y / tiles->Height() + 2;
    
    if (xs+xl > map.Width()) xl = map.Width() - xs;        // clip yo
    if (ys+yl > map.Height()) yl = map.Height() - ys;
    
    u32*   t = map.GetDataPtr(lay) + (ys * map.Width() + xs);
    int xinc = map.Width() - xl;
    
    int curx = xofs;
    int cury = yofs;
    if (transparent)
    {
        video->SetBlendMode(Video::Normal);
        for (int y = 0; y < yl; y++)
        {
            for (int x = 0; x < xl; x++)
            {
                if (*t)
                    video->BlitImage(tiles->GetTile(*t), curx, cury);

                curx += tiles->Width();
                t++;
            }
            cury += tiles->Height();
            curx = xofs;
            t += xinc;
        }
    }
    else
    {
        video->SetBlendMode(Video::None);
        for (int y = 0; y < yl; y++)
        {
            for (int x = 0; x < xl; x++)
            {
                video->BlitImage(tiles->GetTile(*t), curx, cury);

                curx += tiles->Width();
                t++;
            }
            cury += tiles->Height();
            curx = xofs;
            t += xinc;
        }
    }
}

void CEngine::Render(const char* sTemprstring)
{
    CDEBUG("render");
    const Point res = video->GetResolution();
    char    rstring[255];
    char*   p;
    int     numlayers;
    
    if (!bMaploaded)    return;
    
    tiles->UpdateAnimation(GetTime());
    
    if (pcameraTarget)
    {        
        SetCamera(Point(
            pcameraTarget->x - res.x / 2,
            pcameraTarget->y - res.y / 2));
        
        int maxx=(map.Width()  * tiles->Width() ) - res.x;   // and make sure it's still in range
        int maxy=(map.Height() * tiles->Height()) - res.y;
    }
    
    if (!sTemprstring)
        strcpy(rstring, map.GetRString().c_str());
    else
        strcpy(rstring, sTemprstring);

    if (!strlen(rstring))
        return;
    
    p = rstring;
    numlayers = 0;
    
    while (*p)
    {
        switch (*p)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            RenderLayer(*p - '1', numlayers != 0);
            ++numlayers;
            break;
        case '0':
            RenderLayer(9, numlayers != 0);
            ++numlayers;
            break;
        case 'E':
            RenderEntities();
            break;
        case 'R':
            DoHook(_hookRetrace);
            break;
        }
        p++;
    }
}

void CEngine::DoHook(HookList& hooklist)
{
    bool result;
    hooklist.Flush(); // handle any pending insertions/deletions

    for (HookList::List::iterator i = hooklist.begin(); i != hooklist.end(); i++)
    {
        result = script.ExecFunction(*i);

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
        script.ExecFunction(func);
        input.ClearEventQueue();
    }
}

void CEngine::ProcessEntities()
{
    for (EntityIterator i = entities.begin(); i != entities.end(); i++)
    {
        CEntity& ent=**i;
        
        ent.nSpeedcount += ent.nSpeed;
        while (ent.nSpeedcount >= 100)
        {
            ent.Update();
            ent.nSpeedcount -= 100;
        }
    }
}

// --------------------------------------- Entity Handling --------------------------------------

bool CEngine::DetectMapCollision(int x, int y, int w, int h)
// returns true if there is an obstructed map square anywhere along a specified vertical or horizontal line
// Also TODO: think up a better obstruction system
{
    CDEBUG("detectmapcollision");
    int tx = tiles->Width();
    int ty = tiles->Height();
    
    int y2 = (y + h - 1) / ty;
    int x2 = (x + w - 1) / tx;
    x /= tiles->Width();
    y /= tiles->Height();

    for (int cy = y; cy <= y2; cy++)
        for(int cx = x; cx <= x2; cx++)
            if (map.IsObs(cx, cy))
                return true;
    
    return false;
}

CEntity* CEngine::DetectEntityCollision(const CEntity* ent, int x1, int y1, int w, int h, bool wantobstructable)
// returns the entity colliding with the specified entity, or 0 if none.
// Note that passing 0 for ent is valid, indicating that you simply want to know if there are any entities in a given area
{
    CDEBUG("detectentitycollision");
    
    for (EntityIterator i = entities.begin(); i != entities.end(); i++)
    {
        CEntity& e=**i;
        CSprite& s=*e.pSprite;

        if (wantobstructable && !e.bIsobs)   continue;
        if (&e == ent)                       continue;         // the entity is colliding with itself. ;P  That's not overly useful.
        
        if (x1              >= e.x+s.nHotw)  continue;
        if (y1              >= e.y+s.nHoth)  continue;
        if (x1+w            <= e.x)          continue;
        if (y1+h            <= e.y)          continue;
        
        return &e;
    }
    return 0;
}

void CEngine::TestActivate(const CEntity& player)
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
    CSprite& sprite = *player.pSprite;
    
    int tx = (player.x + sprite.nHotw / 2) / tiles->Width();
    int ty = (player.y + sprite.nHoth / 2) / tiles->Height();
    
    int n = map.GetZone(tx, ty);
    // stepping on a zone?
    if ((tx != nOldtx || ty != nOldty) && n)                        // the player is not on the same zone it was before, check for activation
    {
        nOldtx = tx; nOldty = ty;                            // if we don't do this, the next processentities will cause the zone to be activated again and again and again...
        const SMapZone& zone = map.GetZoneInfo(map.GetZone(tx, ty));
        if (((rand()%100) < zone.nActchance) && zone.sActscript.length())
            script.CallEvent(zone.sActscript.c_str());
    }
    
    nOldtx = tx; nOldty = ty;
    
    // adjacent activation
    
    if (!input.Enter().Pressed()) return;                           // From this point on, the only time we'd have to check this crap is if enter was pressed.
    //input.Unpress();
    
    tx = player.x; ty = player.y;
    // entity activation
    switch(player.direction)
    {
    case face_up:        ty -= sprite.nHoth;    break;
    case face_down:      ty += sprite.nHoth;    break;
    case face_left:      tx -= sprite.nHotw;    break;
    case face_right:     tx += sprite.nHotw;    break;
        
    case face_upleft:    tx -= sprite.nHotw;    ty -= sprite.nHoth;    break;
    case face_upright:   tx += sprite.nHotw;    ty -= sprite.nHoth;    break;
    case face_downleft:  tx -= sprite.nHotw;    ty += sprite.nHoth;    break;
    case face_downright: tx += sprite.nHotw;    ty += sprite.nHoth;    break;
    }
    
    CEntity* pEnt = DetectEntityCollision(0 , tx, ty, sprite.nHotw, sprite.nHoth);
    if (pEnt)
    {
        if (!pEnt->bAdjacentactivate && pEnt->sActscript.length() != 0)
        {
            script.CallEvent(pEnt->sActscript.c_str());
            input.Flush();
            return;
        }
    }
    
    // Activating a zone?
    uint z = map.GetZone(tx / tiles->Width(), ty / tiles->Height());
    
    if (z >= map.NumZones())
        return; // invalid zone

    const SMapZone& zone = map.GetZoneInfo(z);
    
    if (zone.bAdjacentactivation)
    {
        script.CallEvent(zone.sActscript.c_str());
        input.Flush();
    }
}

CEntity* CEngine::SpawnEntity()
{
    CEntity* e = new CEntity(this);
    
    entities.push_back(e);
    
    return e;
}

void CEngine::DestroyEntity(CEntity* e)
{
    for (EntityIterator i = entities.begin(); i != entities.end(); i++)
        if (e == (*i))
        {
            sprite.Free(e->pSprite);
            
            // important stuff, yo.  Need to find any existing pointers to this entity, and null them.
            if (pcameraTarget == e)   pcameraTarget = 0;
            if (pPlayer == e)         pPlayer = 0;
            
            // O(n**2) I think.  Gay.
            for (EntityIterator ii = entities.begin(); ii != entities.end(); ii++)
            {
                CEntity& e=*(*ii);
                
                if (e.pChasetarget == (*i))
                    e.pChasetarget = 0, e.movecode = mc_nothing;
                
                // TODO: add others, if needed
            }
            
            // actually nuke it
            entities.remove(e);
            delete e;
            return;
        }
        
    // In a Perfect World, this will never execute.
    Log::Write("Attempt to unallocate invalid entity!!!");
}

// --------------------------------- Misc (interface with old file formats, etc...) ----------------------

void CEngine::LoadMap(const char* filename)
// Most of the work involved here is storing the various parts of the v2-style map into memory under the new structure.
{
    CDEBUG("loadmap");
    
    try
    {
        Log::Write("Loading map \"%s\"", filename);
        
        if (!map.Load(filename)) throw filename;                        // actually load the map
        
        delete tiles;                                                   // nuke the old tileset
        tiles = new CTileSet(map.GetVSPName(), video);          // load up them tiles
        
        script.ClearEntityList();                                       // DEI
        
        for (uint i = 0; i < map.NumEnts(); i++)
        {
            const SMapEntity& ent = map.GetEntity(i);
            
            CEntity* pEnt = new CEntity(this, ent);                     // convert the old entity struct into the new one.
            entities.push_back(pEnt);
            
            pEnt->pSprite = sprite.Load(ent.sCHRname, video);                   // wee
            
            script.AddEntityToList(pEnt);
        }
        
        xwin = ywin = 0;                                                // just in case
        bMaploaded = true;
        
        if (!script.LoadMapScripts(filename))
            Script_Error();
    }
    catch (std::runtime_error err)
    {   Sys_Error(err.what());  }
    catch (const char* msg)
    {    Sys_Error(va("Failed to load %s", msg));    }
    catch (...)
    {    Sys_Error(va("Unknown error loading map %s", filename));    }
}

Point CEngine::GetCamera()
{
    return Point(xwin, ywin);
}

void CEngine::SetCamera(Point p)
{
    Point res = video->GetResolution();
    xwin = clamp(p.x, 0, map.Width()  * tiles->Width()  - res.x - 1);   // (tile width * number of tiles) - resolution - 1
    ywin = clamp(p.y, 0, map.Height() * tiles->Height() - res.y - 1);
}

CEngine::CEngine()
    : tiles(0)
    , video(0)
    , pPlayer(0)
    , pcameraTarget(0)
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
