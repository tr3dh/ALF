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

    void trim(std::string& str, const char token = ' ');
    void trimVec(std::vector<std::string>& strings, const char& token = ' ');

    template<typename T>
    T convert(const std::string& str) {

        if constexpr (std::is_same_v<T, long double>) {
            return std::stold(str);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(str);
        } else if constexpr (std::is_same_v<T, float>) {
            return std::stof(str);
        } else {

            // die Lösung ist leider weniger präzise
            T result;
            auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

            if (ec != std::errc()) throw std::invalid_argument("Invalid number");
            return result;
        }
    }

    std::vector<std::string> split(const std::string& str, const char& token = ' ');

    bool endsWith(const std::string& str, const std::string& seq);
}
