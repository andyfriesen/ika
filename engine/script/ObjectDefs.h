#ifndef SCRIPT_OBJECTDEFS_H
#define SCRIPT_OBJECTDEFS_H

#define METHOD(x, type) PyObject* x(type* self, PyObject* args)
#define METHOD1(x, type) PyObject* x(type* self)

// Rain of prototypes
#include <python/Python.h>
#include "input.h"
class CFont;
class CEntity;
class CEngine;
class Canvas;
namespace audiere   {   class OutputStream; }
namespace Video
{
    class Driver;
    class Image;
}

/// Contains implementations of Python binding things.
namespace Script
{
    // Hardware interfaces
    /// Reflects an image.
    namespace Image
    {
        // object type
        struct ImageObject
        {
            PyObject_HEAD
            ::Video::Image* img;
        };

        // Methods
        METHOD(Image_Blit, ImageObject);
        METHOD(Image_ScaleBlit, ImageObject);
        METHOD(Image_DistortBlit, ImageObject);
        METHOD(Image_Clip, ImageObject);

        void Init();
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(ImageObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects a sound stream.
    namespace Sound
    {
        // Object type
        struct SoundObject
        {
            PyObject_HEAD
            audiere::OutputStream* sound;
        };

        // Methods
        METHOD1(Sound_Play, SoundObject);
        METHOD1(Sound_Pause, SoundObject);

        void Init();
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(SoundObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects an input control.
    namespace Control
    {
        // Object type
        struct ControlObject
        {
            PyObject_HEAD
            Input::Control* control;
        };

        // Methods
        METHOD1(Control_Pressed, ControlObject);
        METHOD1(Control_Position, ControlObject);
        METHOD1(Control_Delta, ControlObject);

        void Init();
        void Destroy(ControlObject* self);
        PyObject* New(Input& input, const char* name);

        // Method table
        extern PyMethodDef methods[];
    }

    // Engine resources
    /// Reflects a canvas. (software image surface thingie)
    namespace Canvas
    {
        // Object type
        struct CanvasObject
        {
            PyObject_HEAD
            bool ref;
            ::Canvas* canvas;
        };
        extern PyTypeObject type;
        extern PyMethodDef methods[];
        
        // Methods
        METHOD(Canvas_Save, CanvasObject);
        METHOD(Canvas_Blit, CanvasObject);
        METHOD(Canvas_ScaleBlit, CanvasObject);
        METHOD(Canvas_GetPixel, CanvasObject);
        METHOD(Canvas_SetPixel, CanvasObject);
        METHOD(Canvas_Clear, CanvasObject);
        METHOD(Canvas_Resize, CanvasObject);
        METHOD1(Canvas_Rotate, CanvasObject);
        METHOD1(Canvas_Flip, CanvasObject);
        METHOD1(Canvas_Mirror, CanvasObject);

        void Init();
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(CanvasObject* self);
    }

    /// Reflects a bitmap font. (maybe a vector font later on)
    namespace Font
    {
        // Object type
        struct FontObject
        {
            PyObject_HEAD
            CFont* font;
        };

        // Methods
        METHOD(Font_Print, FontObject);
        METHOD(Font_CenterPrint, FontObject);
        METHOD(Font_RightPrint, FontObject);
        METHOD(Font_StringWidth, FontObject);

        void Init();
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(FontObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects a map entity.
    namespace Entity
    {
        extern PyTypeObject type;

        // Object type
        struct EntityObject
        {
            PyObject_HEAD
            CEntity* ent;
        };

        // Methods
        METHOD(Entity_Move, EntityObject);
        METHOD(Entity_Chase, EntityObject);
        METHOD(Entity_Wander, EntityObject);
        METHOD(Entity_Wanderzone, EntityObject);
        METHOD(Entity_Stop, EntityObject);
        METHOD(Entity_IsMoving, EntityObject);
        METHOD(Entity_DetectCollision, EntityObject);

        void Init();
        PyObject* New(CEntity* ent);
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(EntityObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    // Singletons
    /// Reflects the input core.
    namespace Input
    {
        // Object type
        struct InputObject
        {
            PyObject_HEAD
            ::Input* input;
        };

        // Methods
        METHOD1(Input_Update, InputObject);

        void Init();
        PyObject* New(::Input& i);
        void Destroy(InputObject* self);

        // Method table
        extern PyMethodDef methods[];
    }

    /// Reflects the video driver.
    namespace Video
    {
        // Object type
        struct VideoObject
        {
            PyObject_HEAD
            ::Video::Driver* video;
        };

        // Methods
        METHOD(Video_Blit, VideoObject);
        METHOD(Video_ScaleBlit, VideoObject);
        METHOD(Video_DistortBlit, VideoObject);
        METHOD(Video_TileBlit, VideoObject);
        METHOD(Video_DrawPixel, VideoObject);
        METHOD(Video_DrawLine, VideoObject);
        METHOD(Video_DrawRect, VideoObject);
        METHOD(Video_DrawEllipse, VideoObject);
        METHOD(Video_DrawTriangle, VideoObject);
        METHOD1(Video_ShowPage, VideoObject);

        void Init();
        PyObject* New(::Video::Driver* v);
        void Destroy(VideoObject* self);

        // Method table
        extern PyMethodDef methods[];
    }

    /// Reflects the current map, and some aspects of the engine.
    namespace Map
    {
        // Object type

        // Methods
        METHOD(Map_Switch, PyObject);
        METHOD(Map_Render, PyObject);
        METHOD(Map_GetTile, PyObject);
        METHOD(Map_SetTile, PyObject);
        METHOD(Map_GetObs, PyObject);
        METHOD(Map_SetObs, PyObject);
        METHOD(Map_GetZone, PyObject);
        METHOD(Map_SetZone, PyObject);
        METHOD(Map_GetParallax, PyObject);
        METHOD(Map_SetParallax, PyObject);

        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        // Method table
        extern PyMethodDef methods[];
    }

    /// lil' Python object used to redirect error messages to pyout.log
    namespace Error
    {
        // Object type

        // Methods
        METHOD(Error_Write, PyObject);

        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        // Method table
        extern PyMethodDef methods[];
    }

    // "Global" things.
    // Definitely not my favourite way to go about this.
    // These are defined in ModuleFuncs.cpp, by the by.
    extern CEngine*    engine;

    extern PyObject*   entitydict;
    extern PyObject*   playerent;
    extern PyObject*   cameratarget;
    extern PyObject*   errorhandler;

    extern PyObject*   sysmodule;                         // the system scripts (system.py)
    extern PyObject*   mapmodule;                         // scripts for the currently loaded map

    METHOD(std_log, PyObject);
    METHOD(std_exit, PyObject);
    METHOD1(std_getcaption, PyObject);
    METHOD(std_setcaption, PyObject);
    METHOD1(std_getframerate, PyObject);
    METHOD(std_delay, PyObject);
    METHOD(std_wait, PyObject);
    METHOD1(std_gettime, PyObject);
    METHOD(std_random, PyObject);

    METHOD(std_showpage, PyObject);
    METHOD(std_rgb, PyObject);
    METHOD(std_getrgb, PyObject);
    METHOD(std_palettemorph, PyObject);

    METHOD(std_processentities, PyObject);
    METHOD(std_setcameratarget, PyObject);
    METHOD1(std_getcameratarget, PyObject);
    METHOD(std_setplayer, PyObject);
    METHOD1(std_getplayer, PyObject);
    METHOD(std_entityat, PyObject);

    METHOD(std_hookbutton, PyObject);
    METHOD(std_hookretrace, PyObject);
    METHOD(std_unhookretrace, PyObject);
    METHOD(std_hooktimer, PyObject);
    METHOD(std_unhooktimer, PyObject);

    extern PyMethodDef standard_methods[];

#undef METHOD
#undef METHOD1
}


#endif
