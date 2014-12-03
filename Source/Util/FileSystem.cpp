#include "FileSystem.hpp"
#include <Defines.hpp>

#include <nowide/convert.hpp>
#include <nowide/cstdio.hpp>
#include <nowide/fstream.hpp>

using Util::FileSystem;

namespace
{
#ifdef LD31_WINDOWS
    typedef wchar_t wildchar_type;
#else
    typedef char wildchar_type;
#endif

    bool wildcmp(const wildchar_type* wild, const wildchar_type* string)
    {
        while ((*string) && (*wild != '*'))
        {
            if ((*wild != *string) && (*wild != '?'))
                return false;

            wild++;
            string++;
        }

        const wildchar_type* cp = NULL, *mp = NULL;

        while (*string)
        {
            if (*wild == '*')
            {
                if (!*++wild)
                    return true;

                mp = wild;
                cp = string + 1;
            }
            else if ((*wild == *string) || (*wild == '?'))
            {
                wild++;
                string++;
            }
            else
            {
                wild = mp;
                string = cp++;
            }
        }

        while (*wild == '*')
            wild++;

        return !*wild;
    }
}

#if defined LD31_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <direct.h>
#include <shlobj.h>
#else
#include <cstdlib>
#include <dirent.h>
#include <libgen.h>
#include <pwd.h>
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

std::string FileSystem::getBasename(const std::string& filename)
{
#ifdef LD31_WINDOWS
    size_t find = filename.find_last_of('\\');
    if (find == std::string::npos)
        return filename;

    return filename.substr(find + 1);
#else
    char buf[BUFSIZ];
    strcpy(buf, filename.c_str());
    return basename(buf);
#endif
}

std::string FileSystem::getDirname(const std::string& filename)
{
#ifdef LD31_WINDOWS
    size_t find = filename.find_last_of('\\');
    if (find == std::string::npos)
        return filename;

    return filename.substr(0, find);
#else
    char buf[BUFSIZ];
    strcpy(buf, filename.c_str());
    return dirname(buf);
#endif
}

std::string FileSystem::getFullFilePath(const std::string& filename)
{
#ifdef LD31_WINDOWS
    std::wstring fname = nowide::widen(filename);

    wchar_t filenameBuf[BUFSIZ];
    wchar_t** lastPart = nullptr;
    int ret = GetFullPathNameW(fname.c_str(), BUFSIZ, filenameBuf, lastPart);
    if (ret == 0)
    {
        errno = EINVAL;
        return "";
    }

    return nowide::narrow(filenameBuf);
#else
    char buf[BUFSIZ];
    char* real = realpath(filename.c_str(), buf);
    if (real)
        return real;
    return "";
#endif
}

std::string FileSystem::getWorkingDirectory()
{
#ifdef LD31_WINDOWS
    wchar_t buf[BUFSIZ];
    _wgetcwd(buf, BUFSIZ);

    return std::string(nowide::narrow(buf));
#else
    char buf[BUFSIZ];
    getcwd(buf, BUFSIZ);
    return std::string(buf);
#endif    
}
bool FileSystem::changeWorkingDirectory(const std::string& dir)
{
#ifdef LD31_WINDOWS
    return _wchdir(nowide::widen(dir).c_str()) == 0;
#else
    return chdir(dir.c_str()) == 0;
#endif
}

std::vector<std::string> FileSystem::findFiles(const std::string& wildcard, bool recursive)
{
    std::vector<std::string> files;

#ifdef LD31_WINDOWS
    std::wstring wcard = nowide::widen(wildcard);
    std::wstring wpath;

    size_t pos = wcard.find_last_of('\\');
    if (pos == std::wstring::npos)
        wpath = L".";
    else
    {
        wpath = wcard.substr(0, pos);
        wcard.erase(0, pos + 1);
    }

    WIN32_FIND_DATAW FindFileData;
    HANDLE hFind;
    unsigned int iFiles = 0;
    hFind = FindFirstFileW((wpath + L"\\*").c_str(), &FindFileData);

    while (hFind != INVALID_HANDLE_VALUE)
    {
        wchar_t* fname = FindFileData.cFileName;

        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && recursive && wcscmp(fname, L".") != 0 && wcscmp(fname, L"..") != 0)
        {
            auto contents = findFiles(nowide::narrow(wpath) + "\\" + nowide::narrow(fname) + "\\" + nowide::narrow(wcard), recursive);
            std::copy(contents.begin(), contents.end(), std::back_inserter(files));
        }
        else if (wildcmp(wcard.c_str(), fname))
            files.push_back(nowide::narrow(wpath) + "\\" + nowide::narrow(fname));

        if (!FindNextFileW(hFind, &FindFileData))
            break;
    }

    FindClose(hFind);
#else
    std::string dirName = getDirname(wildcard);
    std::string findName = getBasename(wildcard);

    DIR* dp;
    dirent* dirp;
    if ((dp = opendir(dirName.c_str())) == NULL)
    {
        return files;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        std::string name = dirp->d_name;
        std::string fullName = dirName + "/" + name;
        
        if (isFolder(fullName) && recursive && (name != "." && name != ".."))
        {
            auto contents = findFiles(fullName + "/" + findName, recursive);
            std::copy(contents.begin(), contents.end(), std::back_inserter(files));
        }
        else if (wildcmp(findName.c_str(), name.c_str()))
        {
            files.push_back(fullName);
        }
    }

    closedir(dp);
#endif

    return files;
}

bool FileSystem::isFile(const std::string& file)
{ 
#ifdef LD31_WINDOWS
    struct _stat32 fileStat;
    int err = _wstat32(nowide::widen(file).c_str(), &fileStat);
#else
    struct stat fileStat;
    int err = stat(file.c_str(), &fileStat);
#endif
    if (err != 0)
        return false;

    return (fileStat.st_mode & S_IFMT) == S_IFREG;
}
bool FileSystem::isFolder(const std::string& folder)
{
#ifdef LD31_WINDOWS
    struct _stat32 fileStat;
    int err = _wstat32(nowide::widen(folder).c_str(), &fileStat);
#else
    struct stat fileStat;
    int err = stat(folder.c_str(), &fileStat);
#endif
    if (err != 0)
        return false;

    return (fileStat.st_mode & S_IFMT) == S_IFDIR;
}

bool FileSystem::copyFile(const std::string& file, const std::string& to)
{
    nowide::ifstream ifs(file.c_str(), std::ios::binary);
    nowide::ofstream ofs(to.c_str(), std::ios::binary | std::ios::trunc);

    if (!ifs || !ofs)
        return false;

    ofs << ifs.rdbuf();

    return (ofs && ifs);
}
bool FileSystem::createFolder(const std::string& folder, bool recurse)
{
    std::string fixed = folder;
#ifdef LD31_WINDOWS
    const char appendix = '\\';
#else
    const char appendix = '/';
#endif

    if (fixed.back() == appendix)
        fixed.erase(fixed.size() - 1);

    if (fixed.empty())
        return false;

    if (isFolder(fixed))
        return true;

    if (recurse)
    {
        std::string upOne = getDirname(fixed);

        createFolder(upOne, true);
    }

#ifdef LD31_WINDOWS
    return _wmkdir(nowide::widen(folder).c_str()) == 0;
#else
    return mkdir(fixed.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
#endif
}
bool FileSystem::deleteFile(const std::string& file)
{
    return nowide::remove(file.c_str()) == 0;
}

std::string FileSystem::getTempFile(const std::string& ext)
{
    char buf[L_tmpnam];
#ifdef LD31_WINDOWS
    if (tmpnam_s(buf) != 0)
        return "";
#else
    tmpnam(buf);
#endif
    size_t len = strlen(buf);
    if (buf[len - 1] == '.')
        buf[len - 1] = 0;

    return getTempDir() + buf + ext;
}

std::string FileSystem::getTempDir()
{
#ifdef LD31_WINDOWS
    static std::string TempDir;
    if (!TempDir.empty())
        return TempDir;

    wchar_t buf[BUFSIZ];
    GetTempPathW(BUFSIZ, buf);
    std::string temp = nowide::narrow(buf);
    if (temp.back() == '\\')
        temp.erase(temp.size() - 1);

    TempDir = temp;

    return temp;
#else
    return "/tmp";
#endif
}

std::string FileSystem::getUserDir()
{
    static std::string UserDir;
    if (!UserDir.empty())
        return UserDir;

#ifdef LD31_WINDOWS
    typedef BOOL(WINAPI *fnGetUserProfDirW)(HANDLE, LPWSTR, LPDWORD);
    fnGetUserProfDirW pGetDir = nullptr;
    HMODULE lib = nullptr;
    HANDLE accessToken = nullptr;

    lib = LoadLibraryA("userenv.dll");
    if (!lib)
        return "";

    pGetDir = (fnGetUserProfDirW)GetProcAddress(lib, "GetUserProfileDirectoryW");
    if (!pGetDir)
    {
        FreeLibrary(lib);
        return "";
    }

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &accessToken))
    {
        FreeLibrary(lib);
        return "";
    }
    
    DWORD psize = 0;
    WCHAR dummy = 0;
    LPWSTR wstr = NULL;
    BOOL rc = 0;

    rc = pGetDir(accessToken, &dummy, &psize);
    std::vector<wchar_t> storage(psize + 1);
    wstr = &storage[0];

    rc = pGetDir(accessToken, wstr, &psize);
    if (wstr[psize - 2] != '\\')
    {
        wstr[psize - 1] = '\\';
        wstr[psize - 0] = 0;
    }

    CloseHandle(accessToken);
    FreeLibrary(lib);

    UserDir = nowide::narrow(wstr);
#else
    char *envr = getenv("HOME");
    if (envr && isFolder(envr))
    {
        size_t len = strlen(envr);
        const size_t add_dirsep = (envr[len - 1] != '/') ? 1 : 0;

        std::vector<char> storage(len + add_dirsep);
        strcpy(&storage[0], envr);

        if (add_dirsep)
            storage[len] = '/';

        UserDir = std::string(&storage[0], len + add_dirsep);
    }
    else
    {
        uid_t uid = getuid();
        struct passwd *pw;

        pw = getpwuid(uid);
        if ((pw != NULL) && (pw->pw_dir != NULL) && (*pw->pw_dir != '\0'))
        {
            const size_t dlen = strlen(pw->pw_dir);
            const size_t add_dirsep = (pw->pw_dir[dlen - 1] != '/') ? 1 : 0;

            std::vector<char> storage(dlen + add_dirsep);
            strcpy(&storage[0], pw->pw_dir);

            if (add_dirsep)
                storage[dlen] = '/';

            UserDir = std::string(&storage[0], dlen + add_dirsep);
        }
    }
#endif

    return UserDir;
}

std::string FileSystem::getApplicationDir(const std::string& appname, const std::string& orgname)
{
    static std::string BaseAppDir;
    if (!BaseAppDir.empty())
#ifdef LD31_WINDOWS
        return BaseAppDir + "\\" + (orgname.empty() ? appname : orgname + "\\" + appname) + "\\";

    WCHAR path[MAX_PATH];
    size_t len = 0;

    if (SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path) != S_OK)
        return "";

    BaseAppDir = nowide::narrow(path);

    std::string appDir = BaseAppDir;
    if (!orgname.empty())
        appDir += "\\" + orgname;
    appDir += "\\" + appname + "\\";

    return appDir;
#else
        return BaseAppDir + "/" + (orgname.empty() ? appname : orgname + "/" + appname) + "/";

#if LD31_MACOSX
    std::string appDir = getUserDir() + "Library/Application Support"
#else
    const char *envr = getenv("XDG_DATA_HOME");
    
    std::string appDir;
    if (!envr)
    {
        // Fall back to $HOME/.local/share/<appname>
        appDir = getUserDir() + ".local/share";
    }
    else
        appDir = std::string(envr);
#endif

    BaseAppDir = appDir;

    if (!orgname.empty())
        appDir += "/" + orgname;
    appDir += "/" + appname + "/";

    return appDir;
#endif
}
