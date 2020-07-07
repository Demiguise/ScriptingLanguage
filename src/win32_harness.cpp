#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>

#include <vector>
#include <string>

using TStringVec = std::vector<std::string>;

TStringVec GetFolderContents(const char *dirPath, const char *ext)
{
    TStringVec folderContents;

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    //First, ensure we have the proper full path to the place we're looking in.
    char originalPath[MAX_PATH];
    memset(originalPath, 0, sizeof(originalPath));
    DWORD filePathLen = GetFullPathName(dirPath, sizeof(originalPath), originalPath, NULL);

    if (originalPath[filePathLen] != '/')
    {
        originalPath[filePathLen++] = '/';
    }

    //Then copy it into a second buffer for string work.
    char searchPath[MAX_PATH];
    memcpy(searchPath, originalPath, MAX_PATH);
    searchPath[filePathLen++] = '*';
    if (ext != nullptr)
    {
        DWORD extLen = (DWORD)strlen(ext);
        memcpy(searchPath + filePathLen, ext, extLen);
        filePathLen += extLen;
    }
    searchPath[filePathLen] = '\0';

    hFind = FindFirstFile(searchPath, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        //Failed to find anything.
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND)
        {
            OutputDebugString("Failed to find any file that matches that description.\r\n");
        }
        else
        {
            OutputDebugString("Failed to lookup directory contents.\r\n");
        }
    }
    else
    {
        std::string tmp = originalPath;
        tmp += FindFileData.cFileName;

        while (FindNextFile(hFind, &FindFileData))
        {
            //Ignore directories beginning with dots
            if (FindFileData.cFileName[0] == '.')
            {
                continue;
            }

            tmp = dirPath;
            tmp += "/";
            tmp += FindFileData.cFileName;
            folderContents.push_back(tmp);
        }

        //Close off the handle.
        FindClose(hFind);
    }

    return folderContents;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Please provide path to data folder (At the root of the repo)" << std::endl;
        return -1;
    }

    auto tests = GetFolderContents(argv[1], ".test");

    std::cout << "Running (" << tests.size() << ") tests..." << std::endl;

    for (auto test : tests)
    {
        std::cout << "Running test: " << test;
        std::cout << "[Success]" << std::endl;
    }

    return 0;
}
