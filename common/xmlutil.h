#ifndef XMLUTIL_H
#define XMLUTIL_H

#include <cppdom/cppdom.h>

cppdom::XMLNodePtr CData(cppdom::XMLContextPtr& context, const char* data)
{
    cppdom::XMLNodePtr node(new cppdom::XMLNode(context));
    node->setType(cppdom::xml_nt_cdata);
    node->setCdata(data);
    return node;
}

cppdom::XMLNodePtr MetaNode(cppdom::XMLContextPtr& context, const char* type, const char* value)
{
    cppdom::XMLNodePtr node(new cppdom::XMLNode(context));
    node->setName("meta");
    node->setAttribute("type", cppdom::XMLAttribute(type));
    node->addChild(CData(context, value));

    return node;
}

#endif