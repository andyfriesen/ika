#ifndef ANIMSCRIPT_H
#define ANIMSCRIPT_H

#include <vector>
#include <string>
#include "common/utility.h"

struct AnimScript {
    struct Command {
        char type;
        int amount;

        Command() : type(0), amount(0)
        {}

        Command(char t, int a)
            : type(t)
            , amount(a)
        {}
    };

    AnimScript();
    explicit AnimScript(const std::string& script);

    const Command& getCurrent() const;
    inline uint getCurFrame() const { return _curFrame; }
    inline bool isDead() const { return _dead; }

    void update(int time);

    std::string toString() const;

    inline bool isEmpty() const { return commands.empty(); }

private:
    uint _offset;
    int  _count;
    uint _curFrame;
    bool _dead;

    std::vector<Command> commands;
};

#endif
