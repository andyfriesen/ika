import ika

class Cursor(object):
    def __init__(self, font):
        self.font = font

    def Draw(self,x ,y ,dest = None):
        self.font.Print(x, y, '>')

    HotSpot = property(lambda self: (self.font.width, self.font.height / 2)) # read only