# Just in case: 
# type python3.1-config --cflags in the terminal to see if you have the correct flags,
# but without the ones that produce useless "Hey, you can't use that for C++!" warnings. 

import platform
import sys
import os

env = Environment(ENV=os.environ, tools=['mingw'])
env.EnsureSConsVersion(1, 0)

def tokenlist(x,*y):
    if type(x) is list: return x + list(y)
    return [x] + list(y)
def cppflags(*x): env.Append(CPPFLAGS = tokenlist(*x))
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
        libs('zlib', 'python3.1')
    elif 'mingw' in env['TOOLS']:
        libs('mingw32')
        env.Append(LINKFLAGS = ' -mwindows')
        libs('z', 'python3.1')

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
    libs('GL', 'GLU', 'util', 'asound')
    cppflags('-fno-strict-aliasing', '-DNDEBUG', '-g', '-fwrapv', '-O2', '-Wall')
    cppflags('-Wno-unknown-pragmas')

    if platform.linux_distribution()[0] == 'Gentoo Base System':
        include('-I/usr/include/python3.1')        
        cppflags('-lpthread', '-ldl', '-lutil', '-lm', '-lpython3.1')        
    else:
        env.ParseConfig('python3.1-config --include')
        env.ParseConfig('python3.1-config --libs') 

############## Build common sources

env.VariantDir('build_common', 'common', duplicate=0)
libcommon = env.SConscript(dirs=['build_common'], exports='env')
env.Alias('common', libcommon)

include('#/build_common')
libpath('#/build_common')
libs('common')

# TODO: See if it's possible for the objects of the common library be only built once. 
############## Set up for and build ika

ika_env = env.Clone()

ika_env.Append(LIBS = ['SDL', 'audiere', 'corona'])

# SDL linking info
ika_env.ParseConfig('sdl-config --cflags')
if sys.platform == 'win32':
    ika_env.Append(LIBS=['SDLmain'])

ika_env.Append(LINKFLAGS = '-Wl,--export-dynamic')
ika_env.VariantDir('build', 'engine', duplicate=0)
ika = ika_env.SConscript(dirs=['build'], exports='ika_env')
ika_env.Alias('ika', ika)
ika_env.Default('ika')
