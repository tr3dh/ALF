#include "__lineCounter.h"

int countLinesInFile(const fs::path& filePath) {

    std::ifstream file(filePath);
    return std::count(std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(), '\n');
}

int countLinesInDirectory(const fs::path& dirPath) {

    if (!fs::exists(dirPath)) {
        return -1;
    }

    int totalLines = 0;
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            if (extension == ".h" || extension == ".cpp" || extension == ".tpp") {
                totalLines += countLinesInFile(entry.path());
            }
        }
    }

    return totalLines;
}

void searchFile(const fs::path& directory, const std::string& filename) {

    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (fs::is_regular_file(entry) && entry.path().filename() == filename) {
                std::cout << "File found: " << entry.path() << "\n";
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << "\n";
    }
}