#ifndef CONTAINER_HPP
#define CONTAINER_HPP
#include "../router/route.hpp"
#include <string>
#include <vector>
#include <random>
#include <sstream>

namespace jetpp{

    class Container{

        private:
        std::string containerId;
        std::vector<Route> routes;
        std::vector<std::string> accessList;

        public:
        Container();
        
        void addRoute(Route route);
        void addAccessHost(std::string host);
        std::vector<Route> getRoutes();
        std::vector<std::string> getAccessList();
        std::string getContainerId();
    };
}

#endif