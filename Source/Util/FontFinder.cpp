#include "FontFinder.hpp"

#include <array>
#include <sstream>

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
#include <nowide/convert.hpp>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace
{
    std::string GetSystemFontFile(const std::string& faceName)
    {
        static const LPWSTR fontRegistryPath = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
        HKEY hKey;
        LONG result;
        std::wstring wsFaceName(faceName.begin(), faceName.end());
        wsFaceName.push_back('*');

        // Open Windows font registry key
        result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey);
        if (result != ERROR_SUCCESS) {
            return "";
        }

        DWORD maxValueNameSize, maxValueDataSize;
        result = RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
        if (result != ERROR_SUCCESS) {
            return "";
        }

        DWORD valueIndex = 0;
        LPWSTR valueName = new WCHAR[maxValueNameSize];
        LPBYTE valueData = new BYTE[maxValueDataSize];
        DWORD valueNameSize, valueDataSize, valueType;
        std::wstring wsFontFile;

        // Look for a matching font name
        do {

            wsFontFile.clear();
            valueDataSize = maxValueDataSize;
            valueNameSize = maxValueNameSize;

            result = RegEnumValueW(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

            valueIndex++;

            if (result != ERROR_SUCCESS || valueType != REG_SZ) {
                continue;
            }

            std::wstring wsValueName(valueName, valueNameSize);

            // Found a match
            if (wildcmp(wsFaceName.c_str(), wsValueName.c_str())) {

                wsFontFile.assign((LPWSTR)valueData, valueDataSize);
                break;
            }
        } while (result != ERROR_NO_MORE_ITEMS);

        delete[] valueName;
        delete[] valueData;

        RegCloseKey(hKey);

        if (wsFontFile.empty()) {
            return "";
        }

        // Build full font file path
        WCHAR winDir[MAX_PATH];
        GetWindowsDirectoryW(winDir, MAX_PATH);

        std::wstringstream ss;
        ss << winDir << "\\Fonts\\" << wsFontFile;
        wsFontFile = ss.str();

        return nowide::narrow(wsFontFile);
    }
}

sf::Font* Util::FontFinder::sCachedDefault = nullptr;

sf::Font& Util::FontFinder::getDefaultFont()
{
    if (!sCachedDefault)
    {
        sCachedDefault = new sf::Font();

        static std::array<std::string, 5> PossibleFonts = {
            "Arial", "Calibri", "Segoe UI", "Trebuchet MS", "Verdana"
        };

        auto it = std::find_if(PossibleFonts.begin(), PossibleFonts.end(), [](const std::string& font) -> bool {
            std::string file = GetSystemFontFile(font);

            return (!file.empty() && sCachedDefault->loadFromFile(file));
        });

        if (it == PossibleFonts.end())
            throw std::runtime_error("Failed to load default font, this should never happen.");
    }

    return *sCachedDefault;
}

bool Util::FontFinder::findFont(sf::Font& out, const std::string& name, const std::string& style)
{
    auto path = GetSystemFontFile(name);
    if (path.empty())
        return false;

    return out.loadFromFile(path);
}

#else
#include <fontconfig/fontconfig.h>
#include <algorithm>
#include <tuple>

namespace
{
    std::string getFont(const std::string& wildcard, const std::string& stylecard)
    {
        std::string ret;

        FcPattern* pat = FcPatternCreate();
        FcObjectSet* os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FILE, (char *)0);
        FcFontSet* fs = FcFontList(FontConfigData.config, pat, os);

        for (int i = 0; fs && i < fs->nfont; ++i)
        {
            FcPattern* font = fs->fonts[i];
            FcChar8 *file, *style, *family;
            if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
                FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
                FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch)
            {
                std::string tmp((char*)file);
                std::reverse(tmp.begin(), tmp.end());
                if (tmp.substr(0, 4) != "ftt.")
                    continue;

                if (wildcmp(wildcard.c_str(), (char*)family) && wildcmp(stylecard.c_str(), (char*)style))
                    ret = std::string((char*)file);
            }
        }

        if (fs) FcFontSetDestroy(fs);
        if (os) FcObjectSetDestroy(os);
        if (pat) FcPatternDestroy(pat);

        return ret;
    }

    class FontFinderInit
    {
    public:
        FontFinderInit()
        {
            FontConfigData.config = FcInitLoadConfigAndFonts();
        }
        ~FontFinderInit()
        {
            // FcFini(); ///\FIXME Crashes when finishing for some reason
        }
    } global;
}

sf::Font* Util::FontFinder::sCachedDefault = nullptr;

sf::Font& Util::FontFinder::getDefaultFont()
{
    if (!sCachedDefault)
    {
        sCachedDefault = new sf::Font();

        static std::array<std::tuple<std::string, std::string>, 5> PossibleFonts = {
            std::make_tuple("Arial", "Regular"),
            std::make_tuple("DejaVu Sans", "Book"),
            std::make_tuple("Liberation Sans", "Regular"),
            std::make_tuple("Bistream Vera Sans", "Roman"),
            std::make_tuple("Unifont", "Medium")
        };

        auto it = std::find_if(PossibleFonts.begin(), PossibleFonts.end(), [](const std::tuple<std::string, std::string>& font) -> bool {
            std::string file = getFont(std::get<0>(font), std::get<1>(font));

            return (!file.empty() && sCachedDefault->loadFromFile(file));
        });

        if (it == PossibleFonts.end())
            throw std::runtime_error("Could not find any valid default fonts.");
    }

    return *sCachedDefault;
}

bool Util::FontFinder::findFont(sf::Font& font, const std::string& wildcard, const std::string& stylecard)
{
    return font.loadFromFile(getFont(wildcard, stylecard));
#endif