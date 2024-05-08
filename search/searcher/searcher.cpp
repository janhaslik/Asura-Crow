#include <searcher/searcher.hpp>
#include <sstream>

namespace searcher{

    Searcher::Searcher(){

    }
    
    std::vector<std::string> Searcher::search(std::string query){
        std::vector<std::string> querySegments;
        this->splitQuery(query,querySegments,'+');


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
}