
#ifdef _DEBUG

#include <list>
#include "log.h"

struct sMemblock {
    char fileName[255];
    int lineNo;
    int size;
    void* ptr;
};

static std::list<sMemblock*>* memblocks = 0;

void AddChunk(const char* fileName, int lineNo, int size, void* p) {
    if (!memblocks) {
        memblocks = new(std::list<sMemblock*>);
    }

    sMemblock* m = new (sMemblock);

    strcpy(m->fileName, fileName);
    m->lineNo = lineNo;
    m->size = size;
    m->ptr = p;

    memblocks->push_back(m);
}

void NukeChunk(void* p)
{
    std::list<sMemblock*>::iterator i;

    if (memblocks == 0) {
        return;
    }

    for (i = memblocks->begin(); i!=memblocks->end(); i++) {
        if ((*i)->ptr == p) {
            memblocks->remove(*i);
            return;
        }
    }
}

void* operator new (unsigned int size, const char* fileName, int lineNo) {
    void* p = malloc(size);

    AddChunk(fileName, lineNo, size, p);

    return p;
}

void operator delete(void* p) {
    if (p) {
        NukeChunk(p);
        free(p);
    }
}

void DumpList() {
    if (!memblocks) {
        return;
    }

    std::list<sMemblock*>::iterator i;
    for (i = memblocks->begin(); i != memblocks->end(); i++) {
        Log::Write(
            "File: %s\tLine: %i\tSize: %i\tLocation: %i", 
            (*i)->fileName, (*i)->lineNo, (*i)->size, (int)((*i)->ptr)
        );
        //memblocks->remove(*i);
    }
    Log::Write("done!");
}

#else

// Release version is a no-op
void DumpList() {
}

#endif	// #ifdef _DEBUG
