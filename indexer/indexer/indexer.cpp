#include "indexer/indexer.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

namespace indexer {

    Indexer::Indexer() {
        this->db=std::make_shared<indexer_db::IndexerDB>();
    }

    void Indexer::indexDocument(Document* document) {
        std::unordered_map<std::string, int> terms;
        splitContentUniqueTerms(document->content, terms, ' ');

        // process each term extracted from the document
        for (const auto& pair : terms) {
            std::string term = pair.first;
            removeWhitespace(term);

            // calculate term frequency (TF) for the current term in the document
            float tf = static_cast<float>(pair.second) / terms.size();

            try{
                indexer_db::IndexDocument indexDocument{document->url, tf, static_cast<int>(document->content.size())};
                this->db.get()->upsertIndexDocument(indexDocument, term);
            }catch(std::exception& e){
                std::cerr << "Error exec upserting: " << e.what() << std::endl;
            }
        }
    }


    void Indexer::splitContentUniqueTerms(const std::string& str, std::unordered_map<std::string, int>& terms, char delimiter) {
        std::istringstream isstream(str);
        std::string segment;
        while (std::getline(isstream, segment, delimiter)) {
            terms[segment]++;
        }
    }

    void Indexer::removeWhitespace(std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    }

}