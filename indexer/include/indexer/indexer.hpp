#ifndef INDEXER_HPP
#define INDEXER_HPP
#include <string>

namespace indexer{
// Define a struct for document
struct Document {
    const std::string url;
    const std::string content;
};

struct IndexDocument{
    const std::string url;
    const int termCount;
    const float tf;
    float idf;
};

class Indexer
{
private:
    int totalDocuments;
    std::unordered_map<std::string, std::vector<IndexDocument>> index;
    void splitContentUniqueTerms(const std::string& str, std::unordered_map<std::string, int> &terms, char delimiter);
    void removeWhitespace(std::string& str);
public:
    Indexer();
    void indexDocument(Document *doc);
    void printIndex();
};

}

#endif

