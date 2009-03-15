#ifndef SCRIPT_OBJECTDEFS_H
#define SCRIPT_OBJECTDEFS_H

#define METHOD(_Method, _SelfType) PyObject* _Method(_SelfType* self, PyObject* args)
#define METHOD1(_Method, _SelfType) PyObject* _Method(_SelfType* self)

#include "Python.h"
#include <sstream>
#include <map>

// Rain of prototypes
namespace Ika {  // X11/SDL fix
    struct Font;
}
struct Entity;
struct Engine;
struct Canvas;

struct Input;
struct InputDevice;
struct InputControl;
struct Keyboard;
struct Mouse;
struct Joystick;
struct Tileset;

namespace audiere   {   
    class OutputStream; 
    class SoundEffect;
}

namespace Video {
    struct Driver;
    struct Image;
}

struct ColourHandler;

/// Contains implementations of Python binding things.
namespace Script {
    // Hardware interfaces
    /// Reflects an image.
    namespace Image {
        // object type
        struct ImageObject {
            PyObject_HEAD
            ::Video::Image* img;
        };

        // Methods
        METHOD(Image_Blit, ImageObject);
		METHOD(Image_ClipBlit, ImageObject);
        METHOD(Image_ScaleBlit, ImageObject);
		METHOD(Image_RotateBlit, ImageObject);
        METHOD(Image_DistortBlit, ImageObject);
		METHOD(Image_TileBlit, ImageObject);
		METHOD(Image_TintBlit, ImageObject);
		METHOD(Image_TintDistortBlit, ImageObject);
		METHOD(Image_TintTileBlit, ImageObject);
        METHOD(Image_Clip, ImageObject);

        void Init();
        PyObject* New(::Video::Image* image);
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(ImageObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects a sound stream.
    namespace Music {
        // Object type
        struct MusicObject {
            PyObject_HEAD
            audiere::OutputStream* music;
        };

        // Methods
        METHOD1(Music_Play, MusicObject);
        METHOD1(Music_Pause, MusicObject);

        void Init();
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(MusicObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace Sound {
        // Object type
        struct SoundObject {
            PyObject_HEAD
            audiere::SoundEffect* sound;
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
    namespace Control {
        // Object type
        struct ControlObject {
            PyObject_HEAD
            InputControl* control;
        };

        // Methods
        METHOD1(Control_Pressed, ControlObject);
        METHOD1(Control_Position, ControlObject);
        METHOD1(Control_Delta, ControlObject);

        void Init();
        void Destroy(ControlObject* self);
        PyObject* New(InputControl* control);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    // Engine resources
    /// Reflects a canvas. (software image surface thingie)
    namespace Canvas {
        // Object type
        struct CanvasObject {
            PyObject_HEAD
            bool ref;
            ::Canvas* canvas;
        };

        // Methods
        METHOD(Canvas_Save, CanvasObject);
        METHOD1(Canvas_AlphaMask, CanvasObject);
        METHOD(Canvas_Blit, CanvasObject);
        METHOD(Canvas_ScaleBlit, CanvasObject);
        METHOD(Canvas_TileBlit, CanvasObject);
        METHOD(Canvas_GetPixel, CanvasObject);
        METHOD(Canvas_SetPixel, CanvasObject);
        METHOD(Canvas_DrawLine, CanvasObject);
        METHOD(Canvas_DrawRect, CanvasObject);
        METHOD(Canvas_DrawText, CanvasObject);
        METHOD(Canvas_Clear, CanvasObject);
        METHOD(Canvas_Resize, CanvasObject);
        METHOD(Canvas_Clip, CanvasObject);
        METHOD1(Canvas_Rotate, CanvasObject);
        METHOD1(Canvas_Flip, CanvasObject);
        METHOD1(Canvas_Mirror, CanvasObject);

        void Init();
        PyObject* New(::Canvas* c);
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(CanvasObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects a bitmap font. (maybe a std::vector font later on)
    namespace Font {
        // Object type
        struct FontObject {
            PyObject_HEAD
            Ika::Font* font;
        };

        // Methods
        METHOD(Font_Print, FontObject);
        METHOD(Font_CenterPrint, FontObject);
        METHOD(Font_RightPrint, FontObject);
        METHOD(Font_StringWidth, FontObject);
        METHOD(Font_StringHeight, FontObject);

        void Init();
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(FontObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects a map entity.
    namespace Entity {
        // Object type
        struct EntityObject {
            PyObject_HEAD
            ::Entity* ent;
        };

        extern std::map< ::Entity*, EntityObject*> instances;

        // Methods
        METHOD(Entity_MoveTo, EntityObject);
        METHOD(Entity_Wait, EntityObject);
        METHOD(Entity_Stop, EntityObject);
        METHOD(Entity_IsMoving, EntityObject);
        METHOD(Entity_DetectCollision, EntityObject);
        METHOD(Entity_Touches, EntityObject);
        METHOD(Entity_Render, EntityObject);
        METHOD(Entity_Draw, EntityObject);
        METHOD(Entity_Update, EntityObject);
        METHOD(Entity_GetAnimScript, EntityObject);
        METHOD(Entity_GetAllAnimScripts, EntityObject);

        void Init();
        PyObject* New(::Entity* ent);
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(EntityObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace InputDevice {
        struct DeviceObject {
            PyObject_HEAD
            ::InputDevice* device;
        };

        // Methods
        METHOD1(Device_Update, DeviceObject);
        METHOD(Device_GetControl, DeviceObject);

        void Init();
        PyObject* New(::InputDevice* device);
        void Destroy(DeviceObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace Keyboard {
        METHOD1(Keyboard_GetKey, PyObject);
        METHOD1(Keyboard_WasKeyPressed, PyObject);
        METHOD1(Keyboard_ClearKeyQueue, PyObject);

        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace Mouse {
        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace Joystick {
        struct JoystickObject {
            PyObject_HEAD
            ::Joystick* joystick;

            // Tuples containing all the controls.
            PyObject* axes;
            PyObject* reverseAxes;
            PyObject* buttons;
        };

        void Init();
        PyObject* New(::Joystick* joystick);
        void Destroy(JoystickObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    // Singletons
    /// Reflects the input core.
    namespace Input {
        // Object type
        struct InputObject {
            PyObject_HEAD
            PyObject* keyboard;
            PyObject* mouse;
            PyObject* joysticks; // tuple containing joystick objects
        };

        // Methods
        METHOD1(Input_Update, InputObject);
        METHOD1(Input_Unpress, InputObject);

        void Init();
        PyObject* New();
        void Destroy(InputObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects the video driver.
    namespace Video {
        // Object type
        struct VideoObject {
            PyObject_HEAD
            ::Video::Driver* video;
        };

        // Methods
        METHOD(Video_Blit, VideoObject);
        METHOD(Video_ClipBlit, VideoObject);
        METHOD(Video_ScaleBlit, VideoObject);
		METHOD(Video_RotateBlit, VideoObject);
        METHOD(Video_DistortBlit, VideoObject);
        METHOD(Video_TileBlit, VideoObject);
        METHOD(Video_TintBlit, VideoObject);
        METHOD(Video_TintDistortBlit, VideoObject);
        METHOD(Video_TintTileBlit, VideoObject);
        METHOD(Video_DrawPixel, VideoObject);
        METHOD(Video_DrawLine, VideoObject);
        METHOD(Video_DrawRect, VideoObject);
        METHOD(Video_DrawEllipse, VideoObject);
        METHOD(Video_DrawArc, VideoObject);
        METHOD(Video_DrawTriangle, VideoObject);
        METHOD(Video_DrawQuad, VideoObject);
        METHOD(Video_DrawLineList, VideoObject);
        METHOD(Video_DrawTriangleList, VideoObject);
        METHOD(Video_ClipScreen, VideoObject);
        METHOD1(Video_GetClipRect, VideoObject);
        METHOD(Video_GrabImage, VideoObject);
        METHOD(Video_GrabCanvas, VideoObject);
        METHOD1(Video_ClearScreen, VideoObject);
        METHOD1(Video_ShowPage, VideoObject);
        //METHOD(Video_RenderTo, VideoObject);
        METHOD(Video_SetResolution, VideoObject);

        void Init();
        PyObject* New(::Video::Driver* v);
        void Destroy(VideoObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }
    
    namespace Colours {
        // Object type
        struct ColoursObject {
            PyObject_HEAD
            ::ColourHandler* handler;
        };
        
        METHOD(Colours_HasKey, ColoursObject);
        METHOD1(Colours_Keys, ColoursObject);
        METHOD1(Colours_Copy, ColoursObject);
        METHOD1(Colours_Items, ColoursObject);
        
        void Init();
        PyObject* New(::ColourHandler* device);
        void Destroy(ColoursObject* self);

        // Method table
        extern PyTypeObject type;
    }

    /// Reflects the current map, and some aspects of the engine.
    namespace Map {
        // Object type

        // Methods
		METHOD(Map_Save, PyObject);
        METHOD(Map_Switch, PyObject);
        METHOD(Map_GetMetaData, PyObject);
        METHOD(Map_Render, PyObject);
        METHOD(Map_GetTile, PyObject);
        METHOD(Map_SetTile, PyObject);
        METHOD(Map_GetObs, PyObject);
        METHOD(Map_SetObs, PyObject);
        METHOD(Map_GetZone, PyObject);
        METHOD(Map_SetZone, PyObject);
        METHOD(Map_GetLayerName, PyObject);
        METHOD(Map_SetLayerName, PyObject);
        METHOD(Map_GetLayerTint, PyObject);
        METHOD(Map_SetLayerTint, PyObject);
        METHOD(Map_FindLayerByName, PyObject);
        METHOD(Map_GetParallax, PyObject);
        METHOD(Map_SetParallax, PyObject);
        METHOD(Map_GetLayerProperties, PyObject);
        METHOD(Map_GetLayerPosition, PyObject);
        METHOD(Map_SetLayerPosition, PyObject);
        METHOD(Map_GetZones, PyObject);
        METHOD(Map_GetWaypoints, PyObject);
        METHOD(Map_GetAllEntities, PyObject);

        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace Tileset {
        // object type
        struct TilesetObject {
            PyObject_HEAD
            //::Tileset* tileset;
        };

        // methods
        METHOD(Tileset_Save, TilesetObject);
        METHOD(Tileset_Load, TilesetObject);

        void Init();
        PyObject* New();//::Tileset* tileset);
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(TilesetObject* self);

        // method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// lil' Python object used to redirect error messages to pyout.log
    namespace Error {
        // Object type

        // Methods
        METHOD(Error_Write, PyObject);

        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    // "Global" things.
    // Definitely not my favourite way to go about this.
    // These are defined in ModuleFuncs.cpp, by the by.
    extern Engine*    engine;

    extern PyObject*   entityDict;
    extern PyObject*   playerEnt;
    extern PyObject*   cameraTarget;

    extern PyObject*   sysModule;                           // the system scripts (system.py)
    extern PyObject*   mapModule;                           // scripts for the currently loaded map

    extern std::stringstream  pyOutput;                     // Python's sys.stdout and sys.stderr go here (defined in ModuleFuncs.cpp)

    METHOD(ika_log, PyObject);
    METHOD(ika_exit, PyObject);
    METHOD1(ika_getcaption, PyObject);
    METHOD(ika_setcaption, PyObject);
    METHOD1(ika_getframerate, PyObject);
    METHOD(ika_delay, PyObject);
    METHOD(ika_wait, PyObject);
    METHOD1(ika_gettime, PyObject);
    METHOD(ika_random, PyObject);

    METHOD(ika_showpage, PyObject);
    METHOD(ika_rgb, PyObject);
    METHOD(ika_getrgb, PyObject);
    METHOD(ika_palettemorph, PyObject);

    METHOD(ika_processentities, PyObject);
    METHOD(ika_setcameraTarget, PyObject);
    METHOD1(ika_getcameraTarget, PyObject);
    METHOD(ika_setplayer, PyObject);
    METHOD1(ika_getplayer, PyObject);
    METHOD(ika_entitiesat, PyObject);

    METHOD(ika_hookretrace, PyObject);
    METHOD(ika_unhookretrace, PyObject);
    METHOD(ika_hooktimer, PyObject);
    METHOD(ika_unhooktimer, PyObject);

    METHOD(ika_setrenderlist, PyObject);

    METHOD(ika_render, PyObject);

    METHOD(ika_setmappath, PyObject);

    extern PyMethodDef standard_methods[];

#undef METHOD
#undef METHOD1
}


#endif
