
#include "aries.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <cassert>
#include <stdexcept>

namespace aries
{
    // Helper functions for the parser

    bool isWhiteSpace(char c)
    {
        return c==' ' || c=='\t' || c=='\n' || c=='\r';
        //static const std::string whiteSpace(" \t\n\r");
        //return whiteSpace.find(c) != std::string::npos;
    }

    std::string readIdentifier(std::istream& stream)
    {
        std::stringstream ss;

        do
        {
            char c = stream.peek();

            if (isWhiteSpace(c) ||
                !stream.good())
                break;

            stream.get();
            ss << c;
        } while (true);

        return ss.str();
    }

    void eatWhiteSpace(std::istream& stream)
    {
        // C++ badly needs a
        // do { ... } while (condition) { ... }
        // construct.
        do
        {
            char c = stream.peek();
        if (isWhiteSpace(c) || !stream.good())  break;
            stream.get();
        } while (1);
    }

    // Removes all leading and trailing whitespace from the string.
    std::string stripString(const std::string& str)
    {
        uint start = 0;
        uint end = str.length();

        while (isWhiteSpace(str[start]) && start < str.length()) start++;
        while (isWhiteSpace(str[end - 1]) && end > 0) end--;
        if (start >= end)
            return "";
        else
            return str.substr(start, end - start);
    }

    DataNode* Node::readDocument(std::istream& stream)
    {
        std::stack<DataNode*> docStack; // I don't want to recurse for some reason, so I keep the context in an explicit stack.
        DataNode* rootNode = new DataNode("root");
        docStack.push(rootNode);

        /*
         * Read a character.
         *   If it's not an opening parenth, then grab characters until we get to a parenth, and pack it into a single StringNode.
         *   If it is, then grab characters up until a space, then create a new DataNode, and parse its children.
         *   If it is a closing parenth, then the node is complete.  We resume parsing its parent.
         */

        while (stream.good())
        {
            assert(docStack.size() >= 1);

            char c = stream.get();

            if (c == '(')
            {
                std::string nodeName = stripString(readIdentifier(stream));
                DataNode* newNode = new DataNode(nodeName);
                docStack.top()->addChild(newNode);
                docStack.push(newNode);
            }
            else if (c == ')')
            {
                // the root node is 1, and you may not actually terminate that node, as it is implicit, and not part
                // of the document itself.
                if (docStack.size() < 2)
                    throw std::runtime_error("Malformed markup document");

                docStack.pop();
            }
            else
            {
                std::stringstream ss;
                while (c != '(' && c != ')' && stream.good())
                {
                    ss << c;
                    c = stream.get();
                }
                stream.unget();

                std::string s = stripString(ss.str());

                if (s.length())
                    docStack.top()->addChild(s);
            }
        }

        return rootNode;
    }

    //-------------------------------------------------------------------------

    StringNode::StringNode(const std::string& str)
        : _str(str)
    {}

    bool StringNode::isString() const
    {
        return true;
    }

    std::string StringNode::toString() const
    {
        return _str;
    }

    StringNode* StringNode::clone() const
    {
        return new StringNode(_str);
    }

    std::ostream& StringNode::write(std::ostream& stream) const
    {
        stream << _str;
        return stream;
    }

    //-------------------------------------------------------------------------

    DataNode::DataNode(const std::string& name)
        : _name(name)
    {}

    DataNode::~DataNode()
    {
        for (uint i = 0; i < _children.size(); i++)
            delete _children[i];
    }

    bool DataNode::isString() const
    {
        return false;
    }

    std::string DataNode::toString() const
    {
        std::stringstream ss;
        write(ss, 0);
        return ss.str();
    }

    DataNode* DataNode::clone() const
    {
        DataNode* newNode = new DataNode(_name);

        for (uint i = 0; i < _children.size(); i++)
        {
            newNode->addChild(*_children[i]);
        }

        return newNode;
    }

    std::string DataNode::getString() const
    {
        for (unsigned int i = 0; i < _children.size(); i++)
        {
            if (_children[i]->isString())
                return _children[i]->toString();
        }
        return "";
    }

    const NodeList& DataNode::getChildren() const
    {
        return _children;
    }

    NodeList& DataNode::getChildren()
    {
        return _children;
    }

    DataNodeList DataNode::getChildren(const std::string& name) const
    {
        DataNodeList list;

        for (NodeList::const_iterator i = _children.begin(); i != _children.end(); i++)
        {
            Node* n = *i;
            if (!n->isString() &&
                reinterpret_cast<DataNode*>(n)->getName() == name)
                list.push_back(reinterpret_cast<DataNode*>(n));
        }

        return list;
    }

    DataNode* DataNode::getChild(const std::string& name) const
    {
        DataNode* n = getChild(name, 0);
        if (!n)
            throw std::runtime_error(std::string("Unable to find node ") + name);
        else
            return n;
    }

    DataNode* DataNode::getChild(const std::string& name, DataNode* defaultValue) const
    {
        for (NodeList::const_iterator i = _children.begin(); i != _children.end(); i++)
        {
            Node* n = *i;
            if (!n->isString() &&
                reinterpret_cast<DataNode*>(n)->getName() == name)
                return reinterpret_cast<DataNode*>(n);
        }

        return defaultValue;
    }

    bool DataNode::hasChild(const std::string& name) const
    {
        for (NodeList::const_iterator i = _children.begin(); i != _children.end(); i++)
        {
            Node* n = *i;
            if (!n->isString() &&
                reinterpret_cast<DataNode*>(n)->getName() == name)
                return true;
        }
        return false;
    }

    std::string DataNode::getName() const
    {
        return _name;
    }

    DataNode* DataNode::addChild(const std::string& str)
    {
        return addChild(new StringNode(str));
    }

    DataNode* DataNode::addChild(int n)
    {
        char c[32];
        sprintf(c, "%i", n);
        return addChild(std::string(c));
    }

    /*DataNode* DataNode::addChild(double d)
    {
        char c[32];
        sprintf(c, "%d", d);
        return addChild(std::string(c));
    }*/

    DataNode* DataNode::addChild(Node* n)
    {
        _children.push_back(n);
        return this;
    }

    DataNode* DataNode::addChild(const Node& n)
    {
        _children.push_back(n.clone());
        return this;
    }

    std::ostream& DataNode::write(std::ostream& stream) const
    {
        write(stream, 0);
        return stream;
    }
    void DataNode::write(std::ostream& stream, uint indentLevel) const
    {
        indentLevel += 1;

        stream << "(" << _name << " ";

        if (_children.size() == 1 && _children[0]->isString())
        {
            // When there's just one string node, no newlines.  It's ugly looking.
            stream << _children[0]->toString() << ")";
        }
        else
        {
            for (uint i = 0; i < _children.size(); i++)
            {
                stream << "\n";
                stream << std::string(indentLevel, '\t');

                if (_children[i]->isString())
                    stream << _children[i]->toString();
                else
                    reinterpret_cast<DataNode*>(_children[i])->write(stream, indentLevel);
            }
            stream << "\n";
            stream << std::string(indentLevel - 1, '\t');
            stream << ")";
        }
    }

    void unittest()
    {
        aries::DataNode* root = new aries::DataNode("root");
        root->addChild(aries::StringNode("wee!"))
            ->addChild(
                aries::newNode("child")
                    ->addChild("String data!")
                )
            ->addChild(
                aries::newNode("child2")
                    ->addChild(
                        aries::newNode("child3")
                            ->addChild("nesting!")
                            ->addChild("This is so hot.")
                    )
                )
            ->addChild("FEEL THE BURN");

        std::stringstream ss;
        ss << root;

        std::cout << ss.str() << std::endl;

        aries::DataNode* n = aries::Node::readDocument(ss);

        std::cout << n << std::endl;

        delete root;
        delete n;
    }

    DataNode* newNode(const std::string& str)
    {
        return new DataNode(str);
    }
}

std::ostream& operator << (std::ostream& stream, aries::Node* node)
{
    node->write(stream);
    return stream;
}

std::istream& operator >> (std::istream& stream, aries::DataNode*& node)
{
    node = aries::Node::readDocument(stream);
    return stream;
}
