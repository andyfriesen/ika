# Gaudy eye candy.
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import math
import ika

def rotatePoint(x, y, angle):
    r = math.hypot(x, y)
    theta = math.atan(float(y) / float(x))
    if x < 0:
        theta += math.pi

    theta += angle
    x = r * math.cos(theta)
    y = r * math.sin(theta)
    return x, y

def rotateBlit(img, cx, cy, angle, scale = 1.0, blendmode = ika.AlphaBlend):
    halfx = img.width / 2
    halfy = img.height / 2
    # TODO: use a matrix to make this more efficient.
    p1 = RotatePoint(-halfx, -halfy, angle)
    p2 = RotatePoint(halfx, -halfy, angle)
    p3 = (-p1[0], -p1[1])
    p4 = (-p2[0], -p2[1])

    p1 = int(p1[0] * scale + cx), int(p1[1] * scale + cy)
    p2 = int(p2[0] * scale + cx), int(p2[1] * scale + cy)
    p3 = int(p3[0] * scale + cx), int(p3[1] * scale + cy)
    p4 = int(p4[0] * scale + cx), int(p4[1] * scale + cy)

    ika.Video.DistortBlit(img, p1, p2, p3, p4, blendmode)

def fade(time, startColour = ika.RGB(0, 0, 0, 0), endColour = ika.RGB(0, 0, 0, 255), draw = ika.Map.Render):
    startColour = ika.GetRGB(startColour)
    endColour   = ika.GetRGB(endColour)
    deltaColour = [ s - e for e, s in zip(startColour, endColour) ]

    t = ika.GetTime()
    endtime = t + time
    saturation = 0.0

    while t < endtime:
        i = ika.GetTime() - t
        t = ika.GetTime()
        saturation = min(saturation + float(i) / time, 1.0)
        draw()
        colour = [int(a + b * saturation) for a, b in zip(startColour, deltaColour)]

        ika.Video.DrawRect(0, 0, ika.Video.xres, ika.Video.yres,
            ika.RGB(*colour),
            True)

        ika.Video.ShowPage()
        ika.Input.Update()

        while t == ika.GetTime():
            ika.Input.Update()

def fadeIn(time, colour = ika.RGB(0, 0, 0), draw = ika.Map.Render):
    fade(time, colour, ika.RGB(0, 0, 0, 0), draw)

def fadeOut(time, colour = ika.RGB(0, 0, 0), draw = ika.Map.Render):
    fade(time, ika.RGB(0, 0, 0, 0), colour, draw)

def effect1(curTime, startScale, scaleRange, scr):
    b = 9
    g = 9 - min(int(curTime) / startScale, 6)
    r = 8 - min(int(curTime) / startScale, 8)
    ika.Video.DrawRect(0, 0, xres, yres, ika.RGB(r, g, b, 250), True)
    RotateBlit(
        scr,
        xres / 2,
        int(yres / 2 + math.sqrt(curTime)),
        math.pi / 95,
        1.1 - curTime / scaleRange,
        ika.AddBlend)
    RotateBlit(scr, xres / 2, int(yres / 2 - math.sqrt(curTime)), math.pi / 95, .5 + curTime / scaleRange, ika.AddBlend)

def effect1a(curTime, startScale, scaleRange, scr):
    r = 9
    g = 9 - min(curTime / startScale, 6)
    b = 8 - min(curTime / startScale, 8)
    #ika.Video.DrawRect(0, 0, xres, yres, ika.RGB(r, g, b, 250), True)
    RotateBlit(scr, xres / 2, yres / 2 + math.sqrt(curTime), math.pi / 95, 1.1 - curTime / scaleRange, ika.Opaque)
    RotateBlit(scr, xres / 2, yres / 2 - math.sqrt(curTime), math.pi / 95, .5 + curTime / scaleRange, ika.Opaque)

def effect2(curTime, startScale, scaleRange, scr):
    r = 9
    g = 9 - min(curTime / startScale, 6)
    b = 8 - min(curTime / startScale, 8)
    ika.Video.DrawRect(0, 0, xres, yres, ika.RGB(r, g, b, 20), True)
    RotateBlit(scr, xres / 2, yres / 2 + math.sqrt(curTime), math.pi / 15, .6 + curTime / scaleRange, ika.AddBlend)
    RotateBlit(scr, xres / 2, yres / 2 - math.sqrt(curTime), math.pi / 15, .6 + curTime / scaleRange, ika.AddBlend)

def effect3(curTime, startScale, scaleRange, scr):
    ika.Video.DrawRect(0, 0, xres, yres, ika.RGB(0, 0, 0, 100), True)
    RotateBlit(scr, xres / 2, yres / 2 + math.sqrt(curTime), math.pi / 55, .6, ika.AddBlend)
    RotateBlit(scr, xres / 2, yres / 2 - math.sqrt(curTime), math.pi / 55, .6, ika.AddBlend)

def blurry(callback = lambda: None):
    global xres, yres

    startscale = 100
    endscale = 500
    scalestep = 1.5
    scaleRange = endscale - startscale

    xres = ika.Video.xres
    yres = ika.Video.yres

    ika.Map.Render()
    scr = ika.Video.GrabImage(0, 0, ika.Video.xres, ika.Video.yres)

    t = ika.GetTime()
    i = 0
    while i < scaleRange:
        x = xres * (i + startscale) / startscale
        y = yres * (i + startscale) / startscale

        Effect1(i, startscale, endscale, scr)

        scr = ika.Video.GrabImage(0, 0, ika.Video.xres, ika.Video.yres)
        ika.Video.ShowPage()
        ika.Input.Update()
        callback()

        i += (ika.GetTime() - t) * scalestep
        t = ika.GetTime()
