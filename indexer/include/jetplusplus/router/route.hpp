#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <functional>
#include "../server/request.hpp"
#include "../server/response.hpp"
#include "../methods/methods.hpp"

namespace jetpp
{
    class Route
    {
    public:
        Route(const std::string routeurl, jetpp::Methods method, std::function<void(Request &, Response &)> callback);

        std::string getRouteurl();
        jetpp::Methods getMethod();
        void execute(Request &request, Response &response);

    private:
        std::string routeurl;
        jetpp::Methods method;
        std::function<void(Request &, Response &)> callback;
    };
}

#endif
