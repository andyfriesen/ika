'''A simple recursive flood fill tool. Easy to use, simply import
it into ikaMap, turn on ikaMap's script mode, select the tile
you want to fill with, and click. :)

Because it's recursive, if you're filling into a large area,
and your system doesn't have a lot of memory, it might crash.
Not to mention take a long time to run. So be conservative
with the size of the areas you're filling.

Special thanks to Zaratustra and andy for help with the
original algorithm.

-Hatchet
hatchet2k2@hotmail.com
'''

import ikamap
import sys
import noisetool

rtile = w = h = layer = tile = 0
depth = maxdepth = recursions = 0
layWidth = layHeight = 0

def Fill(tx, ty):
   global depth, maxdepth, recursions
   recursions += 1
   depth += 1
   if depth>maxdepth: maxdepth = depth #just a way of keeping track how far we recursed

   if depth == sys.getrecursionlimit()-1: #recursion limit reached
      sys.setrecursionlimit (sys.getrecursionlimit()*2) #double the recursion limit
      ika.Log("Recursion limit reached. Limit now set to "+str(sys.getrecursionlimit()))


   if not (0 <= tx < layWidth) or not (0 <= ty < layHeight):
       return   # out of the layer's bounds.  Stop.

   left = right = tx #left and right edges of the horizontal line to recurse from

   for x in range(tx, w): #drawing a line to the right of the given point
      if ika.Map.GetTile(x,ty,layer) == rtile: #tile is fillable, fill it.
         ika.Map.SetTile(x,ty,layer,noisetool.RandomTile(tile))
         right+=1
      elif x != tx:
         break #hit a tile that doesn't match, stop here.

   for x in range(tx-1, -1, -1): #drawing a line to the left
      if ika.Map.GetTile(x,ty,layer) == rtile: #tile is fillable, fill it.
         ika.Map.SetTile(x,ty,layer,noisetool.RandomTile(tile))
         left -= 1
      else:
         #left = x+1
         break #hit a tile that doesn't match, stop here.

   for x in range(left, right): #search the line for possible paths up or down
      if ty-1>=0 and ika.Map.GetTile(x,ty-1,layer) == rtile:
         Fill(x,ty-1) #upwards
      if ty+1<h and ika.Map.GetTile(x,ty+1,layer) == rtile:
         Fill(x,ty+1) #recurse downwards

   depth -= 1

def OnMouseDown(mx, my): #called when the mouse is clicked
   global rtile, w, h, layer, tile, maxdepth, recursions
   global layWidth, layHeight

   maxdepth = recursions = 0

   layWidth, layHeight = ika.Map.GetLayerSize(layer)

   tx, ty = ika.Editor.MapToTile(mx, my)  #gets the x/y position in tiles of where we clicked.
   layer = ika.Editor.curlayer            #get the current layer that we will be filling to
   w, h = ika.Map.GetLayerSize(layer)     #width and height of the layer
   rtile = ika.Map.GetTile(tx, ty, layer) #tile we are replacing
   tile = ika.Editor.curtile              #tile we are filling with

   if tile == rtile: return               #don't want to fill over the same tile!

   Fill(tx,ty)                            #start filling!


   #uncomment for recursion info
   ika.Log("Maximum recursion depth: "+str(maxdepth))
   ika.Log("Number of recursions: "+str(recursions))




