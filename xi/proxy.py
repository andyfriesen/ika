#!/usr/bin/env python

# author: Ian Douglas Bollinger
# This file has been placed in the public domain.

# main ########################################################################

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

# end #########################################################################
