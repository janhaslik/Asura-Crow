#include "indexer/indexer.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

namespace indexer {

    /**
     * @brief Default constructor for the Indexer class.
     * 
     * Initializes the database connection using a shared pointer to IndexerDB.
     */
    Indexer::Indexer() {
        this->db = std::make_shared<indexer_db::IndexerDB>();
    }

    /**
     * @brief Indexes a given document.
     * 
     * This function processes the content of the document, extracts unique terms, calculates their term frequencies (TF),
     * and inserts or updates the index database with the term information.
     * 
     * @param document Pointer to the document to be indexed.
     */
    void Indexer::indexDocument(Document* document) {
        std::unordered_map<std::string, int> terms;
        splitContentUniqueTerms(document->content, terms, ' ');

        // Process each term extracted from the document
        for (const auto& pair : terms) {
            std::string term = pair.first;
            removeWhitespace(term);

            // Calculate term frequency (TF) for the current term in the document
            float tf = static_cast<float>(pair.second) / terms.size();

            try {
                indexer_db::IndexDocument indexDocument{document->url, tf, static_cast<int>(document->content.size())};
                this->db.get()->upsertIndexDocument(indexDocument, term);
            } catch (std::exception& e) {
                std::cerr << "Error executing upsert: " << e.what() << std::endl;
            }
        }
    }

    /**
     * @brief Splits the content into unique terms based on a delimiter.
     * 
     * This function splits the content string into segments (terms) and counts the frequency of each unique term.
     * 
     * @param str The content string to be split.
     * @param terms The map to store the terms and their frequencies.
     * @param delimiter The character used to split the string.
     */
    void Indexer::splitContentUniqueTerms(const std::string& str, std::unordered_map<std::string, int>& terms, char delimiter) {
        std::istringstream isstream(str);
        std::string segment;
        while (std::getline(isstream, segment, delimiter)) {
            terms[segment]++;
        }
    }

    /**
     * @brief Removes whitespace from a string.
     * 
     * This function removes all whitespace characters from the given string.
     * 
     * @param str The string from which whitespace will be removed.
     */
    void Indexer::removeWhitespace(std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    }

}
