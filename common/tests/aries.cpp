#include <iostream>
#include <sstream>

#include "aries.h"


void unittest() {
    aries::DataNode* root = new aries::DataNode("root");
    root//->addChild(aries::StringNode("wee!"))
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
        ->addChild(aries::newNode("empty-child"))
        ->addChild("YOU CAN'T DO THIS ON TV (or can you?!?!?!?!)")
    ;
        //->addChild("FEEL THE BURN");

    std::stringstream ss;
    ss << root;

    std::cout << ss.str() << std::endl;

    aries::DataNode* n = aries::Node::readDocument(ss);

    std::cout << n->getChild("root") << std::endl;

    std::cout << std::endl << "There should be no surrounding quotes in the following string and it should have parentheses." << std::endl;
    std::cout << '\t' << n->getChild("root")->getString() << std::endl;

    delete root;
    delete n;
}

#ifdef DEFINE_MAIN
    int main() {
        unittest();
    }
#endif
