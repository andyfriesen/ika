#include <cassert>
#include <stdexcept>
#include "tileset.h"

#include "common/log.h"
#include "common/utility.h"

#include "video/Driver.h"
#include "video/Image.h"

Tileset::Tileset(const std::string& fname, Video::Driver* v)
    : video(v)
    , animTimer(0)
{
    CDEBUG("ctileset::loadvsp");
    vsp = new VSP;
    
    if (!vsp->Load(fname)) {
        throw std::runtime_error("Unable to load VSP file " + fname);
    }
    
    frameCount = vsp->NumTiles();
    frameWidth = vsp->Width();
    frameHeight = vsp->Height();
    
    try {
        hFrame.resize(frameCount);
        for (int i = 0; i < frameCount; i++) {
            hFrame[i] = video->CreateImage(vsp->GetTile(i));
        }
    }
    catch(...) {	
        throw TilesetException();
    }
    
    // Next up, set up the tile animation stuff
    tileIndex.resize(frameCount);                               // Make the vectors fit
    flipFlag.resize(frameCount);
    for (int i = 0; i < frameCount; i++) {
        tileIndex[i] = i;                                       // set initial values for the vectors
        flipFlag[i] = false;
    }

    animstate = vsp->vspAnim;
    for (uint j = 0; j < vsp->vspAnim.size(); j++) {
        animstate[j].count = animstate[j].delay;                // init the counter
    }
}

Tileset::~Tileset() {
    for (int i = 0; i < frameCount; i++) {
        video->FreeImage(hFrame[i]);
    }
}

void Tileset::Save(const std::string& fileName) const {
    vsp->Save(fileName);
}

Video::Image* Tileset::GetTile(uint index) const {
    if (hFrame.empty()) return 0;

    if (index < 0 || index >= tileIndex.size()) index = 0;

    index = tileIndex[index];

    if (index < 0 || index >= tileIndex.size()) index = 0;

    return hFrame[index];
}

void Tileset::UpdateAnimation(int time) {
    int i = time - animTimer;					// how many ticks have elapsed?
    animTimer = time;
    if (i < 1) return;						// not very much, wait a little longer
    if (i > 100) i = 100;   // hack
    
    while (i--) {
        for (int j = 0; j < 100; j++) {
            if (animstate[j].start < animstate[j].finish) {
                AnimateStrand(animstate[j]);
            }
        }
    }
}

void Tileset::AnimateStrand(VSP::AnimState& anim) {
    anim.count--;
    
    if (anim.count > 0)	return;
    
    switch(anim.mode) {
        case VSP::linear: {
            for (int i = anim.start; i <= anim.finish; i++) {
                tileIndex[i]++;
                if (tileIndex[i] > anim.finish) {
                    tileIndex[i] = anim.start;
                }
            }
            break;
        }

        case VSP::reverse: {
            for (int i = anim.start; i <= anim.finish; i++) {
                tileIndex[i]--;
                if (tileIndex[i] < anim.start)
                    tileIndex[i] = anim.finish;
            }
            break;
        }

        case VSP::random: {
            for (int i = anim.start; i <= anim.finish; i++) {
                tileIndex[i] = Random(anim.start, anim.finish + 1);
            }
            break;
        }

        case VSP::flip: {
            for (int i = anim.start; i <= anim.finish; i++) {
                std::vector<bool>::reference flipped = flipFlag[i];
                std::vector<int>::reference index = tileIndex[i];

                if (flipped && index <= anim.start) {
                    flipped = false;
                } else if (!flipped && index >= anim.finish) {
                    flipped = true;
                }

                if (flipped) {
                    index--;
                } else {
                    index++;
                }
            }
            break;
        }

#if _DEBUG
        for (int i = anim.start; i <= anim.finish; i++) {
            assert(anim.start <= tileIndex[i] && tileIndex[i] <= anim.finish);
        }
#endif
    }
    
    anim.count = anim.delay;
}
