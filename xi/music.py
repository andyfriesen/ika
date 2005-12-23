#!/usr/bin/env python

"""Music handler."""

import ika


_cache_size = 4
_cache = []


def stopMusic():
    if len(_cache) > 0:
        filename, music = _cache[0]
        music.Pause()


def cacheMusic(filename):
    assert isinstance(filename, basestring)
    i = 0
    while i < len(_cache):
        name, music = _cache[i]
        if name == filename:
            # Move it to the head of the list.
            _cache.pop(i)
            _cache.insert(0, (name, music))
            return sound
        i += 1
    # If execution has reached this point, then we have to load it now.
    while len(_cache) > _cache_size:
        # Discard the oldest.
        _cache.pop()
    music = ika.Music(filename)
    music.loop = True
    _cache.insert(0, (filename, music))
    return music


def playMusic(fileName):
    stopMusic()
    music = cacheMusic(filename)
    assert music is not None
    music.Play()
