# Sample fieldeffects.py for Demo
# Coded by Andy Friesen
# 17 Feb 2003
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from xi.menu import Cancel
from menu import util
from stats import activeRoster
import ika

blah = lambda: None

# You don't usually need to worry about what ctx is, except that
# some functions want it.
def GenericHeal(ctx, amount):
    result = util.selectCharacter(ctx)
    if result is None or result is Cancel:
        return False # returning anything except None indicates that the action was cancelled.
    else:
        activeRoster[result].heal(amount)

def Aspirin(ctx):
    return GenericHeal(ctx, 50)

def Buffrin(ctx):
    return GenericHeal(ctx, 300)

def T3(ctx):
    return GenericHeal(ctx, 9999)

def Manual(ctx):
    util.message(
    '''This is a really long paragraph consisting of random blathering that just drones on and on and doesn't really go anywhere.
It's quite depressing, really, when you think about it.  I went to all the trouble to write out all this crap, and really, it's just filler that fails to actually serve any real purpose except to devour space, and to demonstrate this admittedly cool dialog box thingamajig.

ALL GLORY TO THE HYPNOTOAD.''', ctx)

# magic:

def Heal(caster, ctx):
    return GenericHeal(ctx, (10 + caster.stats.mag) * ika.Random(90, 110) / 100) # (magic+10) +/-10%
