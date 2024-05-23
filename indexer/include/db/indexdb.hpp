#ifndef INDEXERDB_HPP
#define INDEXERDB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace indexer_db {

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
     * @class IndexerDB
     * @brief A class to interact with the index database.
     * 
     * This class provides functionality to upsert (update or insert) documents into the index
     * and retrieve documents for a specific term.
     */
    class IndexerDB {
    public:
        /**
         * @brief Default constructor for the IndexerDB class.
         * 
         * Initializes the MongoDB client connection.
         */
        IndexerDB();

        /**
         * @brief Destructor for the IndexerDB class.
         * 
         * Cleans up resources.
         */
        ~IndexerDB();

        /**
         * @brief Upserts (updates or inserts) an index document for a specific term.
         * 
         * @param document The document to be indexed.
         * @param term The term for which the document is being indexed.
         */
        void upsertIndexDocument(const IndexDocument& document, std::string term);

    private:
        std::shared_ptr<mongocxx::client> client; ///< Shared pointer to the MongoDB client.

        /**
         * @brief Retrieves documents that contain a specific term from the database.
         * 
         * @param term The search term.
         * @return A vector of IndexDocument structures containing documents that include the term.
         */
        std::vector<IndexDocument> getTermDocuments(std::string term);
    };

}

#endif
