#include "jetplusplus/server/server.hpp"
#include "jetplusplus/router/router.hpp"
#include "jetplusplus/json/jsonConverter.hpp"
#include "jetplusplus/json/value.hpp"
#include "indexer/indexer.hpp"
#include <mongocxx/instance.hpp>
#include <iostream>
#include <string>
#include <memory>

int main() {

    jetpp::Router router;
    
    //call mongocxx instance once
    mongocxx::instance instance{};

    router.post("/index", [&](jetpp::Request& req, jetpp::Response& res) {
        try {
            std::string document = req.body;

            jetpp::JsonConverter jsonConverter;
            jetpp::JsonValue doc = jsonConverter.stringToJson(document);

            // access url and content from the JSON document
            std::string url = doc.asObject["url"].asString;
            std::string content = doc.asObject["content"].asString;

            // init MongoDB indexer
            std::shared_ptr<indexer::Indexer> indexPtr;
            try {
                indexPtr = std::make_shared<indexer::Indexer>();
            }catch (const std::exception& e) {
                std::cerr << "Error initializing MongoDB indexer: " << e.what() << std::endl;
            }

            // create an indexing document
            indexer::Document indexingDocument{url, content};
            // index the document
            indexPtr->indexDocument(&indexingDocument);

            res.status(200).send("Processing successful");
        } catch (const std::exception& e) {
            std::cerr << "Error processing indexing request: " << e.what() << std::endl;
            res.status(500).send("Internal Server Error");
        }
    });

    // start jet++ server on port 7001
    jetpp::Server server(router);
    try {
        server.start(7001);
        std::cout << "Server started successfully on port 7001" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}