#  odds and ends
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

# wraps the text to the given
# returns a list of strings
# wrapwidth is in chars
def WrapText(s,wrapwidth):
	result=[]										# our list of strings

	while len(s)>0:
		n=s[:wrapwidth].find('\n')
		if n!=-1:									# newline?
			result.append(s[:n])
			s=s[n+1:]
			continue

		if len(s)<wrapwidth:						# enough room for the rest of the string?
			result.append(s)
			s=''
			break

		n=s[:wrapwidth].rfind(' ')					# find a space, then
		if n!=-1:
			result.append(s[:n])
			s=s[n+1:]
			continue

		result.append(s[:wrapwidth+1])				# no suitable place to chop?
		s=s[wrapwidth+1:]							# just hack it anywhere then
	return result

#--------------------------------------------------------------

import ika

def XRes():
    return ika.GetScreenImage().width
    
def YRes():
    return ika.GetScreenImage().height
    
def clamp(value, lower, upper):
    return max(min(value, upper), lower)