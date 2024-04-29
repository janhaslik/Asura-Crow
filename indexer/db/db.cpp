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
        mongocxx::client client{mongocxx::uri(uri)};
        this->db = client["AsuraCrow_DB"];

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
    }

    void IndexerDB::insertIndexDocument(const IndexDocument& document, std::string term) {
        //bson document of the index document of website
        bsoncxx::builder::stream::document doc_builder{};
        doc_builder << "url" << document.url;
        doc_builder << "tf" << document.tf;
        doc_builder << "idf" << document.idf;
        doc_builder << "tf-idf" << document.tf_idf;
        doc_builder << "docLength" << document.docLength;
        doc_builder << "avgDocLength" << document.avgDocLength;
            
        // Implement insertion logic here
        auto indexDocuments=this->db.collection("index");

        auto filter=bsoncxx::builder::stream::document{} << "term" << term << bsoncxx::builder::stream::finalize;

        auto termDoc=indexDocuments.find_one(filter.view());

        if(termDoc){
            auto update=bsoncxx::builder::stream::document{} << "$push" << bsoncxx::builder::stream::open_document <<  "documents" << doc_builder << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

            indexDocuments.update_one(filter.view(),update.view());

        }else{
            bsoncxx::builder::stream::array documents_array_builder;
            documents_array_builder << doc_builder;

            bsoncxx::array::value documents_array = documents_array_builder << bsoncxx::builder::stream::finalize;

            bsoncxx::builder::stream::document term_doc_builder{};
            term_doc_builder << "term" << term;
            term_doc_builder << "documents" << documents_array;
            bsoncxx::document::value term_doc = term_doc_builder << bsoncxx::builder::stream::finalize;

            indexDocuments.insert_one(term_doc.view());
        }
    }

    std::vector<IndexDocument> IndexerDB::getTermDocuments(std::string term) {
        std::vector<IndexDocument> result;
        try {
            auto indexDocuments = this->db.collection("index");

            auto filter = bsoncxx::builder::stream::document{} << "term" << term
                                                               << bsoncxx::builder::stream::finalize;
            mongocxx::options::find findOptions{};
            findOptions.projection(bsoncxx::builder::stream::document{} << "documents" << "1"
                                                                         << bsoncxx::builder::stream::finalize);

            mongocxx::cursor documents = indexDocuments.find(filter.view(), findOptions);

            for (auto document : documents) {
                IndexDocument doc;
                /*doc.url = document["url"].get_string().value.to_string();
                doc.tf = document["tf"].get_double().value;
                doc.idf = document["idf"].get_double().value;
                doc.tf_idf = document["tf-idf"].get_double().value;
                doc.docLength = document["docLength"].get_int64().value;
                doc.avgDocLength = document["avgDocLength"].get_int64().value;*/
                result.push_back(doc);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error retrieving documents from MongoDB: " << e.what() << std::endl;
        }
        return result;
    }

}