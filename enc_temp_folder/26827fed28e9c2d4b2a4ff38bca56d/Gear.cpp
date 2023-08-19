#include "stdafx.h"
#include "Gear.h"

void Gear::initTable(Date* date, const std::string& dbName)
{
    this->querry = "CREATE TABLE IF NOT EXISTS " + dbName + " (id INTEGER PRIMARY KEY, stock_count INTEGER, size TEXT NOT NULL);";
    if (sqlite3_exec(date->db, this->querry.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cout << "Error while creating the table: " << sqlite3_errmsg(date->db) << std::endl;
        return;
    }
}

Gear::Gear(Date* date)
    : dbName(""),
    querry(""),
    stmt(nullptr),
    rc(0)
{
    this->countReservation = 0;
}

Gear::~Gear()
{
}

void Gear::insertDate(sqlite3* db, const std::string& dbName, const int stock_count, const  int minSize)
{
    if (this->isTableEmpty(db, dbName))
    {
        this->querry = "INSERT INTO " + dbName + " (stock_count, size) VALUES(?, ?)";

        for (int i = 0; i < 15; i++)
        {
            if (sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr) != SQLITE_OK)
            {
                std::cout << "ERROR::GEAR::Coult not prepare querry " << sqlite3_errmsg(db) << std::endl;
                return;
            }

            sqlite3_bind_int(this->stmt, 1, stock_count);
            sqlite3_bind_int(this->stmt, 2, minSize + i * 5);

            rc = sqlite3_step(this->stmt);
            if (rc != SQLITE_DONE)
            {
                std::cout << "ERROR::GEAR::INSERTING DATA: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(this->stmt);
                return;
            }

            sqlite3_finalize(this->stmt);
        }
    }
}

void Gear::addGear(sqlite3* db, const std::string& dbName)
{
    int gear = io_read<int>("Enter how many gear do you want to add: ", "Invalid input. Please try again");
    int size = io_read<int>("Enter size of gear: ", "Invalid input. Please try again");

    std::string selectQuery = "SELECT * FROM " + dbName + " WHERE size = ?";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, size);

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
        sqlite3_bind_int(updateStmt, 2, size);

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
    else if (rc != SQLITE_ROW)  // This condition ensures insertion only occurs if the size wasn't found.
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
        sqlite3_bind_int(insertStmt, 2, size);

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


void Gear::removeGear(sqlite3* db, const std::string& dbName)
{
    int gearToRemove = io_read<int>("Enter how many gear do you want to remove: ", "Invalid input. Please try again");
    int size = io_read<int>("Enter size of gear: ", "Invalid input. Please try again");

    this->querry = "SELECT stock_count FROM " + dbName + " WHERE size = ?;";
    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_int(this->stmt, 1, size);

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
            sqlite3_bind_int(this->stmt, 2, size);

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
void Gear::returnGear(sqlite3* db, const std::string& dbName, const int gearID, Gear* gear)
{
    std::string name = dbName + "_id";

    // Zwiêkszenie liczby wypo¿yczonego wyposa¿enia przy zwrocie
    this->querry = "SELECT " + name + " FROM Rentals WHERE customer_id = ?;";
    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr);

    if (this->rc != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt); // finalize the stmt in case of error
        return;
    }

    sqlite3_bind_int(this->stmt, 1, gearID);  // Bindowanie wartoœci gearID do placeholdera

    std::vector<int> ids;  // Przechowuje pobrane gear_id

    while ((rc = sqlite3_step(this->stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(this->stmt, 0);
        ids.push_back(id);
    }

    if (rc != SQLITE_DONE)
    {
        std::cout << "SQL step error: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(this->stmt);

    for (int id : ids)
    {
        gear->updateStockCount(db, 1, id, dbName);
        std::cout << dbName + " id: " << id << std::endl;
    }
}

void Gear::displayDate(sqlite3* db, const std::string& dbName)
{
    this->querry = "SELECT * FROM " + dbName;
    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr);

    if (this->rc != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);  // Upewnij siê, ¿e zwalniasz statement nawet w przypadku b³êdu
        return;
    }

    std::cout << "ID\tSTOCK COUNT\t\tSIZE" << std::endl;  // Poprawi³em literówkê "STOCK CONT" na "STOCK COUNT"

    while ((this->rc = sqlite3_step(this->stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(this->stmt, 0);
        int stock_count = sqlite3_column_int(this->stmt, 1);
        int height = sqlite3_column_int(this->stmt, 2);

        std::cout << id << "\t" << stock_count << "\t\t" << height << std::endl;
    }

    if (this->rc != SQLITE_DONE)
    {
        std::cout << "Error reading row data: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(this->stmt);
}

void Gear::updateStockCount(sqlite3* db, const int value, const int id, const std::string& dbName)
{
    this->querry = "UPDATE " + dbName + " SET stock_count = stock_count + ? WHERE id = ?";
    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr);

    if (this->rc != SQLITE_OK)
    {
        std::cout << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return;
    }

    sqlite3_bind_int(this->stmt, 1, value);
    sqlite3_bind_int(this->stmt, 2, id);

    this->rc = sqlite3_step(stmt);
    if (this->rc != SQLITE_DONE)
    {
        std::cout << "Error updating the counter in the " << dbName << " table: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(this->stmt);
}
int Gear::getStockCount(sqlite3* db, const int gearID, const std::string& dbName)
{
    this->querry = "SELECT stock_count FROM " + dbName + " WHERE id = ?";

    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return -1;
    }

    // Binding the gearID to the placeholder
    sqlite3_bind_int(this->stmt, 1, gearID);

    this->rc = sqlite3_step(this->stmt);
    if (this->rc == SQLITE_ROW)
    {
        int stockCount = sqlite3_column_int(this->stmt, 0);
        if (sqlite3_step(this->stmt) == SQLITE_ROW)
        {
            std::cout << "Warning: More than one row returned for gearID: " << gearID << std::endl;
        }

        sqlite3_finalize(this->stmt);
        return stockCount;
    }
    else
    {
        std::cout << "No data found for gearID: " << gearID << std::endl;
        sqlite3_finalize(this->stmt);
        return -1;
    }
}


bool Gear::isTableEmpty(sqlite3* db, const  std::string& dbName)
{
    this->querry = "SELECT COUNT(*) FROM " + dbName;
    bool result = false;

    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr);
    if (this->rc != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return false;
    }

    this->rc = sqlite3_step(this->stmt);
    if (this->rc == SQLITE_ROW)
    {
        int rowCount = sqlite3_column_int(this->stmt, 0);
        result = rowCount == 0;
    }
    else if (this->rc != SQLITE_DONE)
    {
        std::cout << "Error while executing the query: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(this->stmt);
    return result;
}

int Gear::getGearCountReturnedToReservation(sqlite3* db, const std::string& dbName, const std::string& reservation_date, const int gear_id)
{
    const int ERROR_PREPARE_SQL = -2;
    const int ERROR_EXECUTE_SQL = -3;
    int count = 0;

    std::string nameColumn = dbName + "_id";

    this->querry = "SELECT COUNT(*)\
        FROM Rentals \
        WHERE customer_id IN ( \
        SELECT id \
        FROM Customers \
        WHERE strftime('%s', ?) >= strftime('%s',date_time) \
        AND strftime('%s', ?) <= strftime('%s',date_time, '+' || (rentalTime * 3600) || ' seconds') \
        ) \
        AND " + nameColumn + " = ?;";

    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, NULL);
    if (this->rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return ERROR_PREPARE_SQL;
    }

    sqlite3_bind_text(this->stmt, 1, reservation_date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(this->stmt, 2, reservation_date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(this->stmt, 3, gear_id);

    this->rc = sqlite3_step(this->stmt);
    if (this->rc == SQLITE_ROW)
    {
        count = sqlite3_column_int(this->stmt, 0);
    }
    else
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        count = ERROR_EXECUTE_SQL;
    }

    sqlite3_finalize(this->stmt);
    std::cout << count << std::endl;
    return count;
}

