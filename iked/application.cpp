
#include <wx/xrc/xmlres.h>

#include "common/version.h"
#include "application.h"
#include "main.h"

IMPLEMENT_APP(iked::Application)

namespace iked {

    struct MainWindow;

    bool Application::OnInit() {
        extern void InitXmlResource(); // resource.cpp

        wxXmlResource::Get()->InitAllHandlers();
#if 0 && defined(DEBUG)
        wxXmlResource::Get()->Load("resource.xrc");
#else
        InitXmlResource();
#endif

        std::vector<std::string> args;
        for (size_t i = 0; i < argc; i++) {
            args.push_back(argv[i]);
        }

        Log::Init("iked.log");

        MainWindow* mainwnd = new MainWindow(args);
        mainwnd->Show(TRUE);
        SetTopWindow(mainwnd);

        return true;
    }

}
