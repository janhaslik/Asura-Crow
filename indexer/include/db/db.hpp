#ifndef DB_HPP
#define DB_HPP
#include <string>

namespace indexer_db {


    struct IndexDocument{
        const std::string url;
        const int termCount;
        const float tf;
        float idf;
    };

    class indexer_db
    {
    private:
    public:
        indexer_db();
        ~indexer_db();
        void insertIndexDocument(IndexDocument document);
    };
}

#endif