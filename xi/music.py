'''
Music handler... thingie.
'''

import ika

_cache_size = 4
_cache = []

def stopMusic():
    if len(_cache) > 0:
        fname, sound = _cache[0]
        sound.Pause()

def cacheMusic(fileName):
    assert isinstance(fileName, str)

    i = 0
    while i < len(_cache):
        name, sound = _cache[i]
        if name == fileName:
            # move it to the head of the list
            _cache.pop(i)
            _cache.insert(0, (name, sound))
            return sound
        i += 1

    # if execution has reached this point, then we have to load it now
    while len(_cache) > _cache_size:
        _cache.pop() # discard the oldest

    sound = ika.Sound(fileName)
    sound.loop = True
    _cache.insert(0, (fileName, sound))
    return sound

def playMusic(fileName):
    stopMusic()

    sound = cacheMusic(fileName)
    assert sound is not None
    sound.Play()
