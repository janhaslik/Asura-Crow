#include "db/indexdb.hpp"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/bulk_write.hpp>
#include <iostream>

namespace indexer_db {

    /**
     * @brief Constructor for the IndexerDB class.
     * 
     * Initializes the MongoDB client connection and sets up initial documents for testing.
     */
    IndexerDB::IndexerDB() {
        try {
            const std::string uri = "mongodb://root:1234@localhost:27017/";

            // creating a shared_ptr for the database
            this->client = std::make_shared<mongocxx::client>(mongocxx::client{mongocxx::uri(uri)});
        } catch (const std::exception& e) {
            std::cerr << "Error connecting to MongoDB: " << e.what() << std::endl;
        }
    }

    /**
     * @brief Destructor for the IndexerDB class.
     * 
     * Cleans up resources.
     */
    IndexerDB::~IndexerDB() {
    }

    /**
     * @brief Upserts (updates or inserts) an index document for a specific term.
     * 
     * This function updates or inserts a document into the MongoDB index collection for a specific term.
     * 
     * @param document The document to be indexed.
     * @param term The term for which the document is being indexed.
     */
    void IndexerDB::upsertIndexDocument(const IndexDocument& document, std::string term) {
        try {
            bsoncxx::builder::stream::document doc_builder{};
            doc_builder << "url" << document.url
                        << "tf" << document.tf
                        << "docLength" << document.docLength;

            auto db = this->client->database("AsuraCrow_DB");
            auto indexDocuments = db.collection("index");

            auto documents = this->getTermDocuments(term);

            // Check for updating a document
            bool found = false;
            for (auto& d : documents) {
                if (d.url == document.url) {
                    d = document;
                    found = true;
                    break;
                }
            }

            // If new, just push to documents
            if (!found) documents.push_back(document);

            //build the documents array
            bsoncxx::builder::stream::array arr_builder{};
            for (const auto& d : documents) {
                bsoncxx::builder::stream::document inner_doc_builder{};
                inner_doc_builder << "url" << d.url
                                  << "tf" << d.tf
                                  << "docLength" << d.docLength;
                arr_builder << inner_doc_builder;
            }

            auto filter = bsoncxx::builder::stream::document{} << "term" << term << bsoncxx::builder::stream::finalize;

            // Use update_one with upsert option
            auto update = bsoncxx::builder::stream::document{} << "$set"
                                                               << bsoncxx::builder::stream::open_document
                                                               << "documents" << arr_builder
                                                               << bsoncxx::builder::stream::close_document
                                                               << bsoncxx::builder::stream::finalize;

            indexDocuments.update_one(filter.view(), update.view(), mongocxx::options::update().upsert(true));
        } catch (const mongocxx::exception& e) {
            std::cerr << "MongoDB Error upserting document: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error upserting document into MongoDB: " << e.what() << std::endl;
        }
    }

    /**
     * @brief Retrieves documents that contain a specific term from the database.
     * 
     * This function fetches documents associated with a specific term from the MongoDB index collection.
     * 
     * @param term The search term.
     * @return A vector of IndexDocument structures containing documents that include the term.
     */
    std::vector<IndexDocument> IndexerDB::getTermDocuments(std::string term) {
        std::vector<IndexDocument> result;
        try {
            auto db = this->client->database("AsuraCrow_DB");
            auto indexDocuments = db.collection("index");
            auto filter = bsoncxx::builder::stream::document{} << "term" << term << bsoncxx::builder::stream::finalize;

            // Define projection option
            mongocxx::options::find findOpts{};
            findOpts.projection(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("documents", 1)));

            // Retrieve one index document matching the term from the database
            auto cursor = indexDocuments.find_one(filter.view(), findOpts);

            if (cursor) {
                auto termDoc = *cursor;

                if (termDoc.find("documents") != termDoc.end()) {
                    auto documents_array = termDoc["documents"].get_array().value;
                    for (const auto& doc_value : documents_array) {
                        auto doc = doc_value.get_document().value;
                        if (doc.find("url") != doc.end() &&
                            doc.find("tf") != doc.end() &&
                            doc.find("docLength") != doc.end()) {

                            IndexDocument index_doc;
                            index_doc.url = doc["url"].get_string().value.to_string();
                            index_doc.tf = doc["tf"].get_double().value;
                            index_doc.docLength = doc["docLength"].get_int32().value;
                            result.push_back(index_doc);
                        } else {
                            std::cerr << "Error: Invalid document retrieved from MongoDB." << std::endl;
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error retrieving documents from MongoDB: " << e.what() << std::endl;
        }
        return result;
    }

}