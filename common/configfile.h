/*
    Since most of the apps use the same configuration files, we'll stuff them here.

    user.cfg - per USER config options
  
    - engine runtime options. (resolution etc..)
    - Location of EXEs, DLLs and standard headers. (with the VH extension)
    
    game.cfg - per GAME config. (seeing a trend?) This should be in the game directory (if applicable)
      
    - Location of data files. (could be of any extension, arranged any way)
*/

#ifndef CONFIG_H
#define CONFIG_H

#pragma warning (disable:4786)

#include "misc.h"
#include <map>
#include "types.h"

class CConfigFile
{
    typedef std::map<string,string> ConfigMap;
    
    ConfigMap keys;

public:
    CConfigFile() {}
    CConfigFile(const char* fname);

    void        Add(const string& key,const string& value);
    string Get(const string& key);
    int         GetInt(const string& key);

    void        Load(const char* fname);
    void        Save(const char* fname);
};

#endif