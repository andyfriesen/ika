import sys
import os

env = Environment(ENV=os.environ, tools=['gcc','g++','gnulink'])
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
    include('../3rdparty/include')
include('d:/cygwin/usr/include/python2.3')  # should not be necessary

libs(Split('''
    GL
    GLU
    SDL
    audiere
    corona
    z
'''))
if sys.platform != 'win32': # ???
    libs('util')

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
libs('m', 'python%s' % PY_VER)
libpath(PY_LIB + '/config')

include('../common')
libpath('../common')
libs('common')

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

# env.Install(dir = '/usr/local/bin', source = ['ika'])
