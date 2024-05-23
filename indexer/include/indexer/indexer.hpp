#ifndef INDEXER_HPP
#define INDEXER_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "db/indexdb.hpp"

namespace indexer {

    /**
     * @struct Document
     * @brief Structure to represent a document to be indexed.
     * 
     * This structure holds the URL and content of the document.
     */
    struct Document {
        const std::string url;      ///< URL of the document.
        const std::string content;  ///< Content of the document.
    };

    /**
     * @class Indexer
     * @brief A class to handle the indexing of documents.
     * 
     * This class provides functionality to index documents by extracting terms
     * and storing them in a database.
     */
    class Indexer {
    public:
        /**
         * @brief Default constructor for the Indexer class.
         * 
         * Initializes the database connection and other necessary components.
         */
        Indexer();

        /**
         * @brief Indexes a given document.
         * 
         * @param doc Pointer to the document to be indexed.
         */
        void indexDocument(Document *doc);

    private:
        std::shared_ptr<indexer_db::IndexerDB> db; ///< Shared pointer to the database object.
        int totalDocuments; ///< Total number of documents indexed.
        std::unordered_map<std::string, std::vector<indexer_db::IndexDocument>> index; ///< Index map storing term to document mappings.

        /**
         * @brief Splits the content into unique terms based on a delimiter.
         * 
         * @param str The content string to be split.
         * @param terms The map to store the terms and their frequencies.
         * @param delimiter The character used to split the string.
         */
        void splitContentUniqueTerms(const std::string& str, std::unordered_map<std::string, int> &terms, char delimiter);

        /**
         * @brief Removes whitespace from a string.
         * 
         * @param str The string from which whitespace will be removed.
         */
        void removeWhitespace(std::string& str);
    };

}

#endif
