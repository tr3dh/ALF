#include "defines.h"

//
extern template uint8_t string::convert<uint8_t>(const std::string&);
extern template uint16_t string::convert<uint16_t>(const std::string&);
extern template uint32_t string::convert<uint32_t>(const std::string&);
extern template uint64_t string::convert<uint64_t>(const std::string&);

//
extern template int string::convert<int>(const std::string&);
extern template float string::convert<float>(const std::string&);
extern template double string::convert<double>(const std::string&);
extern template long double string::convert<long double>(const std::string&);

//
extern template size_t string::convert<size_t>(const std::string&);