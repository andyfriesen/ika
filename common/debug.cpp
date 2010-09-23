#include "debug.h"

std::string int2string(const int& number) {
   std::ostringstream oss;
   oss << number;
   return oss.str();
}
