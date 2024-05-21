#include <db/searchdb.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/basic/document.hpp>

namespace searcher_db{

    // Constructor to initialize the SearcherDB class and establish a connection to the MongoDB database
    SearcherDB::SearcherDB(){
        try{
            // MongoDB connection URI
            const std::string uri = "mongodb://root:1234@localhost:27017/";
            this->client = std::make_shared<mongocxx::client>(mongocxx::client{mongocxx::uri(uri)});
        }catch (const std::exception& e) {
            std::cerr << "Error connecting to MongoDB: " << e.what() << std::endl;
        }
    }

    /**
     * @brief Retrieves documents associated with a specific search term from the database.
     * 
     * @param term The search term to look for in the database.
     * @return std::vector<IndexDocument> A vector of IndexDocument containing document details.
     */
    std::vector<IndexDocument> SearcherDB::getDocumentsByTerm(std::string term){
        // Access the database and the collection
        auto db = this->client.get()->database("AsuraCrow_DB");
        auto indexDocuments = db.collection("index");

        std::vector<IndexDocument> result;

        // Build the filter for finding documents by term
        auto filter = bsoncxx::builder::stream::document{} << "term" << term << bsoncxx::builder::stream::finalize;

        // Set find options to project only the "documents" field
        mongocxx::options::find findOpts{};
        findOpts.projection(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("documents", 1)));

        // Execute the find query
        auto cursor = indexDocuments.find_one(filter.view(), findOpts);

        // No match found for the term, return empty vector
        if (!cursor) {
            return result;
        }

        // Extract the array of documents associated with the term
        auto termDoc = *cursor;
        auto documents_array = termDoc["documents"].get_array().value;

        // Iterate through the documents array and populate the result vector
        for(const auto& docVal: documents_array){
            auto doc = docVal.get_document().value;
            if (doc.find("url") != doc.end() && doc.find("tf") != doc.end() && doc.find("docLength") != doc.end()) {
                IndexDocument index_doc;
                index_doc.url = doc["url"].get_string().value.to_string();
                index_doc.tf = doc["tf"].get_double().value;
                index_doc.docLength = doc["docLength"].get_int32().value;

                result.push_back(index_doc);
            }
        }

        return result;
    }

    /**
     * @brief Retrieves the total number of documents in the "websites" collection of the database.
     * 
     * @return int The total number of documents.
     */
    int SearcherDB::getTotalNumberDocuments(){
        // Access the database and the collection
        auto db = this->client.get()->database("AsuraCrow_DB");
        auto websiteDocuments = db.collection("websites");

        // Set find options to project only the "_id" field, only the count is relevant
        mongocxx::options::find options{};
        options.projection(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", 1)));

        // Execute the find query
        auto cursor = websiteDocuments.find({}, options);

        int size = 0;

        // Iterate through the cursor to count the total documents
        for(auto doc : cursor){
            size++;
        }

        return size;
    }
}
