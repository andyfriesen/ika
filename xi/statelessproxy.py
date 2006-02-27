# author: Ian Douglas Bollinger
# This file has been placed in the public domain.

# documentation ###############################################################

"""
This module implements the stateless proxy design pattern.
"""

# main ########################################################################

class StatelessProxy(object):
    """
    This effectively accomplishes the same thing as a singleton, except that
    it disregards object 'identity' all together.
    """

    def __init__(self):
        if '__shared__' not in self.__class__.__dict__:
            self.__class__.__shared__ = {}
        self.__dict__ = self.__class__.__shared__

# test ########################################################################

def __main__(*argv):
    class A(StatelessProxy):
        pass

    A().x = True

    assert A().x is A().x


if __name__ == "__main__":
    __main__()

# end #########################################################################
