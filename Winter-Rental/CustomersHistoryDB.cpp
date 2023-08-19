#include "CustomersHistoryDB.h"

//void CustomersHistoryDB::initTable(Data* data, std::string dbName)
//{
//    std::string createTableQuery;
//    createTableQuery =
//        "CREATE TABLE IF NOT EXISTS CustomersHistoryDB \
//        (id INTEGER PRIMARY KEY,\
//        FirstName VARCHAR(50) NOT NULL, LastName VARCHAR(50) NOT NULL,\
//         phoneNumber VARCHAR(16) NOT NULL, address VARCHAR(255) NOT NULL,\
//        date_time DATETIME, equipmentReturnDate DATETIME, rentalTime DOUBLE,  \
//        supplement DOUBLE, ski_id INTEGER, poles_id INTEGER)";
//    data->rc = sqlite3_exec(data->db, createTableQuery.c_str(), nullptr, nullptr, nullptr);
//    if (data->rc != SQLITE_OK)
//    {
//        std::cout << "Blad przy tworzeniu tabeli: " << sqlite3_errmsg(data->db) << std::endl;
//        sqlite3_close(data->db);
//        return;
//    }
//}

CustomersHistoryDB::CustomersHistoryDB(Data* data)
{
    Customers::initTable(data, "CustomersHistoryDB");
}

CustomersHistoryDB::~CustomersHistoryDB()
{
}

//void CustomersHistoryDB::insertData(sqlite3* db, const int ski_id, const int poles_id, std::string firstName, std::string lastName, std::string phoneNumber, std::string address, double rentalTime)
//{
//    std::string insertQuery =
//        "INSERT INTO CustomersHistoryDB\
//         (FirstName, LastName,\
//          phoneNumber, address,\
//         date_time, equipmentReturnDate, rentalTime,\
//         supplement, ski_id, poles_id) VALUES\
//        ('" + firstName + "', '" + lastName + "'," + \
//        "'" + phoneNumber + "', '" + address + \
//        "', CURRENT_TIMESTAMP, NULL, " + std::to_string(rentalTime) + \
//        ", NULL, " + std::to_string(ski_id) + ", " + std::to_string(poles_id) + ")";
//    int rc = sqlite3_exec(db, insertQuery.c_str(), nullptr, nullptr, nullptr);
//
//    if (rc != SQLITE_OK)
//    {
//        std::cout << "Blad przy wstawianiu danych: " << sqlite3_errmsg(db) << std::endl;
//        return;
//    }
//    else {
//        std::cout << "Dane zostaly wstawione poprawnie." << std::endl;
//    }
//}
//
//void CustomersHistoryDB::deleteData(sqlite3* db, int id)
//{
//    std::string selectQuery = "SELECT * FROM CustomersHistoryDB WHERE id = " + std::to_string(id);
//    sqlite3_stmt* selectStatement;
//    int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &selectStatement, nullptr);
//
//    if (rc != SQLITE_OK)
//    {
//        std::cout << "Blad przy przygotowywaniu zapytania: " << sqlite3_errmsg(db) << std::endl;
//        return;
//    }
//
//    rc = sqlite3_step(selectStatement);
//    if (rc == SQLITE_ROW)
//    {
//        std::string deleteQuery = "DELETE FROM CustomersHistoryDB WHERE id = " + std::to_string(id);
//        int deleteResult = sqlite3_exec(db, deleteQuery.c_str(), nullptr, nullptr, nullptr);
//
//        if (deleteResult != SQLITE_OK) {
//            std::cout << "Blad przy usuwaniu danych: " << sqlite3_errmsg(db) << std::endl;
//            return;
//        }
//        else {
//            std::cout << "Dane zostaly usuniete poprawnie." << std::endl;
//        }
//    }
//    else
//    {
//        std::cout << "Nie ma takiego rekordu o ID: " << id << std::endl;
//    }
//
//    sqlite3_finalize(selectStatement);
//}
//
//void CustomersHistoryDB::displayData(sqlite3* db)
//{
//    std::string selectQuery = "SELECT * FROM CustomersHistoryDB";
//    sqlite3_stmt* statement;
//    int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &statement, nullptr);
//
//    if (rc != SQLITE_OK)
//    {
//        std::cout << "Blad przy przygotowywaniu zapytania: " << sqlite3_errmsg(db) << std::endl;
//        return;
//    }
//
//    std::cout << "|_" << std::right << std::setfill('*') << std::setw(50) << "_ | " << std::endl;
//    std::cout << "|_ " << std::right << std::setfill(' ') << std::setw(28) << "CUSTOMER LIST"
//        << std::right << std::setfill(' ') << std::setw(16) << "_ | " << std::endl;
//    std::cout << "|_" << std::right << std::setfill('*') << std::setw(50) << "_ | " << std::endl;
//
//    std::cout << std::setfill('_') << std::setw(199) << std::left << " _" << "_" << std::endl
//        << " | " << std::setfill(' ') << std::setw(5) << std::left << "ID"
//        << " | " << std::setw(20) << "Name"
//        << " | " << std::setw(20) << "Surname"
//        << " | " << std::setw(15) << "Phone number"
//        << " | " << std::setw(30) << "Address"
//        << " | " << std::setw(20) << "Start"
//        << " | " << std::setw(20) << "End"
//        << " | " << std::setw(10) << "RentalTime"
//        << " | " << std::setw(10) << "Supplement"
//        << " | " << std::setw(10) << "Ski id"
//        << " | " << std::setw(10) << "Poles id"
//        << " | " << std::endl
//        << std::setfill('-') << std::setw(199) << std::left << " |" << "|" << std::endl;
//
//    while (sqlite3_step(statement) == SQLITE_ROW)
//    {
//        int id = sqlite3_column_int(statement, 0);
//        std::string FirstName(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
//        std::string LastName(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));
//        std::string PhoneNumber(reinterpret_cast<const char*>(sqlite3_column_text(statement, 3)));
//        std::string Address(reinterpret_cast<const char*>(sqlite3_column_text(statement, 4)));
//        std::string startData(reinterpret_cast<const char*>(sqlite3_column_text(statement, 5)));
//        const char* endDataText = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
//        std::string endData = (endDataText != nullptr) ? endDataText : "NULL";
//        int rentalTime = sqlite3_column_int(statement, 7);
//        const char* supplementText = reinterpret_cast<const char*>(sqlite3_column_text(statement, 8));
//        std::string supplement = (supplementText != nullptr) ? ((std::stoi(supplementText) < 0) ? "0" : supplementText) : "NULL";
//
//        int ski_id = sqlite3_column_int(statement, 9);
//        std::string s_id = (ski_id != 0) ? std::to_string(ski_id) : "NULL";
//
//        int poles_id = sqlite3_column_int(statement, 10);
//        std::string p_id = (poles_id != 0) ? std::to_string(poles_id) : "NULL";
//
//        std::cout << " | " << std::setfill(' ') << std::setw(5) << std::left << id
//            << " | " << std::setw(20) << FirstName
//            << " | " << std::setw(20) << LastName
//            << " | " << std::setw(15) << PhoneNumber
//            << " | " << std::setw(30) << Address
//            << " | " << std::setw(20) << startData
//            << " | " << std::setw(20) << endData
//            << " | " << std::setw(10) << rentalTime
//            << " | " << std::setw(10) << supplement
//            << " | " << std::setw(10) << s_id
//            << " | " << std::setw(10) << p_id
//            << " | " << std::endl;
//    }
//    std::cout << std::setfill('_') << std::setw(199) << std::left << " |" << "|" << std::endl;
//
//    sqlite3_finalize(statement);
//}

void CustomersHistoryDB::deleteTable(sqlite3* db)
{
    std::string confirmation;
    std::cout << "Wpisz 'delete Customer History Table', aby potwierdziæ usuniêcie tabeli: ";
    std::getline(std::cin, confirmation);

    // Usuniêcie bia³ych znaków z wprowadzonej frazy
    confirmation.erase(std::remove_if(confirmation.begin(), confirmation.end(), ::isspace), confirmation.end());

    if (confirmation != "deleteCustomerHistoryTable")
    {
        std::cout << "Nieprawid³owa fraza potwierdzaj¹ca. Nie usuniêto tabeli." << std::endl;
        return;
    }
    std::string query = "DROP TABLE IF EXISTS CustomersHistoryDB";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "B³¹d wykonania zapytania: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}


