#include "stdafx.h"
#include "Boots.h"

void Boots::insertDate(sqlite3* db, const std::string& dbName, const  int stock_count, const int minSize)
{
    if (this->isTableEmpty(db, dbName))
    {
        sqlite3_stmt* stmt;

        std::string insertQuery = "INSERT INTO " + dbName + " (stock_count, size) VALUES(?, ?)";
        int rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK)
        {
            std::cout << "ERROR::HELMET::INSERTDATA::PREPARE " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        for (int i = 0; i < 15; i++)
        {
            sqlite3_bind_int(stmt, 1, stock_count);
            sqlite3_bind_text(stmt, 2, (std::to_string(32 + i)).c_str(), -1, SQLITE_STATIC);

            rc = sqlite3_step(stmt);

            if (rc != SQLITE_DONE)
            {
                std::cout << "ERROR::HELMET::INSERTDATA::STEP " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                return;
            }

            // Reset the prepared statement for the next iteration
            sqlite3_reset(stmt);
        }

        sqlite3_finalize(stmt);
    }
}

Boots::Boots(Date* date)
    :Gear(date)
{
    this->initTable(date, "Boots");
    this->insertDate(date->db, "Boots", 15, 0);
}
Boots::~Boots()
{
}
