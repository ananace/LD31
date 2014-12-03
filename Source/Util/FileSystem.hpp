#pragma once

#include <string>
#include <vector>

namespace Util
{

/// Helper functions for accessing the filesystem
class FileSystem
{
public:
    /// Sets the working dir to the location of the current executable
    static void setSaneWorkingDir(const std::string& argv0) { changeWorkingDirectory(getDirname(getFullFilePath(argv0))); }

    /// Gets the basename of the specified path
    static std::string getBasename(const std::string& path);
    /// Gets the dirname of the specified path
    static std::string getDirname(const std::string& path);

    /// Gets the full and absolute path to the specified file
    static std::string getFullFilePath(const std::string& filename);
    /// Gets the current working directory
    static std::string getWorkingDirectory();
    /// Changes the current working directory
    static bool changeWorkingDirectory(const std::string& dir);

    /// Finds all files matching a wildcard, relative to the current working directory
    static std::vector<std::string> findFiles(const std::string& wildcard, bool recursive = false);

    /// Checks if a specified path is a file
    static bool isFile(const std::string& path);
    /// Checks if a specified path is a folder
    static bool isFolder(const std::string& path);

    /// Copies file A to file B, overwriting if it exists
    static bool copyFile(const std::string& file, const std::string& to);
    /// Creates the folder specified for the path, and all its parents if recurseUpwards is set
    static bool createFolder(const std::string& path, bool recurseUpwards = false);
    /// Deletes a file at the specified path
    static bool deleteFile(const std::string& file);

    /// Gets the path to a temporary file
    static std::string getTempFile(const std::string& extension = ".tmp");
    /// Gets the temporary file directory
    static std::string getTempDir();
    /// Gets the user directory of the current user
    static std::string getUserDir();

    /// Gets a sane application directory specific to the current user, for storing data in
    static std::string getApplicationDir(const std::string& appname, const std::string& orgname = "");
};

}
