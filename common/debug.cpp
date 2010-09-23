#include "debug.h"

string int2string(const int& number) {
   ostringstream oss;
   oss << number;
   return oss.str();
}
