#include "imagebank.h"
#include "graph.h"

namespace iked {

    ImageArrayDocument::ImageArrayDocument(const std::string& fileName)
        : AbstractDocument(fileName) 
    { }

    ImageArrayDocument::~ImageArrayDocument() {
        free();
    }

    int ImageArrayDocument::getCount() {
        return doGetCount();
    }

    int ImageArrayDocument::getWidth() {
        return doGetWidth();
    }

    int ImageArrayDocument::getHeight() {
        return doGetHeight();
    }

    const Canvas& ImageArrayDocument::getCanvas(int index) {
        assert(0 <= index && index < getCount());
        return doGetCanvas(index);
    }

    /*
    void ImageArrayDocument::insert(const Canvas& canvas, int position) {
        assert(0 <= position && position <= getCount());
        doInsert(canvas, position);
        images.insert(images.begin() + position, 0);
    }

    void ImageArrayDocument::setCanvas(const Canvas& canvas, int index) {
        assert(0 <= index && index < getCount());
        makeDirty(index);
        doSetCanvas(canvas, index);
    }
    */

    Image& ImageArrayDocument::getImage(int index) {
        assert(0 <= index && index < getCount());
        sync(index);
        return *images[index];
    }

    void ImageArrayDocument::flush() {
        free();
        images.resize(getCount());
    }

    bool ImageArrayDocument::isDirty(int index) {
        assert(0 <= index && index < getCount());
        return dirty.count(index) != 0;
    }

    void ImageArrayDocument::makeDirty(int index) {
        assert( 0 <= index && index < getCount());
        if (getCount() != images.size()) {
            // Image lists aren't the same size.  Assume the worst:
            flush();
        }
        dirty.insert(index);
    }

    void ImageArrayDocument::sync(int index) {
        invariant();
        assert(0 <= index && index < getCount());
        if (dirty.count(index)) {
            assert(images[index] != 0);
            images[index]->Update(getCanvas(index));
            dirty.erase(index);
        } else if (images[index] == 0) {
            images[index] = new Image(getCanvas(index));
        } else {
            ; // no-op
        }
    }

    void ImageArrayDocument::syncAll() {
        foreach (int index, dirty) {
            assert(0 <= index && index < getCount());
            images[index]->Update(getCanvas(index));
        }

        dirty.clear();
    }


    void ImageArrayDocument::free() {
        foreach (Image*& img, images) {
            delete img;
            img = 0;
        }
    }

#ifdef DEBUG
    void ImageArrayDocument::invariant() {
        assert(images.size() == getCount());
    }
#endif
}
