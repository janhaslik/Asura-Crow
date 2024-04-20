#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace jetpp
{
    class Request
    {
    private:
        std::vector<std::string> routeSplitted;
        std::vector<std::string> requestSplitted;
        std::string requesturl;
        std::string routeurl;
        std::string request;
        void setQuery();
        void setParams();
        void setHeaders();
        void setBody();
        void splitString(std::string str, std::vector<std::string> &segments, char delimiter);

    public:
        std::unordered_map<std::string, std::string> params;
        std::unordered_map<std::string, std::string> query;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
        Request(std::string requesturl, std::string routeurl, std::string request);
    };
}

#endif
