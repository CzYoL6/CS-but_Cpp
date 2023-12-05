//
// Created by zaiyichen on 2023/12/5.
//

#ifndef CPP_BUT_WINDOWSFILEDIALOG_H
#define CPP_BUT_WINDOWSFILEDIALOG_H
#include <windows.h>
#include <string>
#include <shlobj.h>
#include <memory>
#include <filesystem>

namespace FileDialog{
    namespace fs = std::filesystem;
    inline std::filesystem::path getExecDirAbsolutePath(){
        TCHAR exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);
        auto exec_dir = fs::canonical(fs::path(exePath)).parent_path();
        return exec_dir;
    }
    inline std::filesystem::path getCanonicalPath(std::filesystem::path path){
        fs::current_path(getExecDirAbsolutePath());
        return fs::canonical(path);
    }
    inline void openFileDialog(std::string& ret, const char* filter = "All Files\0*.*\0") {
        OPENFILENAME ofn;
        char szFile[MAX_PATH] = {0};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn) == TRUE) {
            ret= fs::absolute(szFile).string();
        }


    }

    inline void openFolderDialog(std::string& ret, const char* title = "Pick a Folder") {
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        BROWSEINFOA bi = { 0 };
        bi.lpszTitle = title;
        LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

        std::string folderPath;

        if (pidl != NULL) {
            char path[MAX_PATH];
            if (SHGetPathFromIDListA(pidl, path)) {
                folderPath = path;
            }

            IMalloc* imalloc = NULL;
            if (SUCCEEDED(SHGetMalloc(&imalloc))) {
                imalloc->Free(pidl);
                imalloc->Release();
            }
            ret= fs::absolute(folderPath).string();
        }

        CoUninitialize();

    }
};




#endif //CPP_BUT_WINDOWSFILEDIALOG_H
