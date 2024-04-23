/*#include "db/db.hpp"
#include <iostream>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace indexer_db {

    indexer_db::indexer_db() {
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        const std::string uri = "mongodb://root:1234@localhost:27017/";
         mongocxx::instance inst;

        try {

            auto client = mongocxx::client{mongocxx::uri(uri)};
            auto admin = client["admin"];
            auto result = admin.run_command(make_document(kvp("ping", 1)));
            std::cout << bsoncxx::to_json(result) << std::endl;
        } catch (const std::exception& xcp) {
            std::cout << "connection failed: " << xcp.what() << std::endl;
        }
    }

    indexer_db::~indexer_db() {
        // Destructor implementation if needed
    }

    void indexer_db::insertIndexDocument(IndexDocument document) {
        // Implementation for inserting a document into the collection
        // This function can be added to perform insert operations
        std::cout << "Inserting document..." << std::endl;
    }

} */// namespace indexer_db

#include "db/db.hpp"
#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace indexer_db {

    indexer_db::indexer_db() {
        const std::string uri = "mongodb://root:1234@localhost:27017/";
        //mongocxx::instance inst;
    }

    indexer_db::~indexer_db() {
        // Destructor implementation if needed
    }

    void indexer_db::insertIndexDocument(IndexDocument document) {
        // Implementation for inserting a document into the collection
        // This function can be added to perform insert operations
        std::cout << "Inserting document..." << std::endl;
    }

} // namespace indexer_db

