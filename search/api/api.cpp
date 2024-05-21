#include "jetplusplus/server/server.hpp"
#include "jetplusplus/router/router.hpp"
#include "jetplusplus/json/jsonConverter.hpp"
#include "jetplusplus/json/value.hpp"
#include <searcher/searcher.hpp>
#include <mongocxx/instance.hpp>
#include <memory>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Main function to set up and start the server.
 * 
 * @return int Exit status of the application.
 */
int main() {

    // Create a Router object to handle HTTP routes
    jetpp::Router router;
    // Initialize MongoDB instance (required for MongoDB operations)
    mongocxx::instance instance{};
    
    // Define a POST route for searching
    router.post("/search", [&](jetpp::Request& req, jetpp::Response& res) {
        try {
            std::shared_ptr<searcher::Searcher> searcher;
            try {
                // Initialize the Searcher object
                searcher = std::make_shared<searcher::Searcher>();
            } catch(const std::exception& e) {
                std::cerr << "Error initializing searcher: " << e.what() << '\n';
                res.status(500).send("Internal Server Error");
                return;
            }

            // Extract the query parameter from the request
            std::string query = req.query["q"];
            // Perform the search and get the list of URLs
            std::vector<std::string> urls = searcher->search(query);

            // Create a JSON array to hold the search results
            jetpp::JsonValue result;
            result.setArray({});

            // Add each URL to the JSON array
            for(const std::string& url: urls){
                jetpp::JsonValue val;
                val.setString(url);
                result.asArray.push_back(val);
            }

            // Send the JSON response
            res.json(result);
        } catch (const std::exception& e) {
            std::cerr << "Error processing search request: " << e.what() << std::endl;
            res.status(500).send("Internal Server Error");
        }
    });

    // Create a Server object with the defined router and start it on port 7002
    jetpp::Server server(router);
    try {
        server.start(7002);
    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
