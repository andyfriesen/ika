# Basic layout management classes
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.
#
# All glory to the list comprehension.

import gui

class Spacer(gui.Widget):
    'Non-widget.  Use this to make gaps between children of a layout manager.'
    pass

class Layout(gui.Container):
    def layout(self):
        assert False, "Layout should be abstract.  Don't use it directly. :P"

class VerticalBoxLayout(Layout):
    'Arranges its children in a vertical column.'
    def __init__(self, *args, **kw):
        super(VerticalBoxLayout, self).__init__(*args, **kw)
        self._pad = kw.get('pad', 0)
        self._stretch = kw.get('stretch', False)

    def layoutWeighted(self, weights):
        # figure out how much empty space we have:
        freeSpace = self.height - sum([c.height + self._pad for c in self._children])

        # Any space to distribute?
        if freeSpace > 0:
            # do it
            for child, weight in zip(self._children, weights):
                child.height += int(weight * freeSpace)

        # now do the normal layout thing
        self.layout()

    def layout(self):
        y = 0
        for child in self.children:
            if (isinstance(child, Layout)):
                child.layout()

            if self._stretch:
                child.width = self.width

            child.position = (0, y)
            y += child.height + self._pad

        if self.children:
            self.width = max([child.width + self._pad for child in self.children]) - self._pad
        self.height = y - self._pad

class HorizontalBoxLayout(Layout):
    'Arranges its children in a horizontal row.'
    def __init__(self, *args, **kw):
        super(HorizontalBoxLayout, self).__init__(*args, **kw)
        self._pad = kw.get('pad', 0)
        self._stretch = kw.get('stretch', False)

    def layout(self):
        x = 0
        for child in self.children:
            if (isinstance(child, Layout)):
                child.layout()

            if self._stretch:
                child.height = self.height

            child.position = (x, 0)
            x += child.width + self._pad

        if self.children:
            self.height = max(*[child.height + self._pad for child in self.children]) - self._pad

    def layoutWeighted(self, weights):
        # figure out how much empty space we have:
        freeSpace = self.width - sum([c.width + self._pad for c in self._children])

        # Any space to distribute?
        if freeSpace > 0:
            # do it
            for child, weight in zip(self._children, weights):
                child.width += int(weight * freeSpace)

        # now do the normal layout thing
        self.layout()

class GridLayout(Layout):
    '''
    Arranges its children in a grid.  Each grid 'cell' is as wide as the
    widest child, and as high as the highest child.
    '''
    def __init__(self, *args, **kw):
        super(GridLayout, self).__init__(*args, **kw)
        assert 'cols' in kw, 'GridLayout needs a cols argument. (the number of columns)'
        self._cols = kw['cols']
        self._pad = kw.get('pad', 0)
        self._stretch = kw.get('stretch', False)

    def layout(self):
        rowWidth = max([ child.width for child in self.children ]) + self._pad
        colHeight = max([ child.height for child in self.children ]) + self._pad

        row, col = 0, 0
        x, y = 0, 0
        for child in self.children:
            if (isinstance(child, Layout)):
                child.layout()

            child.position = x, y
            x += rowWidth
            col += 1
            if col >= self._cols:
                row, col = row + 1, 0
                x, y = 0, y + colHeight

            if self._stretch:
                child.size = rowWidth, colHeight

        if len(self.children) > self._cols:
            self.width = self._cols * rowWidth
            self.height = row * colHeight
        else:
            self.width = col * rowWidth
            self.height = colHeight

        self.width -= self._pad
        self.height -= self._pad

class FlexGridLayout(GridLayout):
    '''
    More robust GridLayout.  Each row and column is made to be as small
    as possible. (ie as wide or high as the widest or tallest child in that
    row or column)
    '''
    def layout(self):
        for child in self.children:
            if isinstance(child, Layout):
                child.layout()

        # create a 2D matrix to hold widgets for each column
        cols = [[] for x in range(self._cols)]
        for i, child in enumerate(self.children):
            cols[i % self._cols].append(child)

        # Get the widest child in each column
        rowWidths = [
            max([cell.width + self._pad for cell in col])
            for col in cols
            ]

        # get the tallest child in each row
        # Difficult to parse. :x
        # This is what I get for trying to be clever.
        colSize = len(cols[0]) # cols[0] will always be the biggest column
        colHeights = [
                # pad size plus the width of the highest...
                self._pad + max([cell.height for cell in

                        # ... child in each row
                        [col[rowIndex] for col in cols if rowIndex < len(col)]

                    ])
                for rowIndex in range(colSize)
            ]

        # At this point, all the sizes have been worked out.  It's merely
        # a matter of setting them now.
        row, col = 0, 0
        x, y = 0, 0
        for child in self.children:
            child.position = x + self.x, y + self.y

            if self._stretch:
                child.size = rowWidths[col], colHeights[row]

            x += rowWidths[col]
            col += 1
            if col >= self._cols:
                x, y = 0, y + colHeights[row]
                row, col = row + 1, 0

        self.width = max([child.right for child in self.children]) - self._pad - self.x
        self.height = max([child.bottom for child in self.children]) - self._pad - self.y
