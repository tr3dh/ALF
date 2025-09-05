#include "__StringProcessing.h"

namespace string{

    bool contains(const std::string& str, const std::string& subStr){
        return str.find(subStr) != std::string::npos;
    }

    // Findet das erste Vorkommen eines Substrings in einem String
    std::string::size_type findFirst(std::string_view str, std::string_view subStr) {
        return str.find(subStr);
    }

    // Findet das letzte Vorkommen eines Substrings in einem String
    std::string::size_type findLast(std::string_view str, std::string_view subStr) {
        return str.rfind(subStr);
    }

    // // C++23
    // void trim(std::string& str, const char token) {
    //     auto start = std::ranges::find_if_not(str, [&token](unsigned char c) {
    //         return c == token;
    //     });
    //     auto end = std::ranges::find_if_not(str | std::views::reverse, [&token](unsigned char c) {
    //         return c == token;
    //     }).base();
    //     str = (start < end) ? std::string(start, end) : "";
    // }

    void trim(std::string& str, const char token) {
        // finde erstes Zeichen, das nicht 'token' ist
        auto startIt = std::find_if_not(str.begin(), str.end(),
                                        [&token](unsigned char c) { return c == token; });

        // finde letztes Zeichen, das nicht 'token' ist
        auto endIt = std::find_if_not(str.rbegin(), str.rend(),
                                      [&token](unsigned char c) { return c == token; }).base();

        str = (startIt < endIt) ? std::string(startIt, endIt) : "";
    }

    void trimVec(std::vector<std::string>& strVec, const char& token){
        
        // elementweise trimmen
        for(auto& str : strVec){
            trim(str,token);
        }
    }

    // // C++23
    // std::vector<std::string> split(const std::string& str, const char& token){
        
    //     //
    //     return str | std::views::split(token) | std::ranges::to<std::vector<std::string>>();
    // }

    std::vector<std::string> split(const std::string& str, const char& token) {
        std::vector<std::string> result;
        std::string current;

        for (char c : str) {
            if (c == token) {
                if (!current.empty()) result.push_back(current);
                current.clear();
            } else {
                current += c;
            }
        }

        if (!current.empty()) result.push_back(current);
        return result;
    }

    bool endsWith(const std::string& str, const std::string& seq){

        //
        return findLast(str, seq) == str.length() - seq.length();
    }

    bool startsWith(const std::string& str, const std::string& seq){

        //
        return findFirst(str, seq) == 0;
    }

    //
    void fullStrip(std::string& str, const char token) {
        str.erase(std::remove(str.begin(), str.end(), token),str.end());
    }

    std::string relPath(const std::string& str){
        return std::filesystem::relative(str, std::filesystem::current_path()).string();
    }
}