import sys
import os

env = Environment(ENV=os.environ, tools=['mingw'])
env.EnsurePythonVersion(2, 4)
env.EnsureSConsVersion(0, 9)

def tokenlist(x,*y):
    if type(x) is list: return x + list(y)
    return [x] + list(y)
def include(*x): env.Append(CPPPATH = tokenlist(*x))
def libs(*x):    env.Append(LIBS = tokenlist(*x))
def libpath(*x): env.Append(LIBPATH = tokenlist(*x))
def Alert(msg):
    return env.Action((lambda t,s,e: 0),
        (lambda target, source, env: '** ' + msg % locals())
    )

############ Set up for building common sources

include('.', '..')
if sys.platform == 'win32':
    # Win32 specific configuration
    include('#/3rdparty/include')
    libpath('#/3rdparty/lib')
    libs('opengl32')

    if 'msvc' in env['TOOLS']:
        env.Append(CXXFLAGS = '/EHsc /MD')
        env.Append(LINKFLAGS = ' /SUBSYSTEM:CONSOLE')
        libs('zlib', 'python24')
    elif 'mingw' in env['TOOLS']:
        libs('mingw32')
        env.Append(LINKFLAGS = ' -mwindows')
        libs('z', 'python2.4')

    # Python linking info
    import distutils.sysconfig as sc
    PY_INC = sc.get_python_inc()
    PY_LIB = sc.get_config_var('LIBDEST')
    PY_VER = sc.get_config_var('VERSION')
    include(PY_INC)
    libpath(PY_LIB + '/config')

else:
    # *nix specific configuration
    libpath('/usr/X11R6/lib')
    libs('GL', 'GLU', 'util')
    env.ParseConfig('python2.4-config --cflags')
    env.ParseConfig('python2.4-config --libs')

############## Build common sources

libcommon = env.SConscript(dirs=['common'], exports='env')
env.Alias('common', libcommon)

include('#/common')
libpath('#/common')
libs('common')

############## Set up for and build ika

ika_env = env.Copy()

ika_env.Append(LIBS=Split('''
    SDL
    audiere
    corona
'''))

# SDL linking info
ika_env.ParseConfig('sdl-config --cflags')
if sys.platform == 'win32':
    ika_env.Append(LIBS=['SDLmain'])

ika_env.Append(LINKFLAGS = '-Wl,--export-dynamic')
ika = ika_env.SConscript(dirs=['engine'], exports='ika_env')
ika_env.Alias('ika', ika)
ika_env.Default('ika')

################ Build ikaMap? Currently segfaults here.

ikaMap_env = env.Copy()

ikaMap_env.ParseConfig('wx-config --cppflags --libs --gl-libs')
ikaMap_env.Append(LIBS=['wx_gtk2_xrc-2.4']) # not platform independent (yet)
ikamap = ikaMap_env.SConscript(dirs=['ikaMap'], exports='ikaMap_env')
ikaMap_env.Alias('ikamap', ikamap)
if ARGUMENTS.get('ikamap', 'no') != 'no':
    ikaMap_env.Default('ikamap')

################ Installation
ikaMap_env['PREFIX'] = ARGUMENTS.get('prefix', '/usr')
ikaMap_env.Alias('install', ikaMap_env.Install(dir = '$PREFIX/bin',
        source = ika))
