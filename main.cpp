#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void RemoveReadOnly(LPCWSTR filePath)
{
    DWORD fileAttributes = GetFileAttributesW(filePath);

    if ((fileAttributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
    {
        fileAttributes ^= FILE_ATTRIBUTE_READONLY;
        SetFileAttributesW(filePath, fileAttributes);
    }
}

bool IsWine()
{
    HMODULE ntdllHandle = LoadLibraryA("ntdll.dll");
    return GetProcAddress(ntdllHandle, "wine_get_version") != nullptr;
}

int wmain(int argc, wchar_t *argv[])
{
    if (argc < 5) {
        puts("Usage: dircpy.exe source-path dest-path move skip-fl-exe");
        return 0;
    }

    LPCWSTR sourcePath = argv[1];
    LPCWSTR destPath = argv[2];
    bool move = _wtoi(argv[3]);
    bool skipFlExe = _wtoi(argv[4]);
    bool isWine = IsWine();

    try {
        using recursive_directory_iterator = fs::recursive_directory_iterator;
        for (const auto& dirEntry : recursive_directory_iterator(sourcePath))
        {
            if (!dirEntry.is_directory()) {
                auto relativeFile = fs::relative(fs::path(dirEntry), sourcePath);

                if (skipFlExe && _wcsicmp(relativeFile.filename().c_str(), L"Freelancer.exe") == 0)
                    continue;

                auto destFile = destPath / relativeFile;
                auto destFolder = destFile.parent_path();

                if (!fs::is_directory(destFolder)) {
                    fs::create_directories(destFolder);
                }

                fs::copy(dirEntry, destFile, fs::copy_options::overwrite_existing);

                if (move)
                    fs::remove(dirEntry);

                if (!isWine)
                    RemoveReadOnly(destFile.c_str());
            }
        }

        if (move)
            fs::remove_all(sourcePath);

    } catch(...) {
        return 1;
    }

    return 0;
}
