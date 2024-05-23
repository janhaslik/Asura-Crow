#include <searcher/searcher.hpp>
#include <sstream>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <cmath>

namespace searcher{

    /**
     * @brief Default constructor for the Searcher class.
     * 
     * Initializes the database connection using a shared pointer to IndexerDB.
     */
    Searcher::Searcher(){
        this->db = std::make_shared<searcher_db::SearcherDB>();
    }

    // Comparator function to sort documents based on their total scores in descending order
    bool Searcher::cmp(const std::pair<std::string, DocumentScores>& a, const std::pair<std::string, DocumentScores>& b){
        return a.second.totalScore > b.second.totalScore;
    }

    /**
     * @brief Searches the database for documents matching the query.
     * 
     * @param query The search query string.
     * @return std::vector<std::string> A list of URLs of the top-ranked documents.
     */
    std::vector<std::string> Searcher::search(std::string query){
        // Split the query into individual terms
        std::vector<std::string> querySegments;
        this->splitQuery(query, querySegments, '+');

        // Get the total number of documents in the database
        int totalDocuments = this->db->getTotalNumberDocuments();

        // Calculate the score for relevant documents
        for(std::string& term: querySegments){
            std::vector<searcher_db::IndexDocument> documents = this->db->getDocumentsByTerm(term);

            // Tuning parameters for BM25
            float k1 = 1.2;
            float b = 0.75;

            // Calculate the inverse document frequency (IDF) score
            float idf = calculateIDF_Score(totalDocuments, documents.size());

            for(auto doc: documents){
                // Calculate term frequency-inverse document frequency (TF-IDF) score
                float td_idf = doc.tf * idf;
                // Calculate BM25 score
                float bm25 = calculateBM25_Score(doc.docLength, 0, idf, doc.tf, k1, b);

                // Update document scores in the ranking map
                this->rank[doc.url].td_idf += td_idf;
                this->rank[doc.url].bm25 += bm25;
                this->rank[doc.url].totalScore += combineScores(this->rank[doc.url].td_idf, this->rank[doc.url].bm25);
            }
        }

        // Sort documents by their total scores
        std::vector<std::pair<std::string, DocumentScores>> sorted_documents(rank.begin(), rank.end());
        std::sort(sorted_documents.begin(), sorted_documents.end(), cmp);

        // Prepare the result URLs
        std::vector<std::string> resultUrls;
        int counter = 0;
        for(const auto& it: sorted_documents){
            if(counter > 25) break;
            resultUrls.push_back(it.first);
            counter++;
        }
        return resultUrls;
    }

    /**
     * @brief Splits a query string into segments based on a delimiter.
     * 
     * @param str The query string to split.
     * @param segments The vector to store the resulting segments.
     * @param delimiter The character used to split the query string.
     */
    void Searcher::splitQuery(const std::string& str, std::vector<std::string>& segments, char delimiter){
        std::istringstream isstream(str);
        std::string segment;
        while (std::getline(isstream, segment, delimiter)) {
            segments.push_back(segment);
        }
    }

    /**
     * @brief Calculates the Inverse Document Frequency (IDF) score.
     * 
     * @param totalDocuments The total number of documents in the database.
     * @param appearances The number of documents in which the term appears.
     * @return float The IDF score.
     */
    float Searcher::calculateIDF_Score(int totalDocuments, int appearances){
        return static_cast<float>(1 + log(totalDocuments / appearances));
    }

    /**
     * @brief Calculates the BM25 score for a document.
     * 
     * @param doc_length The length of the document.
     * @param avg_doc_length The average length of documents (not used in this implementation).
     * @param idf The IDF score of the term.
     * @param tf The term frequency in the document.
     * @param k1 The BM25 k1 parameter.
     * @param b The BM25 b parameter.
     * @return float The BM25 score.
     */
    float Searcher::calculateBM25_Score(int doc_length, int avg_doc_length, float idf, float tf, float k1, float b){
        return idf * ((tf * (k1 + 1)) / (tf + k1 * (1 - b + b * (doc_length / avg_doc_length))));
    }

    /**
     * @brief Combines TD-IDF and BM25 scores to produce a total score for ranking.
     * 
     * @param td_idf The TD-IDF score.
     * @param bm25 The BM25 score.
     * @return float The combined total score.
     */
    float Searcher::combineScores(float td_idf, float bm25){
        // Weights for the ranking components
        float weightTD_IDF = 0.3;
        float weightBM25 = 0.7;

        return td_idf * weightTD_IDF + bm25 * weightBM25;
    }
}
