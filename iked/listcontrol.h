#pragma once

#include <string>
#include <wx/wx.h>
#include <wx/listctrl.h>

namespace iked {
    /**
     * Slightly tweaked out wxListCtrl because I wanted a few
     * convenience methods.  Doing these little things is an
     * obscene amount of work considering how basic they are. :P
     */
    struct ListControl : wxListCtrl {
        ListControl(
            wxWindow* parent, 
            wxWindowID id, 
            const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, 
            long style = wxLC_ICON, 
            const wxValidator& validator = wxDefaultValidator, 
            const wxString& name = "ListControl"
        );

        /**
         * Returns the index of the current selection, or -1 if there isn't one.
         */
        int getSelection() const;

        /**
         * Sets the selection to the index specified, or unselects if -1 is given.
         * newSelection must not be outside the range of possible values.
         */
        void setSelection(int newSelection);

        /**
         * Returns the label at the row/column.
         * row and column must be in range or I will cut you.
         */
        std::string getString(int row, int column) const;

    private:
        void onSelectItem(wxListEvent& event);
        void onDeleteItem(wxListEvent& event);
        void onDeleteAllItems(wxListEvent& event);

        int curSelection;

        DECLARE_EVENT_TABLE();
    };
}
