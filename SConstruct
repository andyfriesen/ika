import sys
import os

env = Environment(ENV=os.environ, tools=['mingw'])
env.EnsurePythonVersion(2,3)
env.EnsureSConsVersion(0,9)

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
    libs('opengl32', 'SDLmain')

    if 'msvc' in env['TOOLS']:
        env.Append(CXXFLAGS = '/EHsc /MD')
        env.Append(LINKFLAGS = ' /SUBSYSTEM:CONSOLE')
        libs('zlib', 'python23')
    elif 'mingw' in env['TOOLS']:
        libs('mingw32')
        env.Append(LINKFLAGS = ' -mwindows')
        libs('z', 'python2.3')
else:
    # *nix specific configuration
    libpath('/usr/X11R6/lib')
    libs('GL', 'GLU', 'util')

libs(Split('''
    SDL
    audiere
    corona
'''))
libs(sys.platform == 'win32' and 'zlib' or 'z')
libs(sys.platform == 'win32' and 'python23' or 'python2.3')

# SDL linking info
env.ParseConfig('sdl-config --cflags')

# Shared with all subdirectories
env.Export('env')

############## Build common sources

libcommon = env.SConscript(dirs=['common'])
env.Alias('common', libcommon)

############## Set up for and build ika

# Python linking info
import distutils.sysconfig as sc
PY_INC = sc.get_python_inc()
PY_LIB = sc.get_config_var('LIBDEST')
PY_VER = sc.get_config_var('VERSION')
include(PY_INC)
libpath(PY_LIB + '/config')

include('#/common')
libpath('#/common')
libs('common')

env.Append(LINKFLAGS = '-Wl,--export-dynamic')
ika = env.SConscript(dirs=['engine'])
env.Alias('ika', ika)
env.Default('ika')

################ Build ikaMap? Currently segfaults here.

env.ParseConfig('wx-config --cppflags --libs --gl-libs')
libs('wx_gtk2_xrc-2.4') # not platform independent (yet)
ikamap = env.SConscript(dirs=['ikaMap'])
env.Alias('ikamap', ikamap)
if ARGUMENTS.get('ikamap', 'no') != 'no':
    env.Default('ikamap')

################ Installation
env['PREFIX'] = ARGUMENTS.get('prefix', '/usr')
env.Alias('install', env.Install(dir = '$PREFIX/bin',
        source = ika))
