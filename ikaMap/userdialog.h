#ifndef USERDIALOG_H
#define USERDIALOG_H

#include "wxinc.h"
#include <vector>
#include <string>

struct UserDialog : public wxDialog
{
    typedef std::pair<std::string, std::string> Arg;
    typedef std::vector<Arg> ArgList;

    UserDialog(int cols, char* caption, const ArgList& args);

    ArgList& GetResults();

    virtual int ShowModal();

private:
    ArgList _args;
    ArgList _results;
};

#endif