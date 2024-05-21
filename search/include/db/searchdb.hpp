#ifndef SEARCHERDB_HPP
#define SEARCHERDB_HPP
#include <string>
#include <mongocxx/client.hpp>
#include <memory>

namespace searcher_db{

    struct IndexDocument {
        std::string url;
        float tf;
        int docLength;
    };

    class SearcherDB{
        public:
            SearcherDB();
            std::vector<IndexDocument> getDocumentsByTerm(std::string term);
            int getTotalNumberDocuments();
        private:
            std::shared_ptr<mongocxx::client> client;
    };
}

#endif