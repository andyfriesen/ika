#ifndef SCRIPT_OBJECTDEFS_H
#define SCRIPT_OBJECTDEFS_H

#define METHOD(x, type) PyObject* x(type* self, PyObject* args)
#define METHOD1(x, type) PyObject* x(type* self)

// Rain of prototypes
#include <Python.h>
#include <sstream>

struct MainWindow;
struct Map;

/// Contains implementations of Python binding things.
namespace ScriptObject
{
    /// Reflects the current map, and some aspects of the engine.
    namespace Map
    {
        // object type
        struct MapObject
        {
            PyObject_HEAD
            MainWindow* mainWnd;
            ::Map* GetMap() const;
        };

        // Methods
        METHOD(Map_GetTile, MapObject);
        //METHOD(Map_SetTile, MapObject);
        METHOD(Map_PlaceBrush, MapObject);
        METHOD(Map_GetObs, MapObject);
        METHOD(Map_SetObs, MapObject);
        METHOD(Map_GetMetaData, MapObject);
        METHOD(Map_SetMetaData, MapObject);
        METHOD(Map_GetLayerName, MapObject);
        METHOD(Map_GetLayerSize, MapObject);
        METHOD(Map_FindLayerByName, MapObject);
        METHOD(Map_GetParallax, MapObject);
        METHOD(Map_SetParallax, MapObject);

        METHOD(Map_AddLayer, MapObject);
        METHOD(Map_DestroyLayer, MapObject);
        METHOD(Map_ResizeLayer, MapObject);
        METHOD(Map_SwapLayers, MapObject);
        METHOD(Map_CloneLayer, MapObject);
        METHOD(Map_GetLayerProperties, MapObject);
        METHOD(Map_SetLayerProperties, MapObject);

        void Init();
        PyObject* New(MainWindow* mainWnd);
        void Destroy(PyObject* self);

        // Method table
        extern PyMethodDef methods[];
    }

    namespace Brush
    {
        struct BrushObject
        {
            PyObject_HEAD
            MainWindow* mainWnd;
        };

        // Methods
        METHOD(Brush_Resize, BrushObject);
        METHOD1(Brush_Width, BrushObject);
        METHOD1(Brush_Height, BrushObject);
        METHOD(Brush_GetIndex, BrushObject);
        METHOD(Brush_SetIndex, BrushObject);

        void Init();
        PyObject* New(MainWindow* mainWnd);
        void Destroy(PyObject* self);

        // Method table
        extern PyMethodDef methods[];
    }

    namespace Editor
    {
        struct EditorObject
        {
            PyObject_HEAD
            PyObject* brush;
            MainWindow* mainWnd;
        };

        // Methods
        METHOD(Editor_MapToTile, EditorObject);
        METHOD(Editor_TileToMap, EditorObject);
        METHOD1(Editor_GetMouseState, EditorObject);

        void Init();
        PyObject* New(MainWindow* mainWnd);
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

    extern std::stringstream pyErrors;

    METHOD(std_log, PyObject);
    //METHOD(std_exit, PyObject);
    METHOD(std_redraw, PyObject);

    //METHOD(std_entityat, PyObject);

    extern PyMethodDef standard_methods[];

#undef METHOD
#undef METHOD1
}


#endif
