/*!
    Generic representation of a configuration file.

    user.cfg - per USER config options
  
    - engine runtime options. (resolution etc..)
    - Location of EXEs, DLLs and standard headers. (with the VH extension)
    
    game.cfg - per GAME config. (seeing a trend?) This should be in the game directory (if applicable)
      
    - Location of data files. (could be of any extension, arranged any way)
*/

#pragma once

#include <map>

#include "common/utility.h"

struct CConfigFile {
    typedef std::map<std::string, std::string> ConfigMap;
    
    ConfigMap keys;

public:
    CConfigFile();
    CConfigFile(const char* fname);

    void        Add(const std::string& key, const std::string& value);
    std::string      operator [](const std::string& key);
    int         Int(const std::string& key);

    void        Load(const char* fname);
    void        Save(const char* fname);
};

