
#include "xmlutil.h"
#include "misc.h"
#include "log.h"

using namespace cppdom;

XMLNodePtr CData(XMLContextPtr& context, const std::string& data)
{
    XMLNodePtr node(new XMLNode(context));
    node->setType(xml_nt_cdata);
    node->setCdata(data);
    return node;
}

XMLNodePtr MetaNode(XMLContextPtr& context, const std::string& type, const std::string& value)
{
    XMLNodePtr node(new XMLNode(context));
    node->setName("meta");
    node->setAttribute("type", XMLAttribute(type));
    node->addChild(CData(context, value));

    return node;
}

XMLNodePtr GetNode(const XMLNodePtr& parent, const std::string& name)
{
    XMLNodePtr ptr = parent->getChild(name);
    if (!ptr.get())
        throw va("<%s> tag not found.", name.c_str());
    return ptr;
}

std::string GetCdata(const XMLNodePtr& node)
{
    if (node->getChildren().size() == 0) return "";
    XMLNodePtr ptr = node->getChildren().front();
    if (ptr.get() == 0 || ptr->getType() != xml_nt_cdata)
    {
        Log::Write("No cdata found in <%s>", node->getName().c_str());
        return "";
    }
    return ptr->getCdata();
}