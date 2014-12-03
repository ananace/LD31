#include "FileSystem.hpp"

#include <nowide/fstream.hpp>
#include <nowide/cstdio.hpp>

using Util::FileSystem;

namespace
{
#ifdef _WIN32
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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <direct.h>
#else
#include <pwd.h>
#include <cstdlib>
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

std::string FileSystem::getWorkingDirectory()
{
#ifdef _WIN32
    wchar_t buf[BUFSIZ];
    _wgetcwd(buf, BUFSIZ);

    nowide::stackstring sname;
    if (!sname.convert(buf))
    {
        errno = EINVAL;
        return "";
    }

    return std::string(sname.c_str());
#else
    char buf[BUFSIZ];
    getcwd(buf, BUFSIZ);
    return std::string(buf);
#endif    
}
bool FileSystem::changeWorkingDirectory(const std::string& dir)
{
#ifdef _WIN32
    nowide::wstackstring wname;
    if (!wname.convert(dir.c_str())) {
        errno = EINVAL;
        return false;
    }

    return _wchdir(wname.c_str()) == 0;
#else
    return chdir(dir.c_str()) == 0;
#endif
}

std::vector<std::string> FileSystem::findFiles(const std::string& wildcard, bool recursive)
{
    std::vector<std::string> files;

#ifdef _WIN32
    nowide::wstackstring wname;
    if (!wname.convert((getWorkingDirectory() + "\\*").c_str())) {
        errno = EINVAL;
        return files;
    }
    nowide::wstackstring wcard;
    if (!wcard.convert(wildcard.c_str())) {
        errno = EINVAL;
        return files;
    }

    WIN32_FIND_DATAW FindFileData;
    HANDLE hFind;
    unsigned int iFiles = 0;
    hFind = FindFirstFileW(wname.c_str(), &FindFileData);

    while (hFind != INVALID_HANDLE_VALUE)
    {
        wchar_t* fname = FindFileData.cFileName;

        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && recursive && wcscmp(fname, L".") != 0 && wcscmp(fname, L"..") != 0)
        {
            if (_wchdir(fname) != 0)
            {
                errno = EINVAL;
                return files;
            }

            auto contents = findFiles(wildcard, recursive);
            std::copy(contents.begin(), contents.end(), std::back_inserter(files));
            
            if (_wchdir(L"..") != 0)
            {
                errno = EINVAL;
                return files;
            }
        }
        else if (wildcmp(wcard.c_str(), fname))
        {
            wchar_t filenameBuf[BUFSIZ];
            wchar_t** lastPart = nullptr;
            int ret = GetFullPathNameW(fname, BUFSIZ, filenameBuf, lastPart);
            if (ret == 0)
            {
                errno = EINVAL;
                return files;
            }

            nowide::stackstring sname;
            if (!sname.convert(filenameBuf))
            {
                errno = EINVAL;
                return files;
            }

            files.push_back(sname.c_str());
        }

        if (!FindNextFileW(hFind, &FindFileData))
            break;
    }

    FindClose(hFind);
#else
    DIR* dp;
    dirent* dirp;
    int iFiles = 0;
    if ((dp = opendir(getWorkingDirectory().c_str())) == NULL)
    {
        return files;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        std::string name(dirp->d_name);
        std::string fullName = dirName + "/" + name;
        
        if (isFolder(fullName) && recursive && (name != "." && name != ".."))
        {
            if (chdir(name.c_str()) != 0)
                return files;

            auto contents = findFiles(wildcard, recursive);
            std::copy(contents.begin(), contents.end(), std::back_inserter(files));
                
            if (chdir("..") != 0)
                return files;
        }
        else if (wildcmp(wildcard.c_str(), name.c_str()))
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
#ifdef _WIN32
    nowide::wstackstring wname;
    if (!wname.convert(file.c_str())) {
        errno = EINVAL;
        return false;
    }

    struct _stat32 fileStat;
    int err = _wstat32(wname.c_str(), &fileStat);
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
#ifdef _WIN32
    nowide::wstackstring wname;
    if (!wname.convert(folder.c_str())) {
        errno = EINVAL;
        return false;
    }

    struct _stat32 fileStat;
    int err = _wstat32(wname.c_str(), &fileStat);
#else
    struct stat fileStat;
    int err = stat(file.c_str(), &fileStat);
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
    std::replace_if(fixed.begin(), fixed.end(), [](char c)->bool { return c == '\\'; }, '/');
    if (fixed.back() == '/')
        fixed.erase(fixed.size() - 1);

    if (fixed.empty())
        return false;

    if (isFolder(fixed))
        return true;

    if (recurse)
    {
        std::string upOne = fixed;
        upOne.erase(upOne.find_last_of('/'));

        createFolder(upOne, true);
    }

#ifdef _WIN32
    nowide::wstackstring wname;
    if (!wname.convert(fixed.c_str())) {
        errno = EINVAL;
        return false;
    }

    return _wmkdir(wname.c_str()) == 0;
#else
    return mkdir(fixed.c_str(), S_IRWXU | S_IRWXG | S_IRXO) == 0;
#endif
}
bool FileSystem::deleteFile(const std::string& file)
{
    return nowide::remove(file.c_str()) == 0;
}

std::string FileSystem::getUserDir(const std::string& affix)
{
#ifdef _WIN32
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
        wstr[psize - 0] = '\0';
    }

    nowide::stackstring sname;
    if (!sname.convert(wstr))
    {
        CloseHandle(accessToken);
        FreeLibrary(lib);
        
        errno = EINVAL;
        return "";
    }

    CloseHandle(accessToken);
    FreeLibrary(lib);

    std::string profileDir = std::string(sname.c_str()) + affix;

    return profileDir;
#else
    char *envr = getenv("HOME");
    if (envr && isFolder(envr))
    {
        size_t len = strlen(envr);
        const size_t add_dirsep = (envr[envrlen - 1] != '/') ? 1 : 0;

        std::vector<char> storage(len + add_dirsep);
        strcpy(&storage[0], envr);

        if (add_dirsep)
            storage[len] = '/';

        return std::string(&storage[0], len + add_dirsep);
    }

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
            storage[len] = '/';

        return std::string(&storage[0], len + add_dirsep);
    }

    return "";
#endif
}
