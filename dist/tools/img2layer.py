'''
Import Image To Layer -- Written by Praetor 2.18.04

This is a simple utility that takes a tileset imported from an image and 
reconstructs it at 0,0 of the current layer.  Make sure that the current
layer is the same size as the image, otherwise it won't line up properly
'''

from ikamap import *

def OnActivated():
	x = y = 0
	mx, my = Map.GetLayerSize(Editor.curlayer)

	for t in range(Map.tilecount):
		Map.SetTile(x, y, Editor.curlayer, t)

		x += 1
		if x >= mx:
			x = 0
			y += 1
			if y >= my:
				break