#include "db/db.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>


namespace indexer_db {

    IndexerDB::IndexerDB() {
        std::string uri="mongodb://root:1234@localhost:27017/";

        mongocxx::instance instance{};
        mongocxx::client client{mongocxx::uri(uri)};

        mongocxx::database db=client["AsuraCrow_DB"];

        mongocxx::collection indexDocuments=db["index"];

        auto doc=bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("url","testurl:)"));
        auto element=doc.view();
        indexDocuments.insert_one(element);
    }

    IndexerDB::~IndexerDB() {
    }

    void IndexerDB::insertIndexDocument(const IndexDocument& document, std::string term) {
    
    }
} 
