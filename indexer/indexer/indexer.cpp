#include "indexer/indexer.hpp"
#include <iostream>

namespace indexer{

    Indexer::Indexer(){

    }

    void Indexer::indexDocument(Document *doc) {
        std::cout << "Url: " << doc->url << std::endl;
        std::cout << "Content: " << doc->content << std::endl;
    }

}
