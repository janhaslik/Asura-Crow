#include "db/db.hpp"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

namespace indexer_db {

IndexerDB::IndexerDB() {
    try {
        std::string uri = "mongodb://root:1234@localhost:27017/";
        mongocxx::instance instance{};
        this->client = std::make_shared<mongocxx::client>(mongocxx::client{mongocxx::uri(uri)}); // Creating a shared_ptr for the database

        // Example data: term1 and term2 with associated documents
        std::unordered_map<std::string, std::vector<IndexDocument>> termDocuments;
        
        // Example document 1
        IndexDocument doc1;
        doc1.url = "document1_url";
        doc1.tf = 0.5;
        doc1.idf = 0.75;
        doc1.tf_idf = 0.375;
        doc1.docLength = 100;
        doc1.avgDocLength = 200;
        
        // Example document 2
        IndexDocument doc2;
        doc2.url = "document2_url";
        doc2.tf = 0.6;
        doc2.idf = 0.8;
        doc2.tf_idf = 0.48;
        doc2.docLength = 120;
        doc2.avgDocLength = 210;
        
        // Insert documents into term1
        termDocuments["term1"].push_back(doc1);
        termDocuments["term1"].push_back(doc2);
        
        // Insert documents into term2
        termDocuments["term2"].push_back(doc1);
        
        // Iterate over termDocuments and insert documents into the collection
        for (const auto& entry : termDocuments) {
            const std::string& term = entry.first;
            const std::vector<IndexDocument>& documents = entry.second;

            // Prepare a BSON array for documents
            bsoncxx::builder::stream::array documents_array_builder;
            for (const auto& doc : documents) {
                insertIndexDocument(doc,term);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error connecting to MongoDB: " << e.what() << std::endl;
    }
}
IndexerDB::~IndexerDB() {
    std::cout << "destruct db" << std::endl;
}


void IndexerDB::insertIndexDocument(const IndexDocument& document, std::string term) {
    // bson document of the index document of the website, prevents duplicate in the if-else block
    bsoncxx::builder::stream::document doc_builder{};
    doc_builder << "url" << document.url;
    doc_builder << "tf" << document.tf;
    doc_builder << "idf" << document.idf;
    doc_builder << "tf-idf" << document.tf_idf;
    doc_builder << "docLength" << document.docLength;
    doc_builder << "avgDocLength" << document.avgDocLength;


    auto db=this->client.get()->database("AsuraCrow_DB");
    auto indexDocuments = db.collection("index");
    
    //check if term already exists
    auto filter = bsoncxx::builder::stream::document{} << "term" << term << bsoncxx::builder::stream::finalize;
    auto termDoc = indexDocuments.find_one(filter.view());

    if (termDoc) {
        auto update = bsoncxx::builder::stream::document{} << "$push" << bsoncxx::builder::stream::open_document << "documents" << doc_builder << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

        indexDocuments.update_one(filter.view(), update.view());
    } else {
        try{
            bsoncxx::builder::stream::array documents_array_builder;
            documents_array_builder << doc_builder;

            bsoncxx::array::value documents_array = documents_array_builder << bsoncxx::builder::stream::finalize;

            bsoncxx::builder::stream::document term_doc_builder{};
            term_doc_builder << "term" << term;
            term_doc_builder << "documents" << documents_array;
            bsoncxx::document::value term_doc = term_doc_builder << bsoncxx::builder::stream::finalize;

            indexDocuments.insert_one(term_doc.view());
        } catch (const std::exception& e) {
            std::cerr << "Error inserting document into MongoDB: " << e.what() << std::endl;
        }
    }
}



std::vector<IndexDocument> IndexerDB::getTermDocuments(std::string term) {
    std::vector<IndexDocument> result;
    try {
        auto db=this->client.get()->database("AsuraCrow_DB");
        auto indexDocuments = db.collection("index");
        auto filter = bsoncxx::builder::stream::document{} << "term" << term << bsoncxx::builder::stream::finalize;

        // Retrieve documents matching the term from the database
        mongocxx::cursor documents = indexDocuments.find(filter.view());

        for (auto document : documents) {
            // Validate the retrieved document before accessing its fields
            if (document["url"] && document["tf"] && document["idf"] && document["tf-idf"] && document["docLength"] && document["avgDocLength"]) {
                IndexDocument doc;
                doc.url = document["url"].get_string().value.to_string();
                doc.tf = document["tf"].get_double().value;
                doc.idf = document["idf"].get_double().value;
                doc.tf_idf = document["tf-idf"].get_double().value;
                doc.docLength = document["docLength"].get_int64().value;
                doc.avgDocLength = document["avgDocLength"].get_int64().value;
                result.push_back(doc);
            } else {
                std::cerr << "Error: Invalid document retrieved from MongoDB." << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving documents from MongoDB: " << e.what() << std::endl;
    }
    return result;
}

}