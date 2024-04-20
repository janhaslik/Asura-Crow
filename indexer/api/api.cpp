#include "jetplusplus/server/server.hpp"
#include "jetplusplus/router/router.hpp"
#include "jetplusplus/server/request.hpp"
#include "jetplusplus/server/response.hpp"
#include "jetplusplus/json/jsonConverter.hpp"
#include "jetplusplus/json/value.hpp"

#include "indexer/indexer.hpp"
#include <string>

int main(){
    jetpp::Router router;

    router.post("/index",[](jetpp::Request &req, jetpp::Response &res){
        std::string document=req.body;

        jetpp::JsonConverter jsonConverter;
        jetpp::JsonValue doc=jsonConverter.stringToJson(document);

        std::string url = doc.asObject["url"].asString;
        std::string content = doc.asObject["content"].asString;

        indexer::Document indexingDocument{url, content};

        indexer::Indexer indexer;
        indexer.indexDocument(&indexingDocument);
        res.status(200).json(doc);
    });

    jetpp::Server server(router);

    server.start(7001);
    return 0;
}