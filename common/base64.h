#include <string>
#include "types.h"

namespace base64
{
    //void encode(std::vector<u8>& instream, std::vector<u8>& outstream);
    //void decode(std::vector<u8>& instream, std::vector<u8>& outstream);
    std::string encode(u8* instream, uint size);
    int decode(const std::string& instream, u8* outstream, uint size);
}