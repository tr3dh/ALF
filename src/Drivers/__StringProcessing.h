#pragma once

#include <ranges>
#include <string>
#include <string_view>
#include <charconv>
#include <vector>

namespace string{

    //
    inline bool contains(const std::string& str, const std::string& subStr){
        return str.find(subStr) != std::string::npos;
    }

    // Findet das erste Vorkommen eines Substrings in einem String
    inline std::string::size_type findFirst(std::string_view str, std::string_view subStr) {
        return str.find(subStr);
    }

    // Findet das letzte Vorkommen eines Substrings in einem String
    inline std::string::size_type findLast(std::string_view str, std::string_view subStr) {
        return str.rfind(subStr);
    }

    inline std::string trim(const std::string& str, const char& token = ' ') {
        auto start = std::ranges::find_if_not(str, [&token](unsigned char c) {
            return c == token;
        });
        auto end = std::ranges::find_if_not(str | std::views::reverse, [&token](unsigned char c) {
            return c == token;
        }).base();
        return (start < end) ? std::string(start, end) : "";
    }

    template<typename T>
    T convert(const std::string& str) {

        T result;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

        if (ec != std::errc()) throw std::invalid_argument("Invalid number");
        return result;
    }

    inline std::vector<std::string> split(const std::string& str, const char& token = ' '){
        
        //
        return str | std::views::split(token) | std::ranges::to<std::vector<std::string>>();
    }
}
