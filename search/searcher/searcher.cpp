#include <searcher/searcher.hpp>
#include <sstream>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <cmath>

namespace searcher{

    Searcher::Searcher(){
        this->db=std::make_shared<searcher_db::SearcherDB>();
    }
    
    std::vector<std::string> Searcher::search(std::string query){
        std::vector<std::string> querySegments;
        this->splitQuery(query,querySegments,'+');

        int totalDocuments=this->db->getTotalNumberDocuments();

        for(std::string& term: querySegments){
            std::vector<searcher_db::IndexDocument> documents = this->db->getDocumentsByTerm(term);
            float idf=calculateIDF_Score(totalDocuments,documents.size());

            //tuning params for bm25
            float k1 = 1.2;            
            float b = 0.75;

            for(auto doc: documents){
            
                float td_idf=doc.tf*idf;
                float bm25=calculateBM25_Score(doc.docLength, 0, idf, doc.tf, k1, b);
 
                this->rank[doc.url].td_idf+=td_idf;
                this->rank[doc.url].bm25+=bm25;
            }
        }
        std::vector<std::string> resultUrls={"https://apple.com","https://microsoft.com"};
        return resultUrls;
    }

    void Searcher::splitQuery(const std::string& str, std::vector<std::string> segments, char delimiter){
        std::istringstream isstream(str);
        std::string segment;
        while (std::getline(isstream, segment, delimiter)) {
            segments.push_back(segment);
        }
    }

    float Searcher::calculateIDF_Score(int totalDocuments, int appearences){
        return static_cast<float>(log(totalDocuments/appearences));
    }

    float Searcher::calculateBM25_Score(int doc_length, int avg_doc_length, float idf, float tf, float k1, float b){
        return idf * ((tf * (k1 + 1)) / (tf + k1 * (1 - b + b * (doc_length / avg_doc_length))));
    }

    float Searcher::combineScores(float td_idf, float bm25){
        float weightTD_IDF=0.3;
        float weightBM25=0.7;

        return td_idf*weightTD_IDF+bm25*weightBM25;
    }
}