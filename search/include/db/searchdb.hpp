#ifndef SEARCHERDB_HPP
#define SEARCHERDB_HPP

#include <string>
#include <mongocxx/client.hpp>
#include <memory>
#include <vector>

namespace searcher_db {

    /**
     * @struct IndexDocument
     * @brief Structure to represent an indexed document.
     * 
     * This structure holds the URL of the document, its term frequency (TF) for a specific term,
     * and the length of the document.
     */
    struct IndexDocument {
        std::string url;    ///< URL of the document.
        float tf;           ///< Term Frequency of the specific term in the document.
        int docLength;      ///< Length of the document.
    };

    /**
     * @class SearcherDB
     * @brief A class to interact with the search database.
     * 
     * This class provides functionality to retrieve documents from the database
     * based on search terms and to get the total number of documents in the database.
     */
    class SearcherDB {
    public:
        /**
         * @brief Default constructor for the SearcherDB class.
         * 
         * Initializes the MongoDB client connection.
         */
        SearcherDB();

        /**
         * @brief Retrieves documents that contain a specific term.
         * 
         * @param term The search term.
         * @return A vector of IndexDocument structures containing documents that include the term.
         */
        std::vector<IndexDocument> getDocumentsByTerm(std::string term);

        /**
         * @brief Gets the total number of documents in the database.
         * 
         * @return The total number of documents.
         */
        int getTotalNumberDocuments();

    private:
        std::shared_ptr<mongocxx::client> client; ///< Shared pointer to the MongoDB client.
    };

}

#endif
