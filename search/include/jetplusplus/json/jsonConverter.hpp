#ifndef JSONCONVERTER_HPP
#define JSONCONVERTER_HPP
#include <string>
#include <vector>
#include "value.hpp"
#include <sstream>

namespace jetpp
{
    class JsonConverter{
        public:
            JsonConverter();
            std::string jsonToString(jetpp::JsonValue value);
            jetpp::JsonValue stringToJson(std::string value);
        private:
            void splitString(std::string str, std::vector<std::string> &segments, char delimiter);
    };
} // namespace jetpp

#endif
