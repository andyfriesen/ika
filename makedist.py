import sys
import os, os.path
import shutil
import pysvn

from shutil import copy, copyfile
from zipfile import ZipFile, is_zipfile



def zip_files(src, dest_name=None, dest_zip=None, folder=''):
    if dest_zip is None:
        z = ZipFile(dest_name, "w")

    else:
        z = dest_zip

    for file in os.listdir(src):
        full_src = src + "/" + file

        if os.path.isdir(full_src):
            zip_files(full_src, dest_zip=z, folder=folder + file + "/")

        else:
            z.write(src + '/' + file, folder + file)

    if dest_zip is None:
        z.close()

    return z

def copy_files(src_path, dest_path):
    if not os.path.exists(dest_path):
        os.mkdir(dest_path)

    for file in os.listdir(src_path):
        full_src = src_path + "/" + file
        full_dest = dest_path + "/" + file

        if file == '.svn':
            continue

        if os.path.isdir(full_src):
            os.mkdir(full_dest)
            copy_files(full_src, full_dest)

        else:
            copyfile(src_path + "/" + file, dest_path + "/" + file)






if len(sys.argv) == 1:
    print "Needs a version, dummy!"
    sys.exit()

else:
    version = sys.argv[1]

"""

Create our folders here.

"""

path = os.path.dirname(__file__) + '/'
makedist_path = path + 'makedist_temp'
win_path = makedist_path + '/ika-' + version
demo_path = win_path + '/demo'
core_path = makedist_path + '/core'
src_path = makedist_path + '/src'


if os.path.exists(makedist_path):
    print "Temporary folder still exists. Deleting..."
    shutil.rmtree(makedist_path)

# Create folders here
print "Creating temporary folders"

os.mkdir(makedist_path)
os.mkdir(win_path)
os.mkdir(demo_path)
os.mkdir(core_path)
os.mkdir(src_path)

# core.zip
print "Assembling core.zip"

copy("engine/Release/ika.exe", core_path)
copy("ikamap/Release/ikaMap.exe", core_path)

# Compress exes.

# zip files
zip_files(core_path, path + "ika-core-" + version + ".zip")

print "Done"


# Windows ZIP
print "Assembling Windows distribution"

copy("3rdparty/dlls/audiere.dll", demo_path)
copy("3rdparty/dlls/corona.dll", demo_path)
copy("3rdparty/dlls/corona.dll", win_path)
copy("3rdparty/dlls/msvcp90.dll", demo_path)
copy("3rdparty/dlls/msvcr90.dll", demo_path)
copy("3rdparty/dlls/python25.dll", demo_path)
copy("3rdparty/dlls/python25.dll", win_path)
copy("3rdparty/dlls/zlib.dll", demo_path)
copy("3rdparty/dlls/zlib.dll", win_path)
copy("3rdparty/dlls/sdl.dll", demo_path)
copy("3rdparty/dlls/sdl.dll", win_path)
copy("3rdparty/dlls/Microsoft.VC90.CRT.manifest", demo_path)
copy("engine/Release/ika.exe", demo_path)
copy("ikamap/Release/ikaMap.exe", win_path)

copy_files(path + "dist", demo_path)
copy_files(path + "doc", win_path + "/doc")

zip_files(win_path, path + "ika-win-" + version + ".zip")

print "Done"


# Source
print "Assembling source"

client = pysvn.Client()

# Function to trust the SVN server.
def ssl_server_trust_prompt(trust):
    return True, 1, True

client.callback_ssl_server_trust_prompt = ssl_server_trust_prompt

# Go through each folder and export it from SVN
for folder in ('common', 'engine', 'ikaMap'):
    print "Exporting %s from repository" % folder
    client.export('https://ika.svn.sourceforge.net/svnroot/ika/trunk/%s' % folder, src_path + '/' + folder)

zip_files(src_path, path + "ika-src-" + version + ".zip")

print "Removing files"
shutil.rmtree(makedist_path)