#include "INIFile.hpp"
#include <Defines.hpp>

#include <nowide/fstream.hpp>

#include <cstring>

using Util::INIFile;

INIFile::Value::Value() : Type(Type_Null), u64(0) { }

INIFile::Value::Value(uint8_t v) : Type(Type_Int), u8(v) { }
INIFile::Value::Value(uint16_t v) : Type(Type_Int), u16(v) { }
INIFile::Value::Value(uint32_t v) : Type(Type_Int), u32(v) { }
INIFile::Value::Value(uint64_t v) : Type(Type_Int), u64(v) { }
INIFile::Value::Value(int8_t v) : Type(Type_Int), i8(v) { }
INIFile::Value::Value(int16_t v) : Type(Type_Int), i16(v) { }
INIFile::Value::Value(int32_t v) : Type(Type_Int), i32(v) { }
INIFile::Value::Value(int64_t v) : Type(Type_Int), i64(v) { }
INIFile::Value::Value(float v) : Type(Type_Float), f(v) { }
INIFile::Value::Value(double v) : Type(Type_Float), d(v) { }

#ifdef LD31_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

INIFile::Value::Value(const std::string& v) :
    Type(Type_String), s(nullptr)
{
    s = new char[v.size() + 1];
    strcpy(s, v.c_str());
}

INIFile::Value::Value(const Value& v) :
    Type(v.Type), s(nullptr)
{
    if (Type == Type_String)
    {
        s = new char[strlen(v.s) + 1];
        strcpy(s, v.s);
    }
    else
        u64 = v.u64;
}

INIFile::Value::~Value()
{
    if (Type == Type_String)
        delete[] s;
}

INIFile::Value& INIFile::Value::operator=(const Value& v)
{
    if (this == &v)
        return *this;

    if (Type == Type_String)
        delete[] s;

    Type = v.Type;

    if (Type == Type_String)
    {
        s = new char[strlen(v.s) + 1];
        strcpy(s, v.s);
    }
    else
        u64 = v.u64;

    return *this;
}

#ifdef LD31_WINDOWS
#pragma warning(pop)
#endif

namespace
{
    void skipWhitespace(std::string::iterator& it)
    {
        char c = *it;
        if (c != ' ' && c != '\t')
            return;
        
        while ((c = *it++) != 0 && (c == ' ' || c == '\t'));
    }

    void stripWhitespace(std::string& str)
    {
        size_t pos = str.find_first_not_of(' ');
        if (pos > 0)
            str.erase(0, pos);
        pos = str.find_last_not_of(' ');
        if (pos < str.size()-1)
            str.erase(pos);
    }
}


INIFile::INIFile()
{
}

bool INIFile::loadFromFile(const std::string& path)
{
    nowide::ifstream ifs;
    ifs.open(path.c_str());

    if (!ifs.is_open() || !ifs)
        return false;

    std::string curSectionName;
    while (ifs && !ifs.eof())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        auto it = line.begin();

        skipWhitespace(it);

        if (it == line.end())
            continue;

        switch (*it)
        {
        case ';':
        case '#': // Comment
            break;

        case '[': // Section
        {
            ++it;

            size_t end = line.find_last_of(']');
            assert(end != std::string::npos);
            curSectionName.clear();
            
            std::copy(it, line.begin() + end, std::back_inserter(curSectionName));
        } break;

        default: // Value
        {
            std::string valueName;
            std::string valueValue;

            size_t mid = line.find('=');
            assert(mid != std::string::npos);

            std::copy_if(it, line.begin() + mid, std::back_inserter(valueName), isalnum);
            std::copy(line.begin() + mid + 1, line.end(), std::back_inserter(valueValue));

            stripWhitespace(valueName);
            stripWhitespace(valueValue);

            switch (valueName[0])
            {
            case 'i':
                mValues[curSectionName][valueName] = Value((int64_t)atoll(valueValue.c_str()));
                break;
            case 'f':
                mValues[curSectionName][valueName] = atof(valueValue.c_str());
                break;
            case 's':
                mValues[curSectionName][valueName] = valueValue;
                break;
                
            default:
                break;
            }
        } break;
        }
    }

    return true;
}
bool INIFile::saveToFile(const std::string& path)
{
    nowide::ofstream ofs;
    ofs.open(path.c_str(), std::ios::trunc);
    if (!ofs.is_open() || !ofs)
        return false;

    std::string lastSection;
    for (auto& section : mValues)
    {
        if (section.first != lastSection)
        {
            if (!lastSection.empty())
                ofs << std::endl;

            lastSection = section.first;
            ofs << "[" << section.first << "]" << std::endl;
        }

        for (auto& val : section.second)
        {
            if (val.second.Type == Value::Type_Null)
                continue;

            ofs << val.first << "=";

            switch (val.second.Type)
            {
            case Value::Type_Int:
                ofs << val.second.i64;
                break;

            case Value::Type_Float:
                ofs << val.second.d;
                break;

            case Value::Type_String:
                ofs << std::string(val.second.s);
                break;
            }

            ofs << std::endl;
        }
    }

    return true;
}

INIFile::Value& INIFile::getValue(const std::string& name, const std::string& section)
{
    auto& cat = mValues[section];
    return cat[name];
}
