#ifndef SEARCHER_HPP
#define SEARCHER_HPP
#include <string>
#include <vector>

namespace searcher{
    class Searcher{
        public:
            Searcher();
            std::vector<std::string> search(std::string query);
        private:
            void splitQuery(const std::string& str, std::vector<std::string> segments, char delimiter);
    };
}


#endif