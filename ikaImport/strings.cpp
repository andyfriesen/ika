
#include <string.h>
#include "Strings.h"

using System::String;

const char* c_str(String* s)
{
    static char c[1024];

    char buffer __gc[]=System::Text::Encoding::ASCII->GetBytes(s);
    char __pin* ptr=&buffer[0];
    memcpy(c,(char*)ptr,s->Length);
    c[s->Length]=0;

    return c;
}

String* net_str(const char* c)
{
    return new String((signed char *)(c),0,strlen(c));
}
