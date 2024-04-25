#ifndef INDEXER_HPP
#define INDEXER_HPP
#include <string>
#include <vector>
#include "db/db.hpp"

namespace indexer {
// Define a struct for document
struct Document {
    const std::string url;
    const std::string content;
};


class Indexer
{
private:
    indexer_db::IndexerDB db;
    int totalDocuments;
    std::unordered_map<std::string, std::vector<indexer_db::IndexDocument>> index;
    void splitContentUniqueTerms(const std::string& str, std::unordered_map<std::string, int> &terms, char delimiter);
    void removeWhitespace(std::string& str);
public:
    Indexer();
    void indexDocument(Document *doc);
    void printIndex();
};

}

#endif

