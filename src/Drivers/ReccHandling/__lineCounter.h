#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int countLinesInFile(const fs::path& filePath);

int countLinesInDirectory(const fs::path& dirPath);

void searchFile(const fs::path& directory, const std::string& filename);