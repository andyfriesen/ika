
#include "configfile.h"
#include "fileio.h"

CConfigFile::CConfigFile()
    : _good(false)
{}

CConfigFile::CConfigFile(const char* fname)
    : _good(false)
{
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
    {
        _good = false;
        return;
    }

    char key[1024], value[1024];
    while (!f.eof())
    {
        f.ReadToken(key);
        f.ReadToken(value);

        // empty keys / values are no good
        if (!key[0] || !value[0])
            return;

        Add(key, value);
    }

    f.Close();
    _good = true;
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
