#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <string>

namespace indexer_db {

    struct IndexDocument {
        std::string url;
        int termCount;
        float tf;
        float idf;
        float tf_idf;
        float docLength;
        float avgDocLength;
    };

    class IndexerDB {
    public:
        IndexerDB();
        ~IndexerDB();

        void insertIndexDocument(const IndexDocument& document, std::string term);

    private:
    };

}

#endif
