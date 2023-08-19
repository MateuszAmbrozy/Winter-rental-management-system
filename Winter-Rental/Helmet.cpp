#include "stdafx.h"
#include "Helmet.h"

void Helmet::initTable(Date* date, const std::string& dbName)
{
    std::string createTableQuery = "CREATE TABLE IF NOT EXISTS " + dbName + " (id INTEGER PRIMARY KEY, stock_count INTEGER, size TEXT NOT NULL)";
    if (sqlite3_exec(date->db, createTableQuery.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cout << "Blad przy tworzeniu tabeli: " << sqlite3_errmsg(date->db) << std::endl;
        return;
    }
}

void Helmet::insertDate(sqlite3* db, const std::string& dbName, const int stock_count, const int minLength)
{
    if (this->isTableEmpty(db, dbName))
    {
        std::vector<std::string> sizes = { "XS", "S", "M", "L", "XL" };
        for (const auto& size : sizes)
        {
            std::string insertQuery = "INSERT INTO " + dbName + " (stock_count, size) VALUES(?, ?);";
            sqlite3_stmt* stmt;

            int rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK)
            {
                std::cout << "ERROR preparing: " << sqlite3_errmsg(db) << std::endl;
                return;
            }

            // Bind parameters
            sqlite3_bind_int(stmt, 1, stock_count);
            sqlite3_bind_text(stmt, 2, size.c_str(), -1, SQLITE_STATIC);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE)
            {
                std::cout << "ERROR inserting: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                return;
            }

            sqlite3_finalize(stmt);
        }
    }
}



Helmet::Helmet(Date* date)
	:Gear(date)
{
    this->initTable(date, "Helmets");
    this->insertDate(date->db, "Helmets", 15, 0);
}
Helmet::~Helmet()
{
}

void Helmet::displayDate(sqlite3* db, const std::string& dbName)
{
        std::string selectQuery = "SELECT * FROM " + dbName;
    sqlite3_stmt* statement;
    int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &statement, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    std::cout << "ID\tSTOCK COUNT\tSIZE" << std::endl;

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(statement, 0);
        int stock_count = sqlite3_column_int(statement, 1);
        std::string size(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));

        std::cout << id << "\t" << stock_count << "\t\t" << size << std::endl;
    }

    sqlite3_finalize(statement);
}

void Helmet::addGear(sqlite3* db, const std::string& dbName)
{
    std::string size;
    int gear = io_read<int>("Enter how many gear do you want to add: ", "Invalid input. Please try again");
    std::cout << "Enter size of Helmet: ";
    std::cin >> size;

    std::string selectQuery = "SELECT * FROM " + dbName + " WHERE size = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, size.c_str(), -1, SQLITE_STATIC);

    if (rc != SQLITE_OK)
    {
        std::cout << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        int stockCount = sqlite3_column_int(stmt, 1);
        stockCount += gear;

        std::string updateQuery = "UPDATE " + dbName + " SET stock_count = ? WHERE size = ?";
        sqlite3_stmt* updateStmt;
        rc = sqlite3_prepare_v2(db, updateQuery.c_str(), -1, &updateStmt, nullptr);

        if (rc != SQLITE_OK)
        {
            std::cout << "Error preparing update SQL statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_finalize(updateStmt);
            return;
        }

        sqlite3_bind_int(updateStmt, 1, stockCount);
        sqlite3_bind_text(updateStmt, 2, size.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(updateStmt);
        if (rc != SQLITE_DONE)
        {
            std::cout << "Error updating date: " << sqlite3_errmsg(db) << std::endl;
        }
        else
        {
            std::cout << "Stock count updated successfully." << std::endl;
        }

        sqlite3_finalize(updateStmt);
    }
    else if (rc != SQLITE_ROW) // This condition ensures insertion only occurs if the size wasn't found.
    {
        std::string insertQuery = "INSERT INTO " + dbName + " (stock_count, size) VALUES(?, ?)";
        sqlite3_stmt* insertStmt;
        rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &insertStmt, nullptr);

        if (rc != SQLITE_OK)
        {
            std::cout << "Error preparing insert SQL statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(insertStmt);
            return;
        }

        sqlite3_bind_int(insertStmt, 1, gear);
        sqlite3_bind_text(insertStmt, 2, size.c_str(), -1, SQLITE_STATIC); // Corrected from stmt to insertStmt.

        rc = sqlite3_step(insertStmt);
        if (rc != SQLITE_DONE)
        {
            std::cout << "Error inserting data: " << sqlite3_errmsg(db) << std::endl;
        }
        else
        {
            std::cout << "New " + dbName + " record added successfully." << std::endl;
        }

        sqlite3_finalize(insertStmt);
    }

    sqlite3_finalize(stmt);
}


void Helmet::removeGear(sqlite3* db, const std::string& dbName)
{
    std::string size;
    int gearToRemove = io_read<int>("Enter how many gear do you want to remove: ", "Invalid input. Please try again");
    std::cout << "Enter size of Helmet: ";
    std::cin >> size;

    this->querry = "SELECT stock_count FROM " + dbName + " WHERE size = ?;";
    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, size.c_str(), -1, SQLITE_STATIC);

    if (this->rc != SQLITE_OK)
    {
        std::cout << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return;
    }

    this->rc = sqlite3_step(this->stmt);
    if (this->rc == SQLITE_ROW)
    {
        int stockCount = sqlite3_column_int(this->stmt, 0);
        sqlite3_finalize(stmt);  // finalize the previous stmt before reusing the variable

        if (stockCount >= gearToRemove)
        {
            stockCount -= gearToRemove;
            std::string updateQuery = "UPDATE " + dbName + " SET stock_count = ? WHERE size = ?;";
            this->rc = sqlite3_prepare_v2(db, updateQuery.c_str(), -1, &this->stmt, nullptr);

            sqlite3_bind_int(this->stmt, 1, stockCount);
            sqlite3_bind_text(stmt, 2, size.c_str(), -1, SQLITE_STATIC);

            this->rc = sqlite3_step(this->stmt);
            if (this->rc != SQLITE_DONE)
            {
                std::cout << "Error updating data: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                return;
            }

            std::cout << "Stock count updated successfully." << std::endl;
        }
        else
        {
            std::cout << "Insufficient stock of " + dbName + " for the given size." << std::endl;
        }
    }
    else
    {
        std::cout << "No " + dbName + " found for the given size." << std::endl;
    }

    sqlite3_finalize(stmt);
}