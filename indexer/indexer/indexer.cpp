#include "indexer/indexer.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

namespace indexer {

    Indexer::Indexer() {
        totalDocuments=0;
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

            // calculate inverse document frequency (IDF)
            float idf=0.0f;
            if (this->index.find(term) != this->index.end()) {
                idf = std::log(static_cast<float>(this->totalDocuments) / (this->index[term].size())+1);
            } else {
                idf = std::log(static_cast<float>(this->totalDocuments) / 1); // Default IDF for new term
            }       

            if(idf==-INFINITY)  {
                idf=0;
            }   

            indexer_db::IndexDocument indexDocument{ document->url, pair.second, tf, idf, tf*idf, document->content.size(), };
            // Check if the term already exists in the index map
            if (this->index.find(term) != this->index.end()) {
                bool found = false;

                // check if the current url already exists for this term
                for (auto& doc : this->index[term]) {
                    doc.idf = idf;
                    if (doc.url == document->url) {
                        found = true;
                        break;
                    }
                }

                // if the url is not found for the term, add it to the index
                if (!found) {
                    this->index[term].push_back(indexDocument);
                    this->db.insertIndexDocument(indexDocument, term);
                }
            } else {
                this->index[term].push_back(indexDocument);
                this->db.insertIndexDocument(indexDocument, term);
            }
        }
        this->totalDocuments++;
    }


    void Indexer::splitContentUniqueTerms(const std::string& str, std::unordered_map<std::string, int>& terms, char delimiter) {
        std::istringstream isstream(str);
        std::string segment;
        while (std::getline(isstream, segment, delimiter)) {
            terms[segment]++;
        }
    }

    void Indexer::printIndex(){
        for (const auto& entry : this->index) {
            const std::string& term = entry.first;
            const std::vector<indexer_db::IndexDocument>& documents = entry.second;
            ///if (term != "apple")continue;
            std::cout << "Term: " << term << std::endl;

            for (const indexer_db::IndexDocument& doc : documents) {
                std::cout << "  Document URL: " << doc.url << std::endl;
                std::cout << "  Term count: " << doc.termCount << std::endl;
                std::cout << "  TF: " << doc.tf << std::endl;
                std::cout << "  IDF: " << doc.idf << std::endl;
                std::cout << "  TF-IDF: " << doc.tf*doc.idf << std::endl;
            }

            std::cout << std::endl;
        }
    }

    void Indexer::removeWhitespace(std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    }

}
