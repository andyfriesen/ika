# Base UI layer
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

# Frames hold an arbitrary number of widgets. (and are widgets themselves)
# Widgets are, currently, either text labels, or images.  Maybe more later.

import ika
import window

# globals (hurk)

defaultwindow = window.SimpleWindow()

if ika.Video.xres < 640:
    defaultfont = ika.Font('system.fnt')
else:
    defaultfont = ika.Font('arial.fnt')

defaultfont.tabsize = 64


def SetDefaultWindow(wnd):
    global defaultwindow
    defaultwindow = wnd

class Widget(object):
    "Basic widget interface."
    
    __slots__ = [
        'x',        #
        'y',        # Location of the widget, relative to its parent
        'width',    #
        'height',   # Size of the widget
        'border'    # How much breathing room to give the widget.  Only used for the Dock functions.
        ]
    
    def __init__(_, x = 0, y = 0, width = 0, height = 0):
        _.x = x
        _.y = y
        _.width = width
        _.height = height
        _.border = 0

    def Draw(_, xoffset = 0, yoffset = 0):
        pass

    def DockLeft(_, wnd = None):
        _.x = _.border * 2
        if wnd is not None:
            _.x += wnd.Right
        return _
    
    def DockRight(_, wnd = None):
        x = wnd and wnd.Right or ika.Video.xres
        _.x = x - _.width - _.border * 2
        return _
    
    def DockTop(_, wnd = None):
        _.y = _.border * 2
        if wnd is not None:
            _.y += wnd.Bottom
        return _
    
    def DockBottom(_, wnd = None):
        y = wnd and wnd.Bottom or ika.Video.yres
        _.y = y - _.height - _.border * 2
        return _

    def get_Position(_):        return _.x, _.y
    def set_Position(_, pos):   (_.x, _.y) = pos

    def get_Size(_):            return _.width, _.height
    def set_Size(_, value):     (_.width, _.height) = value
    
    def get_Rect(_):            return (_.x, _.y, _.width, _.height)
    def set_Rect(_, r):         _.x, _.y, _.width, _.height = r

    Position = property(get_Position, set_Position, doc = 'Gets or sets the position')
    Size     = property(get_Size, set_Size, doc = 'Gets or sets the size (in pixels)')
    Rect     = property(get_Rect, set_Rect)

    def set_Left(_, value):      _.x = value
    def set_Right(_, value):     _.width = value - _.x
    def set_Top(_, value):       _.y = value
    def set_Bottom(_, value):    _.height = value - _.y

    Left     = property(lambda _: _.x, set_Left)
    Top      = property(lambda _: _.y, set_Top)
    Right    = property(lambda _: _.x + _.width, set_Right)
    Bottom   = property(lambda _: _.y + _.height, set_Bottom)

class Frame(Widget):
    "Base frame class.  A window, with things in it."

    __slots__ = Widget.__slots__ + [
        'wnd',      # The windowstyle to draw for this frame
        'widgets'   # List of child widgets
        ]
    
    def __init__(_, wnd = None, x = 0, y = 0, width = 0, height = 0):
        Widget.__init__(_, x, y, width, height)
        
        _.wnd = wnd or defaultwindow
        _.widgets = []
        _.border = int(1.5 * _.wnd.Left)     # or right or whatever

    def Draw(_, xoffset = 0, yoffset = 0):
        _.wnd.Draw(_.x, _.y, _.x+_.width, _.y+_.height)
        for x in _.widgets:
            x.Draw(_.x, _.y)

    def AutoSize(_):                 # makes the frame big enough to hold its contents
        _.width = 0
        _.height = 0

        for w in _.widgets:
            _.width = max(_.width, w.x+w.width)
            _.height = max(_.height, w.y+w.height)

    def AddChild(_, child):
        _.widgets.append(child)

    def RemoveChild(_, child):
        _.widgets.remove(child)

class TextFrame(Frame):
    "A frame with a simple text widget.  Nothing else."

    __slots__ = Frame.__slots__ + [
        'label'     # The TextLabel attached to this frame
        ]
    
    def __init__(_, wnd = None, x = 0, y = 0, width = 0, height = 0):
        Frame.__init__(_, wnd, x, y, width, height)
        _.label = TextLabel()
        _.widgets.append(_.label)

    def get_Text(_):
        return _.label.Text

    def set_Text(_, *value):
        _.label.Clear()
        _.label.AddText(list(value))

    Text = property(get_Text, set_Text)        

    def Clear(_):
        _.label.Clear()
        _.label.Size = (0, 0)

    def AddText(_, args):
        _.label.AddText(args)
        _.Size = _.label.Size
        
    def AutoSize(_):
        _.label.AutoSize()
        _.Size = _.label.Size

    Text = property(lambda _: _.label.Text)
    

class TextLabel(Widget):
    'Textlabels hold one or more lines of text.'
    __slots__ = Widget.__slots__ + [
        'font',         # The font used to draw the text
        'ypage',        # Position of the viewport within the text list, if there is more text here than can be seen at once.
        'ymax',         # If nonzero, the bottom-line maximum height this label can reach.
        'text',         # The text itself.  Stored as a list of strings; one line per element.
        'PrintString'   # The method used to draw the text.  Typically for internal use only.
        ]

    def __init__(_, font = defaultfont, *text):
        Widget.__init__(_)

        _.font = font
        _.ypage = 0
        _.ymax = 0

        _.text = list(text)
        _.PrintString = None

        _.LeftJustify()

    Length = property(lambda _: len(_.text))

    def LeftJustify(_):
        _.PrintString = _.font.Print

    def RightJustify(_):
        _.PrintString = _.font.RightPrint

    def Center(_):
        _.PrintString = _.font.CenterPrint

    def Clear(_):
        _.text = []
        _.width = 0
        _.height = 0

    def SetText(_, *text):
        _.Clear()
        _.AddText(text)

    def AddText(_, *text):
        for x in text:
            _.text.append(x)
            _.width = max(_.width, _.font.StringWidth(x))
        _.AutoSize()

    def Draw(_, xoffset = 0, yoffset = 0):
        curx = 0
        cury = 0

        i = _.ypage
        while cury < _.height and i < len(_.text):
            _.PrintString(_.x + xoffset + curx, _.y + yoffset + cury, _.text[i])
            i += 1
            cury += _.font.height

    def AutoSize(_):
        _.Size = (0, 0)
        
        for t in _.text:
            _.width = max(_.width, _.font.StringWidth(t))

        _.height = len(_.text) * _.font.height
        
        if _.ymax != 0:
            maxy = (_.ymax - 1) * _.font.height
        else:
            maxy = ika.Video.yres - _.y

        _.height = min(_.height, maxy)

    def set_YPage(_, value):
        _.ypage = min(value, _.height - _.height / _.font.height)

    def set_YMax(_, value):
        _.ymax = value
        _.AutoSize()

    YPage = property(lambda _: _.ypage, set_YPage)
    YMax = property(lambda _: _.ymax, set_YMax)
    Text = property(lambda _: _.text)

class ColumnedTextLabel(Widget):
    'A text label that holds one or more columns of text.  Columns stay lined up.'
    __slots__ = Widget.__slots__ + [
        'font',
        'columns',
        'columngap'
        ]

    #-------------------
    class Row(object):
        __slots__ = [
            'text',    # the text control that owns this row
            'row'      # ordinal index of the row within the text control
            ]
        
        def __init__(_, text, row):
            _.text = text
            _.row = row

        def __getitem__(_, idx):
            return _.text.column[idx].Text[row]

        def __setitem__(_, idx, value):
            _.text.column[idx].Text[row] = value
    #-------------------

    def __init__(_, x = 0, y = 0, columns = 1, font = defaultfont, columngap = 10):
        Widget.__init__(_, x, y)

        _.font = font
        _.columngap = columngap

        _.columns = []
        for i in range(columns):
            _.columns.append(TextLabel(font))

    Length = property(lambda _: _.columns[0].Length)

    # Fakes a 2D array.  mytextcontrol[row][column]
    def __getitem__(_, index):
        return Row(_, index)

    def LeftJustify(_):
        for c in _.columns:
            c.LeftJustify()

    def RightJustify(_):
        for c in _.columns:
            c.RightJustify()

    def Center(_):
        for c in _.columns:
            c.Center()

    def Clear(_):
        for c in _.columns:
            c.Clear()

    def AddText(_,*args):
        i = 0
        for c in _.columns:
            c.AddText(i < len(args) and args[i] or '')
            i += 1
        _.AutoSize()

    def Draw(_, xoffset = 0, yoffset = 0):
        for c in _.columns:
            c.Draw(_.x + xoffset, _.y + yoffset)

    def AutoSize(_):
        _.columns[0].AutoSize()
        _.columns[0].Position = (0, 0)
        for i in range(1, len(_.columns)):
            _.columns[i].AutoSize()
            _.columns[i].DockLeft(_.columns[i - 1])
            _.columns[i].x += _.columngap

        _.Size = (0, 0)
        for c in _.columns:
            _.width = max(_.width, c.Right)
            _.height = max(_.height, c.Bottom)

    def set_YPage(_, value):
        for c in _.columns:
            c.YPage = value

    def set_YMax(_, value):
        for c in _.columns:
            c.ymax = value  # dirty; should be using the property.  But I'm going to resize it in a minute anyway.

        _.AutoSize()            

    YPage = property(lambda _: _.columns[0].YPage, set_YPage)
    YMax = property(lambda _: _.columns[0].ymax, set_YMax)

'''class ColumnedTextLabel(Widget):
    'A text label that holds one or more columns of text.  Columns stay lined up.'

    class __Row(object):
        'Represents a single row of the label'

        def __init__(_, parent, rowindex):
            _.parent = parent
            _.rowindex = rowindex

        def __len__(_):
            return len(_.parent.columns)

        def __getitem__(_, index):
            return _.parent.columns[index][rowindex]

        def __setitem__(_, index, value):
            _.parent.columns[index][rowindex] = value

    class __Column(object):
        'Represents a single column of the label'

        def __init__(_, parent, colindex):
            _.parent = parent
            _.colindex = colindex

        def __len__(_):
            return _.parent.numcolumns

        def __getitem__(_, index):
            return _.parent.columns[colindex][index]

        def __setitem__(_, index, value):
            _.parent.columns[colindex][index] = value

    def __init__(_, x = 0, y = 0, columns = 1, font = None, columngap = 10):
        Widget.__init__(_)
        _.font = font or defaultfont
        _.columngap = columngap
        _.numcolumns = columns
        _.columns = [[''] * columns]

    def Row(_, index):  return __Row(_, index)
    def Col(_, index):  return __Col(_, index)
'''
        

class Bitmap(Widget):
    "Bitmap widgets are images."
    __slots__ = Widget.__slots__ + [
        'img'   # The image drawn within this bitmap
        ]
    
    def __init__(_, img = None):
        Widget.__init__(_)

        _.img = img
        _.x = 0
        _.y = 0
        if img is not None:
            _.Size = img.width, img.height

    def set_Image(_, val):
        _.img = val
        if val is not None:
            _.Size = val.width, val.height

    Image = property(lambda _: _.img, set_Image)

    def Draw(_, xoffset = 0, yoffset = 0):
        # TODO: allow the widget to be resized, thus scaling the image?
        if _.img is not None:
            _.img.Blit(_.x+xoffset, _.y+yoffset)
