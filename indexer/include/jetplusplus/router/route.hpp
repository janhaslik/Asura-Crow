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
        Route(std::string name, jetpp::Methods method, void (*callback)(Request &, Response &));

        std::string getName();
        jetpp::Methods getMethod();
        void execute(Request &request, Response &response);

    private:
        std::string name;
        jetpp::Methods method;
        void (*callback)(Request &, Response &);
    };
}

#endif
