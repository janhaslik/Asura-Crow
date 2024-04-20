#ifndef INDEXER_HPP
#define INDEXER_HPP
#include <string>

namespace indexer{
// Define a struct for document
struct Document {
    const std::string url;
    const std::string content;
};

class Indexer
{
private:
    
public:
    Indexer();
    void indexDocument(Document *doc);
};

}

#endif

