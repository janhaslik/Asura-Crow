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

    bool Searcher::cmp(const std::pair<std::string, DocumentScores>& a, const std::pair<std::string, DocumentScores>& b){
        return a.second.totalScore>b.second.totalScore;
    }

    std::vector<std::string> Searcher::search(std::string query){
        std::vector<std::string> querySegments;
        this->splitQuery(query,querySegments,'+');

        int totalDocuments=this->db->getTotalNumberDocuments();

        //calculate the score for relevant documents
        for(std::string& term: querySegments){
            std::vector<searcher_db::IndexDocument> documents = this->db->getDocumentsByTerm(term);

            //tuning params for bm25
            float k1 = 1.2;//{1.2, 2.0}      
            float b = 0.75;

            //calculate total idf
            float idf=calculateIDF_Score(totalDocuments,documents.size());

            for(auto doc: documents){
                float td_idf=doc.tf*idf;
                float bm25=calculateBM25_Score(doc.docLength, 0, idf, doc.tf, k1, b);
 
                this->rank[doc.url].td_idf+=td_idf;
                this->rank[doc.url].bm25+=bm25;
                this->rank[doc.url].totalScore=combineScores(this->rank[doc.url].td_idf, this->rank[doc.url].bm25);
            }
        }

        std::vector<std::pair<std::string, DocumentScores>> sorted_documents(rank.begin(), rank.end());
        std::sort(sorted_documents.begin(), sorted_documents.end(), cmp);

        std::vector<std::string> resultUrls;

        int counter=0;
        for(const auto& it: sorted_documents){
            if(counter>25)break;
            std::cout << it.first << " " << it.second.totalScore << " " << it.second.td_idf << " " << it.second.bm25 << " " << std::endl;
            resultUrls.push_back(it.first);
        }
        return resultUrls;
    }

    void Searcher::splitQuery(const std::string& str, std::vector<std::string>& segments, char delimiter){
        std::istringstream isstream(str);
        std::string segment;
        while (std::getline(isstream, segment, delimiter)) {
            segments.push_back(segment);
        }
    }

    float Searcher::calculateIDF_Score(int totalDocuments, int appearences){
        return static_cast<float>(1+log(totalDocuments/appearences));
    }

    float Searcher::calculateBM25_Score(int doc_length, int avg_doc_length, float idf, float tf, float k1, float b){
        return idf * ((tf * (k1 + 1)) / (tf + k1 * (1 - b + b * (doc_length / avg_doc_length))));
    }

    float Searcher::combineScores(float td_idf, float bm25){
        //weights for the ranking
        float weightTD_IDF=0.3;
        float weightBM25=0.7;

        return td_idf*weightTD_IDF+bm25*weightBM25;
    }
}