import ika

class Cursor(object):
    def __init__(self, font):
        self.font = font
        self.hotx = self.Width
        self.hoty = self.Height / 2

    def Draw(self,x ,y ,dest = None):
        self.font.Print(x - self.hotx, y - self.hoty, '>')

    Width   = property(lambda self: self.font.StringWidth('>'))
    Height  = property(lambda self: self.font.height)
    Size    = property(lambda self: (self.Width, self.Height))
    HotSpot = property(lambda self: (self.hotx, self.hoty)) # read only