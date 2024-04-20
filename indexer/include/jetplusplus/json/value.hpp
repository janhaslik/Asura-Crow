#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <iostream>

namespace jetpp
{
    class JsonValue
    {
    public:
        enum Type
        {
            OBJECT,
            ARRAY,
            STRING,
            NUMBER,
            BOOLEAN,
            NULL_VALUE
        };

        // Constructors
        JsonValue();
        JsonValue(const char *value);
        JsonValue(double value);
        JsonValue(bool value);

        // Methods to set values
        void setString(const std::string &value);
        void setNumber(double value);
        void setBoolean(bool value);
        void setObject(const std::map<std::string, JsonValue> &value);
        void setArray(const std::vector<JsonValue> &value);

        // Method to convert JSON value to JSON string
        std::string toJsonString() const;
        Type type;
        std::string asString;
        double asNumber;
        bool asBoolean;
        std::map<std::string, JsonValue> asObject;
        std::vector<JsonValue> asArray;
    private:
    };
} // namespace jetpp

#endif
