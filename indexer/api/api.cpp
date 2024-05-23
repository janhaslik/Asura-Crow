#include "jetplusplus/server/server.hpp"
#include "jetplusplus/router/router.hpp"
#include "jetplusplus/json/jsonConverter.hpp"
#include "jetplusplus/json/value.hpp"
#include "indexer/indexer.hpp"
#include <mongocxx/instance.hpp>
#include <iostream>
#include <string>
#include <memory>

/**
 * @brief Main function to run the indexing server.
 * 
 * This function sets up a router to handle HTTP POST requests for indexing documents.
 * It initializes the MongoDB instance, sets up the endpoint, and starts the server.
 * 
 * @return int Returns 0 on successful execution, 1 on failure.
 */
int main() {

    jetpp::Router router;

    // Call mongocxx instance once to initialize MongoDB driver
    mongocxx::instance instance{};

    /**
     * @brief Endpoint to handle document indexing requests.
     * 
     * This endpoint receives a document in JSON format via a POST request,
     * extracts the URL and content, initializes the indexer, and indexes the document.
     */
    router.post("/index", [&](jetpp::Request& req, jetpp::Response& res) {
        try {
            // Get the document from the request body
            std::string document = req.body;

            // Convert the document string to a JSON object
            jetpp::JsonConverter jsonConverter;
            jetpp::JsonValue doc = jsonConverter.stringToJson(document);

            // Access URL and content from the JSON document
            std::string url = doc.asObject["url"].asString;
            std::string content = doc.asObject["content"].asString;

            // Initialize MongoDB indexer
            std::shared_ptr<indexer::Indexer> indexPtr;
            try {
                indexPtr = std::make_shared<indexer::Indexer>();
            } catch (const std::exception& e) {
                std::cerr << "Error initializing indexer: " << e.what() << std::endl;
            }

            // Create an indexing document
            indexer::Document indexingDocument{url, content};
            // Index the document
            indexPtr->indexDocument(&indexingDocument);

            // Send success response
            res.status(200).send("Processing successful");
        } catch (const std::exception& e) {
            std::cerr << "Error processing indexing request: " << e.what() << std::endl;
            res.status(500).send("Internal Server Error");
        }
    });

    // Start Jet++ server on port 7001
    jetpp::Server server(router);
    try {
        server.start(7001);
    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}