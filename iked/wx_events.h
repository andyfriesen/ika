
#pragma once

/**
 * Custom wxWindow event types.  Used for inter-control interaction.
 */
namespace iked {
    enum {
        first = wxEVT_USER_FIRST,

        // raised by ImageArrayPanel
        EVT_IMAGE_SELECT,
        EVT_IMAGE_RIGHT_CLICK,
        EVT_IMAGE_DOUBLE_CLICK,
    };
}

