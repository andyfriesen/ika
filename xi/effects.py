#!/usr/bin/env python

# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import math

import ika

import xi.color


def rotatePoint(x, y, angle):
    r = math.hypot(x, y)
    theta = math.atan(float(y) / float(x))
    if x < 0:
        theta += math.pi
    theta += angle
    x = r * math.cos(theta)
    y = r * math.sin(theta)
    return x, y


def rotateBlit(image, centerx, centery, angle, scale=1.0,
               blendmode=ika.AlphaBlend):
    halfx = image.width / 2
    halfy = image.height / 2
    # TODO: use a matrix to make this more efficient.
    points[0] = RotatePoint(-halfx, -halfy, angle)
    points[1] = RotatePoint(halfx, -halfy, angle)
    points[2] = [-point for point in points[0]]
    points[3] = [-point for point in points[1]]
    points = [(int(points[0][i] * scale + centerx),
               int(points[1][i] * scale + centery)) for i in range(4)]
    ika.Video.DistortBlit(image, *points + (blendmode,))


def fade(time, startColour=xi.color.transparent, endColour=xi.color.black,
         draw=ika.Render):
    startcolor = ika.GetRGB(startColour)
    endcolor = ika.GetRGB(endColour)
    deltacolor = [start - end for end, start in zip(startcolor, endcolor)]
    t = ika.GetTime()
    endtime = t + time
    saturation = 0.0
    while t < endtime:
        i = ika.GetTime() - t
        t = ika.GetTime()
        saturation = min(saturation + float(i) / time, 1.0)
        draw()
        color = [int(start + delta * saturation)
                  for start, delta in zip(startcolor, deltacolor)]
        ika.Video.DrawRect(0, 0, ika.Video.xres, ika.Video.yres,
                           ika.RGB(*color), True)
        ika.Video.ShowPage()
        ika.Input.Update()
        while t == ika.GetTime():
            ika.Input.Update()


def fadeIn(time, color=xi.color.black, draw=ika.Render):
    fade(time, color, xi.color.transparent, draw)


def fadeOut(time, color=xi.color.black, draw=ika.Render):
    fade(time, xi.color.transparent, color, draw)


def effect1(current_time, startscale, scalerange, screen):
    blue = 9
    green = 9 - min(int(current_time) / startscale, 6)
    red = 8 - min(int(current_time) / startscale, 8)
    ika.Video.DrawRect(0, 0, ika.Video.xres, ika.Video.yres,
                       ika.RGB(red, green, blue, 250), True)
    RotateBlit(screen, ika.Video.xres / 2,
               int(ika.Video.yres / 2 + math.sqrt(current_time)),
               math.pi / 95, 1.1 - current_time / scalerange, ika.AddBlend)
    RotateBlit(screen, ika.Video.xres / 2,
               int(ika.Video.yres / 2 - math.sqrt(current_time)), math.pi / 95,
               .5 + current_time / scalerange, ika.AddBlend)


def effect1a(current_time, startscale, scalerange, screen):
    red = 9
    green = 9 - min(current_time / startscale, 6)
    blue = 8 - min(current_time / startscale, 8)
    #ika.Video.DrawRect(0, 0, ika.Video.xres, ika.Video.yres,
    #                   ika.RGB(red, green, blue, 250), True)
    RotateBlit(screen, ika.Video.xres / 2,
               ika.Video.yres / 2 + math.sqrt(current_time), math.pi / 95,
               1.1 - current_time / scalerange, ika.Opaque)
    RotateBlit(screen, ika.Video.xres / 2,
               ika.Video.yres / 2 - math.sqrt(current_time), math.pi / 95,
               .5 + current_time / scalerange, ika.Opaque)


def effect2(current_time, startscale, scalerange, screen):
    red = 9
    green = 9 - min(current_time / startscale, 6)
    blue = 8 - min(current_time / startscale, 8)
    ika.Video.DrawRect(0, 0, ika.Video.xres, ika.Video.yres,
                       ika.RGB(red, green, blue, 20), True)
    RotateBlit(screen, ika.Video.xres / 2,
               ika.Video.yres / 2 + math.sqrt(current_time), math.pi / 15,
               .6 + current_time / scalerange, ika.AddBlend)
    RotateBlit(screen, ika.Video.xres / 2,
               ika.Video.yres / 2 - math.sqrt(current_time), math.pi / 15,
               .6 + current_time / scalerange, ika.AddBlend)


def effect3(current_time, startscale, scalerange, screen):
    ika.Video.DrawRect(0, 0, ika.Video.xres, ika.Video.yres, xi.color.black, True)
    RotateBlit(screen, ika.Video.xres / 2,
               ika.Video.yres / 2 + math.sqrt(current_time), math.pi / 55, .6,
               ika.AddBlend)
    RotateBlit(screen, ika.Video.xres / 2,
               ika.Video.yres / 2 - math.sqrt(current_time), math.pi / 55, .6,
               ika.AddBlend)


def blurry(callback=lambda: None, startscale=100, endscale=500, scalestep=1.5):
    scalerange = endscale - startscale
    ika.Render()
    screen = ika.Video.GrabImage(0, 0, ika.Video.xres, ika.Video.yres)
    time = ika.GetTime()
    i = 0
    while i < scalerange:
        x = ika.Video.xres * (i + startscale) / startscale
        y = ika.Video.yres * (i + startscale) / startscale
        Effect1(i, startscale, endscale, screen)
        screen = ika.Video.GrabImage(0, 0, ika.Video.xres, ika.Video.yres)
        ika.Video.ShowPage()
        ika.Input.Update()
        callback()
        i += (ika.GetTime() - time) * scalestep
        time = ika.GetTime()
