#pragma once

#include <string>
#include <vector>

namespace Util
{

class FileSystem
{
public:
    static std::string getFullFilePath(const std::string& filename);
    static std::string getWorkingDirectory();
    static bool changeWorkingDirectory(const std::string& dir);

    static std::vector<std::string> findFiles(const std::string& wildcard, bool recursive = false);

    static bool isFile(const std::string& filename);
    static bool isFolder(const std::string& folder);

    static bool copyFile(const std::string& file, const std::string& to);
    static bool createFolder(const std::string& file, bool recurseUpwards = false);
    static bool deleteFile(const std::string& file);

    static std::string getUserDir();
    static std::string getApplicationDir(const std::string& appname, const std::string& orgname = "");
};

}
