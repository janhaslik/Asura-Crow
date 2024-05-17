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

int main() {

    jetpp::Router router;
    mongocxx::instance instance{};
    
    router.post("/search", [&](jetpp::Request& req, jetpp::Response& res) {
        try {

            std::shared_ptr<searcher::Searcher> searcher;
            try
            {
                searcher=std::make_shared<searcher::Searcher>();
            }
            catch(const std::exception& e)
            {
                std::cerr << "Error initializing searcher: " << e.what() << '\n';
            }
            
            std::string query=req.query["q"];
            std::vector<std::string> urls=searcher.get()->search(query);

            jetpp::JsonValue result;
            result.setArray({});

            for(const std::string url: urls){
                jetpp::JsonValue val;
                val.setString(url);
                result.asArray.push_back(val);
            }


            res.json(result);
        } catch (const std::exception& e) {
            std::cerr << "Error processing search request: " << e.what() << std::endl;
            res.status(500).send("Internal Server Error");
        }
    });

    // start jet++ server on port 7001
    jetpp::Server server(router);
    try {
        server.start(7001);
    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}