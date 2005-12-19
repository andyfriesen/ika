#!/usr/bin/env python

# author: Ian D. Bollinger
# This file has been placed in the public domain.


class Node(list):

    def __init__(self, name=''):
        super(Node, self).__init__()
        self.name = name

    def __getitem__(self, key):
        if isinstance(key, basestring):
            for child in self:
                if isinstance(child, Node) and child.name == key:
                    if len(child) == 1:
                        return child[0]
                    else:
                        return child
            raise KeyError(key)
        else:
            return super(Node, self).__getitem__(key)

    def get(self, key, default=None):
        try:
            child = self[key]
            if len(child) == 1:
                return child[0]
            else:
                return child
        except KeyError:
            return default

    def get_all(self, key):
        children = Node()
        for child in self:
            if isinstance(child, Node) and child.name == key:
                children.append(child)
        return children

    def to_dict(self, flat=True):
        if flat:
            if len(self) == 1 and not isinstance(self[0], Node):
                return self[0]
            else:
                result = {}
                for child in self:
                    if isinstance(child, Node):
                        result[child.name] = child.todict(flat)
                    else:
                        key = 'cdata'
                        i = 0
                        while key in result:
                            key = '_cdata%s' % i
                        result[key] = child
                return result
        else:
            L = []
            for node in self:
                if isinstance(node, Node):
                    L.append(node.todict(flat))
                else:
                    L.append(node)
            return {self.name: L}

    def __str__(self, indentlevel=0):
        indent = '\t' * indentlevel
        if not self:
            return '(%s)' % self.name
        elif len(self) == 1 and not isinstance(self[0], Node):
            return '(%s %s)' % (self.name, self[0])
        else:
            children = '\n'
            for child in self:
                children += indent + '\t'
                try:
                    children += child.__str__(indentlevel + 1)
                except TypeError:
                    children += str(child)
                children += '\n'
            return '(%s %s%s)' % (self.name, children, indent)

    def __repr__(self):
        return '<Node %s>' % self.name


class Document(object):

    def __init__(self, source):
        super(_Document, self).__init__()
        self._position = 0
        if isinstance(source, basestring):
            source = open(source)
        self.source = source.read()

    def get(self):
        self._position += 1
        return self.source[self._position - 1]

    def peek(self, symbol):
        return self.source[self._position:
                           self._position + len(symbol)] == symbol

    def _grab(self, symbols):
        locations = []
        for symbol in symbols:
            match = self.source[self._position:].find(symbol)
            if match == -1:
                match = len(self.source)
            locations.append(match + len(symbol) - 1)
        return min(locations)

    def _grab_comment(self):
        old = self._position
        self._position += self._grab(['--)']) + 1
        return self.source[old:self._position]

    def _grab_identifier(self):
        old = self._position
        self._position += self._grab(' \n\t()')
        return self.source[old:self._position]

    def _grab_string(self):
        old = self._position
        self._position += self._grab('()')
        return self.source[old:self._position].rstrip()

    def parse(self):
        root = Node()
        L = [root]
        while self._position < len(self.source):
            character = self.get()
            if character == '(':
                if self.peek('--'):
                    self._grab_comment()
                else:
                    node = Node(self._grab_identifier())
                    L[-1].append(node)
                    L.append(node)
            elif character == ')':
                assert len(L) > 1, \
                    'Malformed document: mismatched end parenthesis.'
                L.pop()
            elif not character.isspace():
                L[-1].append(character + self._grab_string())
        return root


def load(source):
    return Document(source).parse()
