#include <string>
#include "common/utility.h"

/**
 * Old shitty base64 implementation.
 * This is WRONG.  It does not produce correct output.
 * It remains in the source so ika can load files that were made using it.
 */

namespace oldBase64
{
    //void encode(std::vector<u8>& instream, std::vector<u8>& outstream);
    //void decode(std::vector<u8>& instream, std::vector<u8>& outstream);
    std::string encode(u8* instream, uint size);
    int decode(const std::string& instream, u8* outstream, uint size);
}
