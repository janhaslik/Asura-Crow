# Asura Crow: Search Engine

Author: [Jan Haslik](https://github.com/janhaslik)

Asura Crow is a project aimed at replicating the functionality of a basic search engine. It involves crawling, indexing, and searching functionalities implemented using C++, Go, and MongoDB. Various algorithms such as TF-IDF and BM25 are applied to enhance the search results. 

## Components

#### Crawler
- **Description**: Responsible for crawling web pages using Go and saving the extracted information into MongoDB for indexing.

#### Indexer
- **Description**: Implemented in C++, the indexer accesses MongoDB to perform indexing of crawled data and stores it efficiently for quick retrieval during searches.

#### Searcher
- **Description**: Accesses the MongoDB database and delivers relevant information to the query using various algorithms like TF-IDF and BM25.