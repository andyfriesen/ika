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
#include <string>

class CConfigFile
{
    typedef std::map<std::string,std::string> ConfigMap;
    
    ConfigMap keys;

public:
    CConfigFile() {}
    CConfigFile(const char* fname);

    void        Add(const std::string& key,const std::string& value);
    std::string Get(const std::string& key);
    int         GetInt(const std::string& key);

    void        Write(const char* fname);
    void        Read (const char* fname);
};

#endif