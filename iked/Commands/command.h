
#pragma once

namespace iked { namespace commands {

    struct CommandContext {
    };

    struct Command {
        virtual ~Command() {}

        virtual void Do(CommandContext* ctx) = 0;
        virtual void Undo(CommandContext* ctx) = 0;
    };

}}
