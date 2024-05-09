#include <db/db.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/bulk_write.hpp>
#include <iostream>

namespace indexer_db {

    IndexerDB::IndexerDB() {
        try {
            const std::string uri = "mongodb://root:1234@localhost:27017/";
             // creating a shared_ptr for the database
            this->client = std::make_shared<mongocxx::client>(mongocxx::client{mongocxx::uri(uri)});

            std::unordered_map<std::string, std::vector<IndexDocument>> termDocuments;
            
            IndexDocument doc1;
            doc1.url = "document1_url";
            doc1.tf = 0.8;
            doc1.docLength = 100;
            
            IndexDocument doc2;
            doc2.url = "document2_url";
            doc2.tf = 0.6;
            doc2.docLength = 120;
            
            termDocuments["term1"].push_back(doc1);
            termDocuments["term1"].push_back(doc2);
            doc1.tf=0.4;
            termDocuments["term2"].push_back(doc1);
            
            for (const auto& entry : termDocuments) {
                const std::string& term = entry.first;
                const std::vector<IndexDocument>& documents = entry.second;

                bsoncxx::builder::stream::array documents_array_builder;
                for (const auto& doc : documents) {
                    upsertIndexDocument(doc,term);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error connecting to MongoDB: " << e.what() << std::endl;
        }
    }

    IndexerDB::~IndexerDB() {
    }


   void IndexerDB::upsertIndexDocument(const IndexDocument& document, std::string term) {
        try {
            bsoncxx::builder::stream::document doc_builder{};
            doc_builder << "url" << document.url
                        << "tf" << document.tf
                        << "docLength" << document.docLength;

            auto db = this->client.get()->database("AsuraCrow_DB");
            auto indexDocuments = db.collection("index");

            auto documents=this->getTermDocuments(term);

            //check for updating a document
            bool found=false;
            for(auto &d: documents){
                if(d.url==document.url){
                    d=document;
                    found=true;
                    break;
                }
            }

            //if new, just push to documents
            if(!found)documents.push_back(document);

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



    std::vector<IndexDocument> IndexerDB::getTermDocuments(std::string term) {
        std::vector<IndexDocument> result;
        try {
            auto db = this->client.get()->database("AsuraCrow_DB");
            auto indexDocuments = db.collection("index");
            auto filter = bsoncxx::builder::stream::document{} << "term" << term << bsoncxx::builder::stream::finalize;

            // Define projection option
            mongocxx::options::find findOpts{};
            findOpts.projection(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("documents", 1)));

            // retrieve one index document matching the term from the database
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