#pragma once

#include <imgui/imgui.h>
#include <imgui-filebrowser/imfilebrowser.h>
#include <raylib.h>

#include <functional>

//
static std::unique_ptr<ImGui::FileBrowser> g_fileDialog;
static std::function<void(const std::string&)> g_filebrowserCallback;

void OpenFileDialog(const char* title, const std::vector<std::string>& filters, bool isSaveDialog, bool selectDirs,
                    const std::string& pwdPath, std::function<void(const std::string&)> callback);

void RenderFileDialog();
void HandleFileDialog();