#ifndef XMLUTIL_H
#define XMLUTIL_H

#include <cppdom/cppdom.h>

cppdom::XMLNodePtr CData(cppdom::XMLContextPtr& context, const std::string& data);
cppdom::XMLNodePtr MetaNode(cppdom::XMLContextPtr& context, const std::string& type, const std::string& value);
cppdom::XMLNodePtr GetNode(const cppdom::XMLNodePtr& parent, const std::string& name);
std::string GetCdata(const cppdom::XMLNodePtr& node);

#endif