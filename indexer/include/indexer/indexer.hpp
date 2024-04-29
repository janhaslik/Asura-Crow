#ifndef INDEXER_HPP
#define INDEXER_HPP
#include <string>
#include <vector>
#include <memory>
#include "db/db.hpp"

namespace indexer {

struct Document {
    const std::string url;
    const std::string content;
};


class Indexer
{
private:
     std::shared_ptr<indexer_db::IndexerDB> db;
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