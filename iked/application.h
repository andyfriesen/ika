
#pragma once

#include <wx/wx.h>

namespace iked {

    struct Application : public wxApp {
    public:
	virtual bool OnInit();
    };

}
