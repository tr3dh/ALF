#include "defines.h"

//
template uint8_t string::convert<uint8_t>(const std::string&);
template uint16_t string::convert<uint16_t>(const std::string&);
template uint32_t string::convert<uint32_t>(const std::string&);
template uint64_t string::convert<uint64_t>(const std::string&);

//
template int string::convert<int>(const std::string&);
template float string::convert<float>(const std::string&);
template double string::convert<double>(const std::string&);
template long double string::convert<long double>(const std::string&);