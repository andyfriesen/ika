
#pragma once

#include <set>

#include "common/utility.h"
#include "document.h"

struct Canvas;

namespace iked {

    struct Image;

    /**
     * A document that is an array of images.
     * Handles image generation, and updating of those images
     * when changes to the underlying canvases occurs.
     *
     * Width and height should return the width/height of the 
     * biggest image in the array. (easy most of the time, when
     * all the images are of the same size)
     *
     * IMPORTANT: call flush() at the end of the base class constructor.
     */
    struct ImageArrayDocument : AbstractDocument {
        ImageArrayDocument(const std::string& fileName);
        virtual ~ImageArrayDocument();

        int getCount();
        int getWidth();
        int getHeight();
        const Canvas& getCanvas(int idx);

        void update(const Canvas& img, int idx);
        void insert(const Canvas& canvas, int position);
        void remove(int position);

        void append(const Canvas& canvas) {
            insert(canvas, getCount());
        }

        Image& getImage(int idx);

    protected:
        // Call this when the actual ordering of the images has changed.
        void flush();

        // Abstract part that must be implemented by subclasses.
        virtual int doGetCount() = 0;
        virtual int doGetWidth() = 0;
        virtual int doGetHeight() = 0;
        virtual const Canvas& doGetCanvas(int index) = 0;
        virtual void doSetCanvas(const Canvas& canvas, int index) = 0;
        virtual void doInsert(const Canvas& canvas, int position) = 0;
        virtual void doRemove(int position) = 0;
        // --

    private:
        bool isDirty(int index);
        void makeDirty(int index);

        void sync(int index);
        void syncAll();
        void free();

        typedef std::vector<Image*> ImageList;
        typedef std::set<int> DirtyList;
            
        ImageList images;
        DirtyList dirty;

#ifdef DEBUG
        void invariant();
#else
        inline void invariant() {}
#endif
    };
}
