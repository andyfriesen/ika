
#include "configfile.h"
#include "fileio.h"

CConfigFile::CConfigFile(const char* fname)
{
    Load(fname);
}

void CConfigFile::Add(const string& key,const string& value)
{
    if (key.length()==0 || value.length()==0)
        return;

    keys[key]=value;
}

string CConfigFile::operator [](const string& key)
{
    ConfigMap::iterator i=keys.find(key);

    if (i==keys.end())
        return "";

    return (*i).second;
}

int CConfigFile::Int(const string& key)
{
    return atoi((*this)[key].c_str());
}

void CConfigFile::Load(const char* fname)
{
    File f;

    bool result=f.OpenRead(fname);
    if (!result)    return;

    char key[1024],value[1024];
    while (!f.eof())
    {
        f.ReadToken(key);
        f.ReadToken(value);

        // empty keys/values are no good
        if (!key[0] || !value[0])
            return;

        Add(key,value);
    }

    f.Close();
}

void CConfigFile::Save(const char* fname)
{
    File f;

    f.OpenWrite(fname);

    for (ConfigMap::iterator i=keys.begin(); i!=keys.end(); i++)
    {
        f.Write(i->first.c_str());
        f.Write(" ");
        f.Write(i->second.c_str());
        f.Write("\n");
    }

    f.Close();
}
