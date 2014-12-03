#pragma once

#include <string>
#include <map>

#include <cstdint>

namespace Util
{

class INIFile
{
public:
    struct Value
    {
        Value();

        Value(uint8_t);
        Value(uint16_t);
        Value(uint32_t);
        Value(uint64_t);
        Value(int8_t);
        Value(int16_t);
        Value(int32_t);
        Value(int64_t);
        Value(float);
        Value(double);
        Value(const std::string&);

        Value(const Value&);

        ~Value();

        Value& operator=(const Value&);

        enum {
            Type_Null,

            Type_Int,
            Type_Float,
            Type_String
        } Type;

        union
        {
            uint8_t u8;
            uint16_t u16;
            uint32_t u32;
            uint64_t u64;

            int8_t i8;
            int16_t i16;
            int32_t i32;
            int64_t i64;

            float f;
            double d;

            char* s;
        };
    };

    INIFile();
    INIFile(const INIFile&) = default;
    ~INIFile() = default;

    INIFile& operator=(const INIFile&) = default;

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path);

    Value& getValue(const std::string& name, const std::string& section = "");

private:
    std::map<std::string, std::map<std::string, Value>> mValues;
};

}
