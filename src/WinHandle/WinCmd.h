#pragma once

#include "defines.h"

void mkdir(const std::string& path);

int runWinCommand(const std::string& cmd, const std::string& cwd = "");

int streamWinCommand(const std::string& cmd, std::function<void(const char*)> callback, const std::string& cwd = "");