#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace indexer_db {

    struct IndexDocument {
        std::string url;
        float tf;
        float docLength;
    };

    class IndexerDB {
    public:
        IndexerDB();
        ~IndexerDB();

        void upsertIndexDocument(const IndexDocument& document, std::string term);
        std::vector<IndexDocument> getTermDocuments(std::string term);

    private:
        std::shared_ptr<mongocxx::client> client; // using shared_ptr for managing the database pointer
    };

}

#endif