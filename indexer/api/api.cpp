#include "jetplusplus/server/server.hpp"
#include "jetplusplus/router/router.hpp"
#include "jetplusplus/json/jsonConverter.hpp"
#include "jetplusplus/json/value.hpp"
#include "indexer/indexer.hpp"
#include <iostream>
#include <string>
#include <memory>

int main() {
    jetpp::Router router;
    std::shared_ptr<indexer::Indexer> indexPtr = std::make_shared<indexer::Indexer>();


    router.post("/index",[&](jetpp::Request &req, jetpp::Response &res) {
        std::string document = req.body;

        jetpp::JsonConverter jsonConverter;
        jetpp::JsonValue doc = jsonConverter.stringToJson(document);

        // access 'url' and 'content' from the JSON document
        std::string url = doc.asObject["url"].asString;
        std::string content = doc.asObject["content"].asString;

        // create an indexing document
        indexer::Document indexingDocument{url, content};
        // index the document
        indexPtr->indexDocument(&indexingDocument);
        
        res.status(200).send("Processing successfull");
    });
    
    // server, port: 7001
    jetpp::Server server(router);
    server.start(7001);

    return 0;
}
