#include "WinCmd.h"

void mkdir(const std::string& path){
    
    // Prüfen, ob das Verzeichnis existiert
    if (fs::exists(path)) {
        
        // Löscht das Verzeichnis
        return;
    }

    // Verzeichnis neu erstellen
    if (fs::create_directory(path)) {
        
    } else {
        ASSERT(TRIGGER_ASSERT, "mkdir fehlgeschlagen");
    }
}

int runWinCommand(const std::string& cmd, const std::string& cwd) {

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = 0; // optional, CREATE_NO_WINDOW reicht
    
    BOOL success = CreateProcessA(
        NULL,
        (LPSTR)cmd.c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        (cwd == "" ? NULL : cwd.c_str()),
        &si,
        &pi
    );

    if (!success) return -1;

    // Optional: warten bis der Prozess fertig ist
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if(exitCode != 0){
        _ERROR << "Command : " << cmd << " abgebrochen mit exitCode : " << exitCode << ENDL;
    }

    return static_cast<int>(exitCode & 0xFFFF);
}

int streamWinCommand(const std::string& cmd, std::function<void(const char*)> callback, const std::string& cwd) {

    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        std::cerr << "Pipe creation failed\n";
        return -1;
    }

    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;
    si.wShowWindow = 0;  // Fenster verstecken

    BOOL success = CreateProcessA(
        NULL,
        (LPSTR)cmd.c_str(),
        NULL,
        NULL,
        TRUE,  // Wichtig: HANDLEs werden vererbt
        CREATE_NO_WINDOW,
        NULL,
        (cwd.empty() ? NULL : cwd.c_str()),
        &si,
        &pi
    );

    CloseHandle(hWrite);  // Parent schließt das Schreib-Ende

    if (!success) return -1;

    // Ausgabe live lesen
    char buffer[4096];
    DWORD bytesRead;
    while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {

        buffer[bytesRead] = '\0';

        callback(buffer);
    }

    CloseHandle(hRead);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return static_cast<int>(exitCode & 0xFFFF);
}