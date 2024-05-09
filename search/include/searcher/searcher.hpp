#ifndef SEARCHER_HPP
#define SEARCHER_HPP
#include <string>
#include <vector>
#include <db/db.hpp>

namespace searcher{

    struct DocumentScores{
        float td_idf=0;
        float bm25=0;
    };

    class Searcher{
        public:
            Searcher();
            std::vector<std::string> search(std::string query);
        private:
            //key: url; value: scores
            std::unordered_map<std::string, DocumentScores> rank;
            std::shared_ptr<searcher_db::SearcherDB> db;
            void splitQuery(const std::string& str, std::vector<std::string> segments, char delimiter);
            float combineScores(float td_idf, float bm25);
            float calculateIDF_Score(int totalDocuments, int appearences);
            float calculateBM25_Score(int doc_length, int avg_doc_length,float idf, float tf, float k1, float b);
    };
}


#endif