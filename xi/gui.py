# Basic GUI elements
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

"""
I have tried time and again to make such a system as elegant as possible, and
have repeatedly failed.  This time, I am bound and determined to make a simple,
consistent framework.

Style:
Everything public uses camel notation.  Classes have their first letter uppercase.
Private things are preceded with a single underscore.  Under no circumstance
are private attributes to be used instead of their accessor properties.  This is
essential to preserve polymorphic behaviour.
"""

import ika

import window
import cursor

import decorator
Decorator = decorator.Decorator

default_font = None
default_cursor = None
default_window = window.GradientWindow(
        ika.RGB(0, 0, 192),
        ika.RGB(0, 0, 128),
        ika.RGB(0, 0,  32),
        ika.RGB(0, 0,  64)
)

def init(font, window = None, cursor = None):
    '''init(font, window = None, cursor = None) -- Initialize the xi GUI system.

    Initializes defaults for the GUI system.  This MUST be called before
    creating any instances.

    If window is omitted, a blue gradient window with a black border is used.
    The > character of the default font is used if the cursor is omitted.
    '''
    # symbol conflict, heh.  But I want that keyword arg to be called cursor,
    # so we do a tiny hack:
    import cursor as xiCursor

    global default_font
    global default_window
    global default_cursor

    default_font = font
    default_window = window or default_window
    default_cursor = cursor or xiCursor.TextCursor(default_font, '>')

class Widget(object):
    '''
    Basic GUI element class.  Widgets themselves have absolutely no
    identity save for that of a rect with size and position.  This is
    the ultimate base GUI class.
    '''

    def __init__(self, x = 0, y = 0, width = 0, height = 0, **kw):
        assert default_font is not None and default_cursor is not None, 'Must call gui.init before creating widgets!'
        self._x = x
        self._y = y
        self._width = width
        self._height = height
        self.border = 0 # replace this with a property at some point.

    # Property accessors always have well defined names.  Prepend 'get'
    # and 'set'.  Override these to override the property.  Always.

    def getX(self):
        return self._x
    def setX(self, value):
        self._x = value
    x = property(getX, setX)

    def getY(self):
        return self._y
    def setY(self, value):
        self._y = value
    y = property(getY, setY)

    def getWidth(self):
        return self._width
    def setWidth(self, value):
        self._width = value
    width = property(getWidth, setWidth)

    def getHeight(self):
        return self._height
    def setHeight(self, value):
        self._height = value
    height = property(getHeight, setHeight)

    # breaking the above rule.  doh.
    # These depend on x, y, width, and height anyway.
    # you shouldn't need to override them.
    left = property(lambda self: self.x)
    top = property(lambda self: self.y)
    right = property(lambda self: self.x + self.width)
    bottom = property(lambda self: self.y + self.height)

    # it is important that these accessors go through the above properties for
    # polymorphic purposes.
    def getPosition(self):
        return self.x, self.y
    def setPosition(self, value):
        self.x, self.y = value
    position = property(getPosition, setPosition)

    def getSize(self):
        return self.width, self.height
    def setSize(self, value):
        self.width, self.height = value
    size = property(getSize, setSize)

    def getRect(self):
        return self.x, self.y, self.width, self.height
    def setRect(self, value):
        self.x, self.y, self.width, self.height = value
    rect = property(getRect, setRect)

    def dockTop(self, o = None):
        if o is not None:
            self.y = o.bottom + self.border
        else:
            self.y = self.border
        return self

    def dockLeft(self, o = None):
        if o is not None:
            self.x = o.right + self.border
        else:
            self.x = self.border
        return self

    def dockRight(self, o = None):
        if o is not None:
            self.x = o.left - self.width - self.border
        else:
            self.x = ika.Video.xres - self.width - self.border
        return self

    def dockBottom(self, o = None):
        if o is not None:
            self.y = o.top - self.height - self.border
        else:
            self.y = ika.Video.yres - self.height - self.border
        return self

    def draw(self, xofs = 0, yofs = 0):
        '''
        Draws the widget onscreen.  xofs and yofs are added to the widget's
        own positional coordinates.  xofs and yofs are customarily the absolute
        x/y position of the containing widget.  This makes for a nice recursive
        drawing process.
        '''
        pass

class Container(Widget):
    '''
    A widget that contains children.  The base container does nothing but draw
    its children.
    '''
    def __init__(self, *args, **kw):
        super(Container, self).__init__(*args)
        self._children = kw.get('children', [])

    def getChildren(self):
        return self._children  # I should make a const proxy or something for lists that mustn't be altered.
    children = property(getChildren)

    def addChild(self, child):
        'Adds a child to the widget.'

        assert child not in self.children, 'Object %o is already a child!'
        self.children.append(child)

    def addChildren(self, *children):
        for child in children:
            self.addChild(child)

    def removeChild(self, child):
        'Removes a child from the widget.'
        assert child in self.children, 'Attempt to remove nonexistent child!'
        self.children.remove(child)

    def removeChildren(self, *children):
        for child in children:
            self.removeChild(child)

    def removeAllChildren(self):
        # not assigning the empty list because 1) we aren't allowed,
        # and 2) it would break polymorphism, if some subclass were
        # to override the children property.
        self.removeChildren(*self.children)

    def hasChild(self, child):
        'Returns True if the child widget belongs to us.'
        return child in self.children

    def autoSize(self):
        '''
        Sets the size of the container such that it encloses all of its children.
        '''
        self.size = (1, 1)
        for child in self.children:
            self.size = (
                max(self.width, child.width + child.x),
                max(self.height, child.height + child.y)
                )

    def draw(self, xofs = 0, yofs = 0):
        for child in self.children:
            child.draw(self.x + xofs, self.y + yofs)

class Frame(Widget):
    '''
    A widget that appears as a graphical frame of some sort.
    Frames have one child: a single Container that encloses its
    entire client region.
    '''
    def __init__(self, x = 0, y = 0, width = 0, height = 0, **kwargs):
        Widget.__init__(self, x, y, width, height)
        self._client = Container()

        self._wnd = kwargs.get('wnd', default_window)
        self.border = self.wnd.border * 2
        self._client.border = self.wnd.border * 2

    def getClient(self):
        return self._client
    client = property(getClient)

    def getWnd(self):
        return self._wnd
    wnd = property(getWnd)

    # width and height are overridden so that the client is always the right size.
    def setWidth(self, value):
        self.client.width = value - self.client.border * 2
        super(Frame, self).setWidth(value)
    width = property(Widget.getWidth, setWidth)

    def setHeight(self, value):
        self.client.height = value - self.client.border * 2
        super(Frame, self).setHeight(value)
    height = property(Widget.getHeight, setHeight)

    # just a convenience method.
    def addChild(self, *args, **kw):
        self.client.addChild(*args, **kw)

    def autoSize(self):
        self.client.autoSize()
        self.size = (
            self.client.width + self.client.border * 2,
            self.client.height + self.client.border * 2
            )

    def draw(self, xofs = 0, yofs = 0):
        self.wnd.draw(self.x + xofs, self.y + yofs, self.width, self.height)
        self.client.draw(self.x + xofs + self.client.border, self.y + yofs + self.client.border)

class FrameDecorator(Widget, Decorator):
    """
    Decorates a widget with a frame.  This is generally better than using a
    Frame if you only want to enclose a single Widget object with the frame.
    """
    def __init__(self, subject, wnd = None):
        Decorator.__init__(self, subject)
        Widget.__init__(self)
        self._wnd = wnd or default_window
        self.border = self._wnd.border * 2

    # I'm not certain that this is a good idea.
    # Regardless, it's kind of nice to have FrameDecorator
    # behave like a Frame, as well as whatever it is that it is
    # decorating.
    def getClient(self):
        return self.getSubject() # is this right?
    client = property(getClient)

    def getWnd(self):
        return self._wnd
    wnd = property(getWnd)

    def getX(self):
        return self.getSubject().x - self.getSubject().border
    def setX(self, value):
        self.getSubject().x = value + self.getSubject().border
    x = property(getX, setX)

    def getY(self):
        return self.getSubject().y - self.getSubject().border
    def setY(self, value):
        self.getSubject().y = value + self.getSubject().border
    y = property(getY, setY)

    def getWidth(self):
        return self.getSubject().width + self.getSubject().border * 2
    def setWidth(self, value):
        self.getSubject().width = value - self.getSubject().border * 2
    width = property(getWidth, setWidth)

    def getHeight(self):
        return self.getSubject().height + self.getSubject().border * 2
    def setHeight(self, value):
        self.getSubject().height = value - self.getSubject().border * 2
    height = property(getHeight, setHeight)

    def draw(self, xofs = 0, yofs = 0):
        s = self.getSubject()
        self._wnd.draw(xofs + self.x, yofs + self.y, self.width, self.height)
        s.draw(xofs, yofs)

class StaticText(Widget):
    '''
    A widget that appears as some lines of text.
    No frame is drawn.
    '''

    def __init__(self, x = 0, y = 0, w = 0, h = 0, *args, **kwargs):
        super(StaticText, self).__init__(x, y, w, h, **kwargs)

        t = kwargs.get('text')
        if isinstance(t, str):
            self._text = [t]
        elif isinstance(t, list):
            self._text = t
        elif t is None:
            self._text = []
        else:
            assert False, 'StaticText.text property must be a list, a string, or None. (or unspecified)'

        self._font = kwargs.get('font', default_font)

        self.autoSize()

    def getText(self):
        return self._text
    text = property(getText)

    def getFont(self):
        return self._font
    font = property(getFont)

    def addText(self, *args):
        'Appends text to what is already stored'
        self.text.extend([str(i) for i in args])

    def clear(self):
        'Clears all the text.'
        self._text = []

    def autoSize(self):
        'Sets the size of the StaticText control such that there is enough room for all the text contained.'
        if self.text:
            self.height = len(self.text) * self.font.height
            self.width = max(1,
                *[ self.font.StringWidth(t) for t in self.text ]
                )
        else:
            self.size = 1, 1

    def draw(self, xoffset = 0, yoffset = 0):
        y = self.y + yoffset
        x = self.x + xoffset
        for t in self.text:
            self.font.Print(x, y, t)
            y += self.font.height

class ScrollableTextLabel(StaticText):
    '''
    A text label that can potentially hold more text than it can visually display, given
    whatever size it may be at the time.

    The text label's scroll position (YWin) is in pixel coordinates, and can range from 0 to
    its YMax value.
    '''

    def __init__(self, *args, **kw):
        super(ScrollableTextLabel, self).__init__(*args, **kw)
        self._ywin = 0

    def getYwin(self):
        return self._ywin
    def setYwin(self, value):
        value = max(0, value)
        self._ywin = min(self.ymax, value)
    ywin = property(getYwin, setYwin)

    def getYmax(self):
        bleh = (len(self.text) * self.font.height) - self.height
        return bleh
    ymax = property(getYmax)

    def draw(self, xoffset = 0, yoffset = 0):
        x = self.x + xoffset
        y = self.y + yoffset
        ika.Video.ClipScreen(x, y, x + self.width, y + self.height)

        firstLine = self.ywin / self.font.height
        lastLine = (self.height + self.ywin) / self.font.height + 1

        curY = y - self.ywin % self.font.height
        for line in self.text[firstLine:lastLine]:
            self.font.Print(x, curY, line)
            curY += self.font.height

        ika.Video.ClipScreen()

    def autoSize(self):
        super(type(self), self).autoSize()
        self.height = min(self.height, ika.Video.yres - self.y - self.border)

class ColumnedTextLabel(Widget):
    def __init__(self, *args, **kw):
        super(ColumnedTextLabel, self).__init__(*args, **kw)
        import layout

        assert 'columns' in kw, 'Must specify columns argument in ColumnedTextLabel constructor!'
        columns = kw['columns']
        assert columns > 0, 'Need at least one column!  %i is not enough!' % columns
        self._pad = kw.get('pad', 0)
        self._columns = [ScrollableTextLabel() for q in range(columns)]
        self._layout = layout.HorizontalBoxLayout(pad=self._pad, children=self._columns)
        self.columnWeights = None

    def getText(self):
        return self._columns[0].text # not much else I can do without making this prohibitively expensive
    text = property(getText)

    def getFont(self):
        return self._columns[0].font
    font = property(getFont)

    def getX(self):
        return self._layout.x
    def setX(self, value):
        self._layout.x = value
    x = property(getX, setX)

    def getY(self):
        return self._layout.y
    def setY(self, value):
        self._layout.y = value
    y = property(getY, setY)

    def getWidth(self):
        return self._layout.width
    def setWidth(self, value):
        self._layout.width = value

        if self.columnWeights:
            self._layout.layoutWeighted(self.columnWeights)
        else:
            self._layout.layout()

    width = property(getWidth, setWidth)

    def getHeight(self):
        return self._layout.height
    def setHeight(self, value):
        self._layout.height = value
        for c in self._columns:
            c.height = value
    height = property(getHeight, setHeight)

    def getYwin(self):
        return self._columns[0].ywin
    def setYwin(self, value):
        for col in self._columns:
            col.ywin = value
    ywin = property(getYwin, setYwin)

    def getYmax(self):
        return self._columns[0].ymax
    ymax = property(getYmax)

    def addText(self, *args):
        'Unlike StaticText.addText, this only adds to one column at a time!'

        assert len(args) == len(self._columns), 'Need to add something to each column, bucko.'
        for col, s in zip(self._columns, args):
            col.addText(s)

    def clear(self):
        for col in self._columns:
            col.clear()

    def autoSize(self):
        for col in self._columns:
            col.autoSize()

        if self.columnWeights:
            self._layout.layoutWeighted(self.columnWeights)
        else:
            self._layout.layout()
            self.width = self._columns[-1].right

    def draw(self, xoffset = 0, yoffset = 0):
        self._layout.draw(xoffset, yoffset)

class Picture(Widget):
    '''
    A widget that takes the shape of an image.
    Little else to say.
    '''
    def __init__(self, x = 0, y = 0, width = 0, height = 0, **kw):
        super(Picture, self).__init__(x, y, width, height)

        assert 'image' in kw, 'Must specify an image argument to Picture constructor.'

        i = kw.get('image')

        if isinstance(i, ika.Image):
            self._image = i
        elif isinstance(i, (ika.Canvas, str)):
            self._image = ika.Image(i)
        else:
            assert False, 'Picture.image must be a string, canvas, or image'

        self.size = (width or self.image.width), (height or self.image.height)

        self.drawImage = Picture.drawImage

    # make this writable?
    def getImage(self):
        return self._image
    image = property(getImage)

    def drawImage(image, x, y, width, height):
        '''
        Does the actual drawing.
        The cool part of this is that you can assign a new drawImage method
        to a Picture object to override how it does the drawing.
        (ie tinting, scaling, tiling, etc)
        '''
        ika.Video.ScaleBlit(image, x, y, width, height)

    drawImage = staticmethod(drawImage)

    def draw(self, xoffset = 0, yoffset = 0):
        self.drawImage(self.image, self.x + xoffset, self.y + yoffset, self.width, self.height)
