#include "db/db.hpp"
#include <iostream>
#include <string>
#include <stdio.h>


namespace indexer_db {

    IndexerDB::IndexerDB() {
        std::string dbPath="./index.db";
        int rc=sqlite3_open(dbPath.c_str(),&this->db);

        if(rc){
            fprintf(stderr,"Can't open database: %s\n",sqlite3_errmsg(this->db));
            return;
        }

        createTables();
    }

    IndexerDB::~IndexerDB() {
         if (this->db) {
            sqlite3_close(this->db);
        }
    }

    void IndexerDB::insertIndexDocument(const IndexDocument& document, std::string term) {
        //dump in the term
        std::string sqlTerm = "INSERT INTO indexTerms (term) VALUES (?)";

        sqlite3_stmt *stmtTerm;
        int rcTerm = sqlite3_prepare_v2(this->db, sqlTerm.c_str(), -1, &stmtTerm, nullptr);
        if (rcTerm != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(this->db) << std::endl;
            return;
        }

        sqlite3_bind_text(stmtTerm, 1, term.c_str(), -1, SQLITE_STATIC);

        sqlite3_step(stmtTerm);

        // finalize the statement to free up resources
        sqlite3_finalize(stmtTerm);


        // sql statement for document
        std::string sql = "INSERT INTO indexDocuments (term, websiteUrl, termCount, tf, idf) VALUES (?,?, ?, ?, ?);";

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(this->db) << std::endl;
            return;
        }
        
        // bind parameters to the prepared statement
        sqlite3_bind_text(stmt, 1, term.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, document.url.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, document.termCount);
        sqlite3_bind_double(stmt, 4, document.tf);
        sqlite3_bind_double(stmt, 5, document.idf);

        // Execute the prepared statement
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Error inserting document: " << sqlite3_errmsg(this->db) << std::endl;
        }

        // finalize the statement to free up resources
        sqlite3_finalize(stmt);
    }

    void IndexerDB::createTables(){
        std::string sqlTables = "CREATE TABLE IF NOT EXISTS indexTerms ("
                                "term TEXT PRIMARY KEY);"
                                "CREATE TABLE IF NOT EXISTS indexDocuments ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "term TEXT NOT NULL, "
                                "websiteUrl TEXT,"
                                "termCount INTEGER, "
                                "tf REAL, "
                                "idf REAL,"
                                "foreign key (term) references indexTerms(term) );";

        char* errMsg = nullptr;

        int rc=sqlite3_exec(this->db,sqlTables.c_str(),nullptr,nullptr,&errMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "Error creating tables: " << sqlite3_errmsg(this->db) << std::endl;
            return;
        }
    }

} // namespace indexer_db
