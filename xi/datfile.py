#!/usr/bin/env python

def Read(string):
    starting_position = string.find('(')
    ending_position = string.rfind(')')
    if starting_position != -1 and ending_position != -1:
        return [string[:starting_position].split() +
                Read(string[starting_position + 1 : ending_position]) +
                string[ending_position + 1:].split()]
            
    else:
        return [string.split()]
