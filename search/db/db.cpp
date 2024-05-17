#include <db/search_db.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/basic/document.hpp>

namespace searcher_db{

    SearcherDB::SearcherDB(){
        try{
            const std::string uri = "mongodb://root:1234@localhost:27017/";
            this->client = std::make_shared<mongocxx::client>(mongocxx::client{mongocxx::uri(uri)});
        }catch (const std::exception& e) {
            std::cerr << "Error connecting to MongoDB: " << e.what() << std::endl;
        }
    }

    std::vector<IndexDocument> SearcherDB::getDocumentsByTerm(std::string term){
        auto db=this->client.get()->database("AsuraCrow_DB");
        auto indexDocuments=db.collection("index");

        std::vector<IndexDocument> result;

        auto filter = bsoncxx::builder::stream::document{} << "term" << term << bsoncxx::builder::stream::finalize;


        mongocxx::options::find findOpts{};
        findOpts.projection(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("documents", 1)));
        auto cursor = indexDocuments.find_one(filter.view(), findOpts);

        // no match found for the term, return empty vector
        if (!cursor) {
            return result;
        }

        auto termDoc= *cursor;
        auto documents_array = termDoc["documents"].get_array().value;

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

    int SearcherDB::getTotalNumberDocuments(){
        auto db=this->client.get()->database("AsuraCrow_DB");
        auto websiteDocuments=db.collection("websites");

        mongocxx::options::find options{};
        options.projection(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", 1)));

        auto cursor=websiteDocuments.find({},options);

        int size=0;

        for(auto doc:cursor){
            size++;
        }
        std::cout << size << std::endl;
        return size;
    }
}