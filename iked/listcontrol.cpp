
#include "listcontrol.h"

namespace iked {

    ListControl::ListControl(
        wxWindow* parent, 
        wxWindowID id, 
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxValidator& validator,
        const wxString& name
    )
        : wxListCtrl(parent, id, pos, size, style, validator, name)
        , curSelection(-1)
    {}

    int ListControl::getSelection() const {
        return curSelection;
    }

    void ListControl::setSelection(int newSelection) {
        wxListItem listItem;
        listItem.m_mask = wxLIST_MASK_STATE;
        listItem.m_itemId = curSelection;
        listItem.m_state = 0;
        SetItem(listItem);
        curSelection = -1;
    }

    std::string ListControl::getString(int row, int column) const {
        wxListItem listItem;
        listItem.m_itemId = row;
        listItem.m_col = column;
        listItem.m_mask = wxLIST_MASK_TEXT;
        GetItem(listItem);

        return listItem.m_text.c_str();
    }

    void ListControl::onSelectItem(wxListEvent& event) {
        curSelection = event.m_itemIndex;
        event.Skip();
    }

    void ListControl::onDeleteItem(wxListEvent& event) {
        const int idx = event.m_itemIndex;
        if (idx == curSelection) {
            curSelection = -1;
        } else if (idx > curSelection) {
            curSelection -= 1;
        }

        event.Skip();
    }

    void ListControl::onDeleteAllItems(wxListEvent& event) {
        curSelection = -1;
        event.Skip();
    }

    BEGIN_EVENT_TABLE(ListControl, wxListCtrl)
        EVT_LIST_ITEM_SELECTED(-1, ListControl::onSelectItem)
        EVT_LIST_DELETE_ITEM(-1, ListControl::onDeleteItem)
        EVT_LIST_DELETE_ALL_ITEMS(-1, ListControl::onDeleteAllItems)
    END_EVENT_TABLE()
}