#include "__fileBrowser.h"

void OpenFileDialog(const char* title, const std::vector<std::string>& filters, bool isSaveDialog, bool selectDirs, const std::string& pwdPath, std::function<void(const std::string&)> callback) {

    ImGuiFileBrowserFlags flags = ImGuiFileBrowserFlags_ConfirmOnEnter | ImGuiFileBrowserFlags_CloseOnEsc;

    if (isSaveDialog) {
        flags |= ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir;
    }

    if(selectDirs){
        flags |= ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_HideRegularFiles;
    }

    g_fileDialog = std::make_unique<ImGui::FileBrowser>(flags);

    g_fileDialog->SetTitle(title);
    g_fileDialog->SetTypeFilters(filters);
    g_fileDialog->SetPwd(pwdPath);
    g_fileDialog->Open();

    g_filebrowserCallback = callback;
}

void RenderFileDialog(){

    // rendere falls konstruiert
    if(g_fileDialog){
        g_fileDialog->Display();
    }
}

void HandleFileDialog(){

    //
    if (g_fileDialog && g_fileDialog->HasSelected())
    {

        const std::string chosenFilePath = g_fileDialog->GetSelected().string();

        //
        g_filebrowserCallback(chosenFilePath);

        // Dekonstruktion des filebrowsers über reset des unique pointers
        g_fileDialog.reset();
    }
}