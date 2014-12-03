#pragma once

#include <string>

namespace Util
{

class INIFile
{
public:
    INIFile();
    INIFile(const INIFile&) = default;
    ~INIFile() = default;

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path);



private:
};

}
