#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>

#include "../json/value.hpp"
#include "../json/jsonConverter.hpp"

namespace jetpp
{
    class Response
    {
    private:
        int statuscode;
        int clientSocket;
        std::vector<std::string> header;

    public:
        Response(int clientSocket);
        void send(std::string message);
        void sendFile(std::string path);
        jetpp::Response status(int status);
        void json(jetpp::JsonValue object);
        void addHeader(const std::string &key, const std::string &value);
    };
}

#endif
