
#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include "python.h"

/*
    Due to the way the Python API is structured, the script engine simply will not support multiple instances.
    Don't even try it. ^_~

    um... I think this is technically a singleton. :o

    A very, very good idea to try would be to make a template class for all these Python objects.
    
    I hate this.  It's retarded.  Either wrap it in a namespace and live with its globalness, or
    figure out how to do it the Right Way.
*/

class CEngine;                                                // proto

class CScriptEngine
{
    std::string sCurmapscript;

    static CEngine* pEngine;

    static PyObject*    pScreenobject;
    static PyObject*    pEntitydict;
    static PyObject*    pRenderdest;
    static PyObject*    pPlayerent;
    static PyObject*    pCameratarget;
    static PyObject*    pErrorhandler;

    static PyObject*    pSysmodule;
    static PyObject*    pMapmodule;

    bool bInited;

public:
    CScriptEngine() { bInited=false; }

    void Init(CEngine* pEngine);                            // -_-
    void Shutdown();

    bool LoadSystemScripts(char* fname);
    bool LoadMapScripts(const char* fname);

    bool ExecFunction(void* pFunc);

    void ClearEntityList();
    void AddEntityToList(class CEntity* e);

    void CallEvent(const char* sName);

    // ----------------------- OBJECTS AND STUFF -----------------------
private:
    // Object definition structs
    static PyMethodDef  image_methods[];
    static PyTypeObject imagetype;
    static PyMethodDef  entity_methods[];
    static PyTypeObject entitytype;
    static PyMethodDef  music_methods[];
    static PyTypeObject musictype;
    static PyMethodDef  sound_methods[];
    static PyTypeObject soundtype;
    static PyMethodDef  font_methods[];
    static PyTypeObject fonttype;

    // Singleton interface thingies
    static PyMethodDef  map_methods[];
    static PyTypeObject maptype;
    static PyMethodDef  input_methods[];
    static PyTypeObject inputtype;
    static PyMethodDef  error_methods[];
    static PyTypeObject errortype;
    
public:                                                        // too bad

    // Image objects
    void Init_Image();
    static PyObject* Image_New(PyObject* self,PyObject* args);
    static void      Image_Destroy(PyObject* self);
    static PyObject* Image_GetAttribute(PyObject* self,char* name);

    // Entity objects
    void Init_Entity();
    static PyObject* Entity_New(class CEntity* e);                                      // never called by Python, only in C++
    static void      Entity_Destroy(PyObject* self);
    static PyObject* Entity_GetAttribute(PyObject* self,char* name);
    static int       Entity_SetAttribute(PyObject* self,char* name,PyObject* value);

    // Music objects
    void Init_Music();
    static PyObject* Music_New(PyObject* self,PyObject* args);
    static void      Music_Destroy(PyObject* self);
    static PyObject* Music_GetAttribute(PyObject* self,char* name);
    static int       Music_SetAttribute(PyObject* self,char* name,PyObject* value);

    // Sound (.WAV) objects
    void Init_Sound();
    static PyObject* Sound_New(PyObject* self,PyObject* args);
    static void      Sound_Destroy(PyObject* self);
    static PyObject* Sound_GetAttribute(PyObject* self,char* name);
    static int       Sound_SetAttribute(PyObject* self,char* name,PyObject* value);

    // Fonts
    void Init_Font();
    static PyObject* Font_New(PyObject* self,PyObject* args);
    static void      Font_Destroy(PyObject* self);
    static PyObject* Font_GetAttribute(PyObject* self,char* name);
    static PyObject* Font_SetAttribute(PyObject* self,char* name,PyObject* value);

    // Singleton objects
    // map
    void Init_Map();
    static PyObject* Map_New();
    static void      Map_Destroy(PyObject* self);
    static PyObject* Map_GetAttribute(PyObject* self,char* name);
    static int       Map_SetAttribute(PyObject* self,char* name,PyObject* value);

    // input
    void Init_Input();
    static PyObject* Input_New();
    static void      Input_Destroy(PyObject* self);
    static PyObject* Input_GetAttribute(PyObject* self,char* name);
    static int       Input_SetAttribute(PyObject* self,char* name,PyObject* value);    
    
    // error handling
    void Init_Error();
    static PyObject* Error_New();
    static void      Error_Destroy(PyObject* self);
    static PyObject* Error_GetAttribute(PyObject* self,char* name);

    #define METHOD(x) static PyObject* x (PyObject* self,PyObject* args)

    // Core API
    METHOD(std_log);
    METHOD(std_exit);
    METHOD(std_wait);
    METHOD(std_delay);
    METHOD(std_gettime);
    METHOD(std_random);

    METHOD(std_loadimage);

    METHOD(std_showpage);
    METHOD(std_setrenderdest);
    METHOD(std_getrenderdest);
    METHOD(std_getscreenimage);
    METHOD(std_palettemorph);
    METHOD(std_rgb);
    METHOD(std_getrgb);

    METHOD(std_processentities);
    METHOD(std_spawnentity);
    METHOD(std_setplayer);
    METHOD(std_setcameratarget);
    METHOD(std_getcameratarget);

    METHOD(std_hookbutton);
    METHOD(std_hookretrace);
    METHOD(std_unhookretrace);
    METHOD(std_hooktimer);
    METHOD(std_unhooktimer);

    // Image methods
    METHOD(image_load);
    METHOD(image_copy);
    METHOD(image_blit);
    METHOD(image_scaleblit);
    METHOD(image_copychannel);
    METHOD(image_clip);
    METHOD(image_line);
    METHOD(image_rect);
    METHOD(image_ellipse);
    METHOD(image_setpixel);
    METHOD(image_getpixel);
    METHOD(image_flatpoly);

    // Entity methods
    METHOD(entity_move);
    METHOD(entity_chase);
    METHOD(entity_wander);
    METHOD(entity_wanderzone);
    METHOD(entity_stop);
    METHOD(entity_ismoving);
    METHOD(entity_detectcollision);

    // Music methods
    METHOD(music_play);
    METHOD(music_pause);

    // Sound method :)
    METHOD(sound_play);

    // Font methods
    METHOD(font_print);
    METHOD(font_centerprint);
    METHOD(font_rightprint);
    METHOD(font_stringwidth);

    // Map methods
    METHOD(map_switch);
    METHOD(map_render);
    METHOD(map_gettile);
    METHOD(map_settile);
    METHOD(map_getobs);
    METHOD(map_setobs);
    METHOD(map_getzone);
    METHOD(map_setzone);
    METHOD(map_getparallax);
    METHOD(map_setparallax);

    // Input methods
    METHOD(input_update);
    METHOD(input_button);
    METHOD(input_setbutton);
    METHOD(input_nextpressed);
    METHOD(input_setnextpressed);
    METHOD(input_mousestate);
    METHOD(input_setmousestate);
    METHOD(input_clipmouse);
    METHOD(input_bindkey);
    METHOD(input_unbindkey);

    // error method
    METHOD(error_write);

    #undef METHOD

};

/*
    FIXME:
    Some of hese objects can't have properties added to them at runtime.
    This is unacceptable. -_-

    Objects:

    Image            check!    (extra blits)
    Font             check!
    Entity           check!    (needs extra robustness)
    Music            check!
    Sound effects    check!
*/

#endif