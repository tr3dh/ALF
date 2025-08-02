#pragma once

#include "defines.h"

template bool InputSliderInt<int>(const std::string&, int&, const int&, const int&, bool, const std::string&);
template bool InputSliderInt<size_t>(const std::string&, size_t&, const int&, const int&, bool, const std::string&);
template bool InputSliderInt<uint8_t>(const std::string&, uint8_t&, const int&, const int&, bool, const std::string&);
template bool InputSliderInt<uint16_t>(const std::string&, uint16_t&, const int&, const int&, bool, const std::string&);
template bool InputSliderInt<uint32_t>(const std::string&, uint32_t&, const int&, const int&, bool, const std::string&);