#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <string>
#include <vector>
#include <db/searchdb.hpp>
#include <algorithm>
#include <unordered_map>
#include <memory>

namespace searcher {

    /**
     * @struct DocumentScores
     * @brief Structure to hold various scoring metrics for a document.
     * 
     * This structure holds the TF-IDF score, BM25 score, and the total combined score
     * for a document.
     */
    struct DocumentScores {
        float td_idf = 0;    ///< Term Frequency-Inverse Document Frequency score
        float bm25 = 0;      ///< BM25 score
        float totalScore;    ///< Combined total score
    };

    /**
     * @class Searcher
     * @brief A class to perform search operations on documents.
     * 
     * This class provides functionality to search documents based on a query string.
     * It uses TF-IDF and BM25 scoring methods to rank the documents.
     */
    class Searcher {
    public:
        /**
         * @brief Default constructor for the Searcher class.
         * 
         * Initializes the database connection and other necessary components.
         */
        Searcher();

        /**
         * @brief Searches for documents matching the query string.
         * 
         * @param query The search query string.
         * @return A vector of URLs to the documents ranked by relevance.
         */
        std::vector<std::string> search(std::string query);

    private:
        std::unordered_map<std::string, DocumentScores> rank; ///< Map to hold document URLs and their scores.
        std::shared_ptr<searcher_db::SearcherDB> db; ///< Shared pointer to the database object.

        /**
         * @brief Splits the query string into segments based on a delimiter.
         * 
         * @param str The query string to be split.
         * @param segments The vector to store the split segments.
         * @param delimiter The character used to split the string.
         */
        void splitQuery(const std::string& str, std::vector<std::string>& segments, char delimiter);

        /**
         * @brief Combines TF-IDF and BM25 scores into a total score.
         * 
         * @param td_idf The TF-IDF score.
         * @param bm25 The BM25 score.
         * @return The combined total score.
         */
        float combineScores(float td_idf, float bm25);

        /**
         * @brief Calculates the IDF (Inverse Document Frequency) score.
         * 
         * @param totalDocuments The total number of documents in the database.
         * @param appearances The number of documents in which the term appears.
         * @return The IDF score.
         */
        float calculateIDF_Score(int totalDocuments, int appearances);

        /**
         * @brief Calculates the BM25 score for a term in a document.
         * 
         * @param doc_length The length of the document.
         * @param avg_doc_length The average document length in the corpus.
         * @param idf The IDF score for the term.
         * @param tf The term frequency in the document.
         * @param k1 The BM25 k1 parameter.
         * @param b The BM25 b parameter.
         * @return The BM25 score.
         */
        float calculateBM25_Score(int doc_length, int avg_doc_length, float idf, float tf, float k1, float b);

        /**
         * @brief Comparator function to sort documents based on their scores.
         * 
         * @param a The first document and its scores.
         * @param b The second document and its scores.
         * @return True if the score of document a is greater than the score of document b.
         */
        static bool cmp(const std::pair<std::string, DocumentScores>& a, const std::pair<std::string, DocumentScores>& b);
    };

}

#endif
