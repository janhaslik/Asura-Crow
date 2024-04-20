#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../router/route.hpp"
#include <string>
#include <vector>
#include <optional>
#include "../server/request.hpp"
#include "../server/response.hpp"
#include "../container/container.hpp"
#include <memory>

namespace jetpp
{
    class Router
    {
    public:
        Router();
        
        //with default container
        void get(const std::string &routeurl, void (*callback)(Request &, Response &));
        void post(const std::string &routeurl, void (*callback)(Request &, Response &));
        void put(const std::string &routeurl, void (*callback)(Request &, Response &));
        void patch(const std::string &routeurl, void (*callback)(Request &, Response &));
        void Delete(const std::string &routeurl, void (*callback)(Request &, Response &));
        void options(const std::string &routeurl, void (*callback)(Request &, Response &));

        //with container
        void get(const std::string &routeurl, void (*callback)(Request &, Response &), std::shared_ptr<Container>& container);
        void post(const std::string &routeurl, void (*callback)(Request &, Response &), std::shared_ptr<Container>& container);
        void put(const std::string &routeurl, void (*callback)(Request &, Response &), std::shared_ptr<Container>& container);
        void patch(const std::string &routeurl, void (*callback)(Request &, Response &), std::shared_ptr<Container>& container);
        void Delete(const std::string &routeurl, void (*callback)(Request &, Response &), std::shared_ptr<Container>& container);
        void options(const std::string &routeurl, void (*callback)(Request &, Response &), std::shared_ptr<Container>& container);

        std::optional<Route> findRoute(std::string request, jetpp::Methods method, std::string clientAddress);
    private:
        std::vector<Route> routes;
        std::vector<std::shared_ptr<Container>> containers;
        void splitRoute(std::string str, std::vector<std::string> &segments, char delimiter);
        static bool checkAccess(std::shared_ptr<Container>& container, std::string clientAddress);
        void addRoute(const std::string &routeurl, jetpp::Methods method, void (*callback)(Request &, Response &), std::shared_ptr<Container>& container);
        void checkContainer(std::shared_ptr<Container>& container);

    };
}

#endif
