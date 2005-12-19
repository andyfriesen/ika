# Entity movement classes
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

class Wander(object):
    MOVE = 0
    WAIT = 1

    def __init__(self, delay, steps):
        self.delay = delay
        self.steps = steps
        self.state = self.WAIT

    def __call__(self, ent):
        if self.state == self.WAIT:
            self.state = self.MOVE
            ent.Wait(self.delay)

        elif self.state == self.MOVE:
            self.state = self.WAIT
            dir = ika.Random(0, 4) # pick a direction (just cardinal for now)
            dist = ika.Random(self.steps / 2, self.steps) # pick a distance

            if dir == 0:    # left
                ent.MoveTo(ent.x - dist, ent.y)
            elif dir == 1:  # right
                ent.MoveTo(ent.x + dist, ent.y)
            elif dir == 2:  # up
                ent.MoveTo(ent.x, ent.y - dist)
            elif dir == 3:  # down
                ent.MoveTo(ent.x, ent.y + dist)

class Script(object):
    def __init__(self):
        self.script = []
        self.offset = 0
        self.killed = False

    def Kill(self, ent):
        self.killed = True
        ent.Stop()
        ent.movescript = None

    def __call__(self, ent):
        assert not self.killed

        if len(self.script) == 0:
            self.Kill(ent)             # end of script (there is no script :P)
            return

        s = self.script[self.offset]
        self.offset += 1
        s(ent)

        if self.offset >= len(self.script):       # end of script.  Terminate.
            #ent.movescript = None
            self.Kill(ent)
            return

    def Clone(self):
        '''
        Each individual Script instance should be used for one, and only one entity, or the state
        will get all messed up.

        This method returns clone of the script; use this if you want many entities to use the same move script.
        '''
        clone = Script()
        clone.script = self.script[:]          # the individual commands are immutable, so a shallow copy is sufficient
        return clone

    '''
    Now, for defining the move script.
    Calling any of the following methods appends a command to the script.
    They all return a self reference, so you can just chain them along.

    eg.
    MyMoveScript = (movescript.Script()        # the beginning parenth causes python to ignore line breaks
        .MoveRight(16)  # move right 16 pixels
        .Wait(100)      # stop for 1 second
        .MoveLeft(16)   # left 16 pixels
        .Wait(100)      # wait for another second
        .Loop()         # repeat ad infinitum
        )

    Additionally, if you want to be a goof, you can add things to the script member yourself.  Just make
    sure that it will execute properly when given the entity as its only argument, and that it does what
    it needs to do.

    In theory, a grouping command (that would execute a subcommand), and a few subclasses to conditionally,
    and repeatedly execute some smaller list of scripts would be all that would be necessary to make this
    script setup thing turing complete. (woo?)
    '''

    def MoveUp(self, steps = 1):
        self.script.append(lambda ent: ent.MoveTo(ent.x, ent.y - steps))
        return self

    def MoveDown(self, steps = 1):
        self.script.append(lambda ent: ent.MoveTo(ent.x, ent.y + steps))
        return self

    def MoveLeft(self, steps = 1):
        self.script.append(lambda ent: ent.MoveTo(ent.x - steps, ent.y))
        return self

    def MoveRight(self, steps = 1):
        self.script.append(lambda ent: ent.MoveTo(ent.x + steps, ent.y))
        return self

    def MoveTo(self, x, y):
        self.script.append(lambda ent: ent.MoveTo(x, y))
        return self

    def Wait(self, count):
        self.script.append(lambda ent: ent.Wait(count))
        return self

    def SetSpecFrame(self, frame):
        # can't do an assignment in a lambda
        def setSpecFrame(ent):
            ent.specframe = frame

        self.script.append(setSpecFrame)
        return self

    def SetSpeed(self, speed):
        def doSetSpeed(ent):
            ent.speed = speed

        self.script.append(doSetSpeed)
        return self

    def Stop(self):
        'Completely halts the movement script.'

        def doStop(ent):
            #ent.movescript = None
            self.Kill(ent)

        self.script.append(doStop)
        return self

    def Loop(self):
        def doLoop(ent):    # ent is unused here.  closures rock.
            self.offset = 0    # we retain the outer self reference, and can use it like this. >:D

        self.script.append(doLoop)
        return self

def Follow(ent, chaseTarget):
    r16 = range(16)

    while True:
        ent.MoveTo(chaseTarget.x, chaseTarget.y)
        for count in r16:
            yield None
