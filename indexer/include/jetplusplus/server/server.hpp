#ifndef SERVER_HPP
#define SERVER_HPP

#include "../router/router.hpp"
#include "../server/request.hpp"
#include "../server/response.hpp"
#include <optional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

namespace jetpp
{
    class Server
    {
    private:
        Router router;
        int port;
        void handleClient(int clientSocket);

    public:
        Server(Router router);
        void start(int port);
        void sendResponse(int clientSocket, const char *response); // Change SOCKET to int
        std::string getFullClientAddress(const std::string& request);
    };
}

#endif
