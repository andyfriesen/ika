import ika

class Cursor(object):
    def __init__(_, font):
        _.font = font

    def Draw(_,x ,y ,dest = None):
        _.font.Print(x, y, '>')

    Width   = property(lambda _: _.font.StringWidth('>'))
    Height  = property(lambda _: _.font.height)
    Size    = property(lambda _: (_.Width, _.Height))
    HotSpot = property(lambda _: (_.font.width, _.font.height / 2)) # read only