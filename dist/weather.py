
from xi import field
from xi.textbox import text
from xi import music
import stats
import ika

CLOUD_COUNT = 7

def beginGlow():
    ika.HookRetrace(drawGlow)
def endGlow():
    ika.UnhookRetrace(drawGlow)
def drawGlow():
    factor=0.10
    tint = ika.RGB(128, 128, 128)

    screen = ika.Video.GrabImage(0, 0, ika.Video.xres, ika.Video.yres)
    ika.Video.ScaleBlit(screen, 0, 0, screen.width * factor, screen.height * factor)
    smallScreen = ika.Video.GrabImage(0, 0, screen.width * factor, screen.height * factor)

    ika.Video.Blit(screen, 0, 0, ika.Opaque)
    ika.Video.TintDistortBlit(
        smallScreen,
        (0, 0, tint),
        (ika.Video.xres, 0, tint),
        (ika.Video.xres, ika.Video.yres, tint),
        (0, ika.Video.yres, tint),
        ika.AddBlend
    )

def cloudWeather():
    lay = ika.Map.layercount - 1 # on the top layer
    for i in range(CLOUD_COUNT):
        cloud = ika.Entity(
            0, 0,
            lay, 'cloud.ika-sprite')
        cloudReset(cloud)
        cloud.Stop()
        cloud.speed = 10
        cloud.entobs = cloud.mapobs = False
        cloud.movescript = cloudMove
        ika.Map.entities['cloud%i' % i] = cloud

def endWeather():
    '''
    Nothing to do here yet.  Clouds are normal entities and get cleaned up
    by the engine itself.
    '''
    pass

def cloudReset(cloud):
    '''
    Doesn't really spawn, just moves the entity where it may pop up
    onscreen again
    '''
    # make some neato constants
    TOP, BOTTOM, LEFT, RIGHT = range(4)

    # pick a screen edge to place the cloud at

    while True:
        edge = ika.Random(0, 4)
        if edge == TOP or edge == BOTTOM:
            # top/bottom edge
            x = ika.Random(0, ika.Video.xres)
            cloud.x = x + ika.Map.xwin
            cloud.y = ika.Map.ywin
            if edge == TOP:
                cloud.y -= cloud.hotheight
            elif edge == BOTTOM:
                cloud.y += ika.Video.yres
            else:
                assert False, '???'
        else:
            # left/right edge
            y = ika.Random(0, ika.Video.yres)
            cloud.x = ika.Map.xwin
            cloud.y = y + ika.Map.ywin
            if edge == LEFT:
                cloud.x -= cloud.hotwidth
            elif edge == RIGHT:
                cloud.x += ika.Video.xres

        # clouds should not overlap.  go back if the cloud touches another
        if cloud.DetectCollision():
            continue
        else:
            break

def cloudMove(cloud):
    xw, yw = ika.Map.xwin, ika.Map.ywin
    x2 = xw + ika.Video.xres
    y2 = yw + ika.Video.yres

    if ((not (xw - cloud.hotwidth < cloud.x < x2) or
        not (yw - cloud.hotheight < cloud.y < y2)) and
        (ika.Random(0, 100) > 70)
        ): # :(
        cloudReset(cloud)

    cloud.MoveTo(cloud.x - 16, cloud.y + 1)
