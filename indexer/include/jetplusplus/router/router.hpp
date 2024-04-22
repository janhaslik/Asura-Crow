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
        void get(const std::string &routeurl, std::function<void(Request &, Response &)> callback);
        void post(const std::string &routeurl, std::function<void(Request &, Response &)> callback);
        void put(const std::string &routeurl, std::function<void(Request &, Response &)> callback);
        void patch(const std::string &routeurl, std::function<void(Request &, Response &)> callback);
        void Delete(const std::string &routeurl, std::function<void(Request &, Response &)> callback);
        void options(const std::string &routeurl, std::function<void(Request &, Response &)> callback);

        //with container
        void get(const std::string &routeurl, std::function<void(Request &, Response &)> callback, std::shared_ptr<Container>& container);
        void post(const std::string &routeurl, std::function<void(Request &, Response &)> callback, std::shared_ptr<Container>& container);
        void put(const std::string &routeurl, std::function<void(Request &, Response &)> callback, std::shared_ptr<Container>& container);
        void patch(const std::string &routeurl, std::function<void(Request &, Response &)> callback, std::shared_ptr<Container>& container);
        void Delete(const std::string &routeurl, std::function<void(Request &, Response &)> callback, std::shared_ptr<Container>& container);
        void options(const std::string &routeurl, std::function<void(Request &, Response &)> callback, std::shared_ptr<Container>& container);

        std::optional<Route> findRoute(std::string request, jetpp::Methods method, std::string clientAddress);
    private:
        std::vector<Route> routes;
        std::vector<std::shared_ptr<Container>> containers;
        void splitRoute(std::string str, std::vector<std::string> &segments, char delimiter);
        static bool checkAccess(std::shared_ptr<Container>& container, std::string clientAddress);
        void addRoute(const std::string &routeurl, jetpp::Methods method, std::function<void(Request &, Response &)> callback, std::shared_ptr<Container>& container);
        void checkContainer(std::shared_ptr<Container>& container);

    };
}

#endif
