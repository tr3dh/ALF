#pragma once

#include <ranges>
#include <string>
#include <string_view>
#include <charconv>
#include <vector>

namespace string{

    //
    bool contains(const std::string& str, const std::string& subStr);

    // Findet das erste Vorkommen eines Substrings in einem String
    std::string::size_type findFirst(std::string_view str, std::string_view subStr);

    // Findet das letzte Vorkommen eines Substrings in einem String
    std::string::size_type findLast(std::string_view str, std::string_view subStr);

    std::string trim(const std::string& str, const char& token = ' ');

    template<typename T>
    T convert(const std::string& str);

    std::vector<std::string> split(const std::string& str, const char& token = ' ');
}
