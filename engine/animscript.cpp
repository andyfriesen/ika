
#include <sstream>
#include "animscript.h"
#include "common/log.h"

AnimScript::AnimScript()
    : _offset(0)
    , _count(0)
    , _curFrame(0)
    , _dead(true)
{}

AnimScript::AnimScript(const std::string& script)
    : _offset(0)
    , _count(0)
    , _curFrame(0)
    , _dead(false)
{
    uint index = 0;

    while (index < script.length())
    {
        char c = script[index++];

        // eat whitespace
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
            continue;

        // eat more whitespace
        while (
            index < script.length() && (
            script[index] == ' '  || 
            script[index] == '\n' || 
            script[index] == '\r' || 
            script[index] == '\t')
            )
        {
            index++;
        }

        // get a number
        uint end = index;
        while (end < script.length() && script[end] >= '0' && script[end] <= '9')
            end++;

        // force c to uppercase
        if (c >= 'a' && c <= 'z')
            c ^= 32;

        commands.push_back(
            Command(
                c, 
                atoi(script.substr(index, end - index).c_str())
            )
        );

        index = end;
    }
}

const AnimScript::Command& AnimScript::getCurrent() const
{
    static Command dummy;

    if (_offset > commands.size())
        return dummy;
    else
        return commands[_offset];
}

void AnimScript::update(uint time)
{
    if (commands.empty() || _dead)
        return;

    _count -= time;

    // used to make sure we don't loop around and around forever if the animation has no waits
    int startOffset = _offset;

    while (_count < 0)
    {
        const Command& cmd = getCurrent();
        int s = commands.size();
        _offset = (_offset + 1) % commands.size();

        switch (cmd.type)
        {
        case 'F':
            _curFrame = cmd.amount;
            break;

        case 'W':
            _count += cmd.amount;
            break;

        case 'E':
            _dead = true;
            return;

        default:
            Log::Write("In animation script '%s': Unrecognized command '%c'!", toString().c_str(), cmd.type);
            break;
        }

        if (_offset == startOffset)
            return;
    }
}

std::string AnimScript::toString() const
{
    std::stringstream ss;

    for (uint i = 0; i < commands.size(); i++)
        ss << commands[i].type << commands[i].amount << ' ';

    return ss.str();
}