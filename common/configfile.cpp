
#include "configfile.h"
#include "fileio.h"

CConfigFile::CConfigFile()
{
    // Set some defaults
    keys["xres"] = "640";
    keys["yres"] = "480";
    keys["bitdepth"] = "32";
    keys["frameskip"] = "10";
    keys["videodriver"] = "opengl";
}

CConfigFile::CConfigFile(const char* fname)
{
    // Set some defaults (constructors can't call each other.  GAY GAY GA YGAY GAY AGY AGYGA YGAGY AGYA)
    keys["xres"] = "640";
    keys["yres"] = "480";
    keys["bitdepth"] = "32";
    keys["frameskip"] = "10";
    keys["videodriver"] = "opengl";

    Load(fname);
}

void CConfigFile::Add(const std::string& key, const std::string& value)
{
    if (key.length()==0 || value.length()==0)
        return;

    keys[key]=value;
}

std::string CConfigFile::operator [](const std::string& key)
{
    ConfigMap::iterator i = keys.find(key);

    if (i==keys.end())
        return "";

    return i->second;
}

int CConfigFile::Int(const std::string& key)
{
    return atoi((*this)[key].c_str());
}

void CConfigFile::Load(const char* fname)
{
    File f;

    bool result = f.OpenRead(fname);
    if (!result)    
        return;

	std::string key;
	std::string value;
    while (!f.eof())
    {
        f.ReadToken(key);
        f.ReadToken(value);

        // empty keys / values are no good
        if (key.empty() || value.empty())
            return;

        Add(key, value);
    }

    f.Close();
}

void CConfigFile::Save(const char* fname)
{
    File f;

    f.OpenWrite(fname);

    for (ConfigMap::iterator i = keys.begin(); i!=keys.end(); i++)
    {
        f.Write(i->first.c_str());
        f.Write(" ");
        f.Write(i->second.c_str());
        f.Write("\n");
    }

    f.Close();
}
