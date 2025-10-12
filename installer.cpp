#include <iostream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <fstream>

const std::string INSTALL_DIR = "C:\\Program Files\\stgh";
const std::string EXE_NAME = "stgh.exe";
const std::string INSTALL_PATH = INSTALL_DIR + "\\" + EXE_NAME;

bool isInstalled() {
    std::ifstream file(INSTALL_PATH);
    return file.good();
}

void createDirectory() {
    if (CreateDirectoryA(INSTALL_DIR.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        // Successfully created the directory or it already exists.
    } else {
        std::cerr << "Failed to create directory: " << INSTALL_DIR << ". Error: " << GetLastError() << std::endl;
        exit(1);
    }
}

void copyFile() {
    if (CopyFileA(EXE_NAME.c_str(), INSTALL_PATH.c_str(), FALSE)) {
        std::cout << "File copied successfully." << std::endl;
    } else {
        std::cerr << "Failed to copy file. Make sure " << EXE_NAME << " is in the same directory as the installer. Error: " << GetLastError() << std::endl;
        exit(1);
    }
}

std::string getPath() {
    HKEY hKey;
    LONG lResult = RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ, &hKey);
    if (lResult != ERROR_SUCCESS) {
        return ""; // Key might not exist, which is fine.
    }

    char szBuffer[4096];
    DWORD dwBufferSize = sizeof(szBuffer);
    lResult = RegQueryValueExA(hKey, "Path", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
    RegCloseKey(hKey);

    if (lResult != ERROR_SUCCESS) {
        return ""; // Path value might not exist.
    }

    return std::string(szBuffer);
}

void setPath(const std::string& newPath) {
    HKEY hKey;
    LONG lResult = RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_WRITE, &hKey);
    if (lResult != ERROR_SUCCESS) {
        std::cerr << "Failed to open registry key for writing." << std::endl;
        exit(1);
    }

    lResult = RegSetValueExA(hKey, "Path", 0, REG_EXPAND_SZ, (const BYTE*)newPath.c_str(), newPath.length() + 1);
    RegCloseKey(hKey);

    if (lResult != ERROR_SUCCESS) {
        std::cerr << "Failed to set registry value." << std::endl;
        exit(1);
    }

    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Environment", SMTO_ABORTIFHUNG, 5000, NULL);
}

void addToPath() {
    std::string currentPath = getPath();
    if (currentPath.find(INSTALL_DIR) == std::string::npos) {
        std::string newPath = currentPath;
        if (!newPath.empty() && newPath.back() != ';') {
            newPath += ";";
        }
        newPath += INSTALL_DIR;
        setPath(newPath);
        std::cout << "Added to PATH." << std::endl;
    } else {
        std::cout << "Already in PATH." << std::endl;
    }
}

void removeFromPath() {
    std::string currentPath = getPath();
    size_t pos = currentPath.find(INSTALL_DIR);
    if (pos != std::string::npos) {
        std::string newPath = currentPath;
        newPath.erase(pos, INSTALL_DIR.length());

        if (!newPath.empty() && newPath.front() == ';') {
            newPath.erase(0, 1);
        }
        if (!newPath.empty() && newPath.back() == ';') {
            newPath.pop_back();
        }

        // Handling consecutive semicolons
        size_t semiPos = newPath.find(";;");
        while(semiPos != std::string::npos){
            newPath.replace(semiPos, 2, ";");
            semiPos = newPath.find(";;");
        }

        setPath(newPath);
        std::cout << "Removed from PATH." << std::endl;
    } else {
        std::cout << "Not found in PATH." << std::endl;
    }
}

void install() {
    if (isInstalled()) {
        std::cout << "stgh is already installed." << std::endl;
        return;
    }

    createDirectory();
    copyFile();
    addToPath();

    std::cout << "stgh has been installed successfully." << std::endl;
}

void uninstall() {
    if (!isInstalled()) {
        std::cout << "stgh is not installed." << std::endl;
        return;
    }

    if (remove(INSTALL_PATH.c_str()) != 0) {
        std::cerr << "Failed to delete " << INSTALL_PATH << ". Error: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Deleted " << INSTALL_PATH << std::endl;
    }

    if (RemoveDirectoryA(INSTALL_DIR.c_str())) {
        std::cout << "Removed directory " << INSTALL_DIR << std::endl;
    } else {
        std::cerr << "Failed to remove directory " << INSTALL_DIR << ". It might not be empty. Error: " << GetLastError() << std::endl;
    }

    removeFromPath();

    std::cout << "stgh has been uninstalled." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--uninstall") {
        uninstall();
    } else {
        install();
    }
    return 0;
}
