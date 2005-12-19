# Coded by Ear
# Public domain, I think.

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
            object.__getattribute__(self, name)  #obnoxious hack
        except AttributeError:
            setattr(self.__dict__['__subject__'], name, value)
        else:
            object.__setattr__(self, name, value)