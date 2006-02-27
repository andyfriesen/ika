# Input control abstraction
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

up = lambda: ika.Input.up.Position() > 0
down = lambda: ika.Input.down.Position() > 0
left = lambda: ika.Input.left.Position() > 0
right = lambda: ika.Input.right.Position() > 0
enter = lambda: ika.Input.enter.Pressed()
cancel = lambda: ika.Input.cancel.Pressed()