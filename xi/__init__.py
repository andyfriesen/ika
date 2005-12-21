#!/usr/bin/env python

__all__ = ['character', 'color', 'controls', 'cursor', 'decorator', 'effects',
           'equipmenu', 'field', 'fps', 'gui', 'item', 'itemdatabase',
           'itemmenu', 'layout', 'mainmenu', 'menu', 'menuwindows',
           'movescript', 'music', 'parser', 'party', 'skill', 'skilldatabase',
           'skillmenu', 'sound', 'statset', 'statusmenu', 'test', 'textbox',
           'token', 'transition', 'utilmenu', 'widget', 'widgetmanager',
           'window']


class XiException(Exception):
    """Standard exception class."""


class XiError(XiException, RuntimeError):
    pass


class XiWarning(XiException, Warning):
    pass


class Proxy(object):

    def __init__(self, subject):
        self.__dict__['__subject__'] = subject

    def getSubject(self):
        return self.__dict__['__subject__']

    def __getattr__(self, name):
        try:
            return object.__getattribute__(self, name)
        except AttributeError:
            return getattr(self.__dict__['__subject__'], name)


class Singleton(type):

    def __new__(cls, name, bases, d):
        def __init__(self, *args, **kw):
            raise TypeError, ("Cannot create '%s' instances." %
                              self.__class__.__name__)
        instance = type(name, bases, d)()
        instance.__class__.__init__ = __init__
        return instance


class StatelessProxy(object):
    """This effectively accomplishes the same thing as a singleton,
    except that it disregards object 'identity' all together.
    """

    def __init__(self):
        if '__shared__' not in self.__class__.__dict__:
            self.__class__.__shared__ = {}
        self.__dict__ = self.__class__.__shared__


class Decorator(object):

    def __init__(self, subject):
        super(Decorator, self).__init__()
        self.__dict__['__subject__'] = subject

    def getSubject(self):
        return self.__dict__['__subject__']

    def __getattr__(self, name):
        try:
            return object.__getattribute__(self, name)
        except AttributeError:
            return getattr(self.__dict__['__subject__'], name)

    def __setattr__(self, name, value):
        try:
            # Obnoxious hack.
            object.__getattribute__(self, name)
        except AttributeError:
            setattr(self.__dict__['__subject__'], name, value)
        else:
            object.__setattr__(self, name, value)


def readonly(fget, doc=""):
    return property(fget, doc=doc)


def wrapText(text, maxWidth, font):
    """Wraps the text to the given pixel width using the font specified.

    Returns a list of strings.
    """
    result = []
    pos = 0
    lastSpace = 0
    while len(text) > 0:
        # find a space, tab, or newline.  whichever comes first.
        # if the word can be appended to the current line, append it.
        # if not, and the current line is not empty, put it on a new line.
        # if the word is longer than a single line, hack it wherever, and make the hunk its own line.
        # find the next space, tab, or newline.
        if pos >= len(text):
            # hit the end of the string?
            result.append(text)
            # we're done.  add the last of it to the list
            break
        if text[pos].isspace():
            lastSpace = pos
        if text[pos] == '\n':
            # newline.  Chop.
            result.append(text[:pos])
            text = text[pos + 1:]
            pos = 0
            lastSpace = 0
            continue
        # Python slices are [low, high), so we must compensate by adding 1.
        l = font.StringWidth(text[:pos + 1])
        if l >= maxWidth:
            # too wide.  Go back to the last whitespace character, and chop
            if lastSpace > 0:
                result.append(text[:lastSpace])
                text = text[lastSpace + 1:]
                pos = 0
                lastSpace = 0
            else:
                # no last space!  Hack right here, since the word is obviously too goddamn long.
                result.append(text[:pos + 1])
                text = text[pos + 1:].lstrip()
                pos = 0
                lastSpace = 0
            continue
        pos += 1

    return result


def clamp(value, lower, upper):
    """Returns the value, unless it is outside the min and max allowed
    values, in which case it returns whichever boundary the value
    crossed.
    
    (min if value is to low, max if value is too high)
    """
    return max(min(value, upper), lower)


def formatTime(time, separator = ':'):
    hours = time / 360000
    minutes = time / 6000 % 60
    seconds = time / 100 % 60
    return separator.join(['%02i' % i for i in (hours, minutes, seconds)])
