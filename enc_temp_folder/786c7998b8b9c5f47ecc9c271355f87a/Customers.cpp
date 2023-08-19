#include "stdafx.h"
#include "Customers.h" 

void Customers::initTable(Date* date, const std::string& dbName)
{
    this->querry = "CREATE TABLE IF NOT EXISTS " + dbName + "(\
        id INTEGER PRIMARY KEY AUTOINCREMENT,\
        FirstName VARCHAR(50) NOT NULL,\
        LastName VARCHAR(50) NOT NULL,\
        phoneNumber VARCHAR(16) NOT NULL,\
        address VARCHAR(255) NOT NULL,\
        date_time TEXT,\
        equipmentReturndate TEXT,\
        rentalTime DOUBLE,\
        supplement DOUBLE)";
    if (sqlite3_exec(date->db, this->querry.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS::INITTABLE:: " << sqlite3_errmsg(date->db) << std::endl;
        return;
    }
}
void Customers::initTableRentals(Date* date, const std::string& rental)
{
    this->querry = "CREATE TABLE IF NOT EXISTS " + rental + "(\
        customer_id INTEGER, \
        Skis_id INTEGER, \
        Poles_id INTEGER, \
        Helmets_id INTEGER, \
        Boots_id INTEGER, \
        FOREIGN KEY(customer_id) REFERENCES Customers(id), \
        FOREIGN KEY(Skis_id) REFERENCES Skis(id), \
        FOREIGN KEY(Poles_id) REFERENCES Poles(id), \
        FOREIGN KEY(Helmets_id) REFERENCES Helmets(id) \
        FOREIGN KEY(Boots_id) REFERENCES Boots(id) \
        )";
    if (sqlite3_exec(date->db, this->querry.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS::INIT_TABLE_RENTALS:: " << sqlite3_errmsg(date->db) << std::endl;
        sqlite3_close(date->db);
        return;
    }
}

Customers::Customers(Date* date, const std::string& dbName, const std::string& rental)
    : dbName(""), 
    querry(""),   
    stmt(nullptr),
    rc(0)
{
	this->initTable(date, dbName);
    this->initTableRentals(date, rental);
}
Customers::~Customers()
{
}

int Customers::getLastCustomerId(sqlite3* db, const std::string& dbName)
{
    this->querry = "SELECT MAX(id) FROM " + dbName;
    int lastId = -1;

    if (sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr) != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS:: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);  // To jest wa¿ne, nawet jeœli wyst¹pi³ b³¹d!
        return lastId;
    }

    if (sqlite3_step(this->stmt) == SQLITE_ROW)
    {
        lastId = sqlite3_column_int(this->stmt, 0);
    }
    else
    {
        std::cout << "ERROR while retrieving max ID: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(this->stmt);

    return lastId;
}
void Customers::insertDate(sqlite3* db, const std::string& firstName, const std::string& lastName, const std::string& phoneNumber, const std::string& address, double rentalTime, const std::string& dbName)
{
    this->querry = "INSERT INTO " + dbName +
        "(FirstName, LastName, phoneNumber, address, date_time, equipmentReturndate, rentalTime, supplement) "
        "VALUES (?, ?, ?, ?, ?, NULL, ?, NULL)";

    if (sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr) != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS::INITDATA " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return;
    }

    sqlite3_bind_text(this->stmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(this->stmt, 2, lastName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(this->stmt, 3, phoneNumber.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(this->stmt, 4, address.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(this->stmt, 5, this->getCurrentdateTime().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(this->stmt, 6, rentalTime);

    rc = sqlite3_step(this->stmt);

    if (rc != SQLITE_DONE)
    {
        std::cout << "ERROR while inserting: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(this->stmt);
}

std::string Customers::calculateEnddate(const std::string& startdate, double rentalTime)
{
    std::tm tm = {};
    std::istringstream iss(startdate);
    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (iss.fail())
    {
        std::cout << "Invalid date format. Please enter date in YYYY-MM-DD HH:MM:SS format." << std::endl;
        return "";
    }

    std::time_t start_time = std::mktime(&tm);
    std::time_t end_time = start_time + static_cast<std::time_t>(rentalTime * 3600); // Przeliczenie na sekundy

    std::tm end_tm;
    localtime_s(&end_tm, &end_time);

    std::ostringstream oss;
    oss << std::put_time(&end_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
void Customers::insertRental(sqlite3* db, const std::string rental, const int customer_id, const int ski_id, const int poles_id, const int helmet_id, const int boots_id)
{
    // Bezpoœrednie wstawienie nazwy tabeli w zapytaniu
    this->querry = "INSERT INTO " + rental + " (customer_id, Skis_id, Poles_id, Helmets_id, Boots_id) "
        "VALUES (?, ?, ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, querry.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS::INITRENTAL:: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return;
    }
    sqlite3_bind_int(this->stmt, 1, customer_id);
    sqlite3_bind_int(this->stmt, 2, ski_id);
    sqlite3_bind_int(this->stmt, 3, poles_id);
    sqlite3_bind_int(this->stmt, 4, helmet_id);
    sqlite3_bind_int(this->stmt, 5, boots_id);

    // Wykonywanie zapytania
    rc = sqlite3_step(this->stmt);

    if (rc != SQLITE_DONE)
    {
        std::cout << "ERROR while inserting: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(this->stmt);
}
void Customers::deleteDate(sqlite3* db, int id, const std::string& dbName)
{
    std::string deleteQuery = "DELETE FROM " + dbName + " WHERE id = ?";
    sqlite3_stmt* deleteStatement;
    int rc = sqlite3_prepare_v2(db, deleteQuery.c_str(), -1, &deleteStatement, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS::DELETE_DATA::DELETE QUERY:: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(deleteStatement);  // Finalize the statement before returning.
        return;
    }

    sqlite3_bind_int(deleteStatement, 1, id);

    if (sqlite3_step(deleteStatement) == SQLITE_DONE)
    {
        int rowCount = sqlite3_changes(db);
        if (rowCount == 0)
        {
            std::cout << "No record found with ID: " << id << std::endl;
        }
        else
        {
            std::cout << "Record with ID: " << id << " deleted successfully." << std::endl;
        }
    }
    else
    {
        std::cout << "ERROR::CUSTOMERS::DELETE_DATA::DELETE QUERY:: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(deleteStatement);  // Always finalize at the end, regardless of the result.
}

std::string Customers::getCurrentdateTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm timeInfo;
    localtime_s(&timeInfo, &currentTime);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);

    return buffer;
}

void Customers::displayDate(sqlite3* db, const std::string& dbName, const std::string& rentals)
{
    std::string selectQuery = "SELECT * FROM " + dbName;
    sqlite3_stmt* statement;
    int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &statement, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS::DISPLAYDATA::SELECT_QUERRY_1:: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "|_" << std::right << std::setfill('*') << std::setw(65) << "_ | " << std::endl;
    std::cout << "|_ " << std::right << std::setfill(' ') << std::setw(28) << "CUSTOMER LIST"
        << std::right << std::setfill(' ') << std::setw(16) << "_ | " << std::endl;
    std::cout << "|_" << std::right << std::setfill('*') << std::setw(45) << "_ | " << std::endl;

    std::cout << std::setfill('_') << std::setw(270) << std::left << " _" << "_" << std::endl

        << " | " << std::setfill(' ') << std::setw(5) << std::left << "ID"
        << " | " << std::setw(20) << "Name"
        << " | " << std::setw(20) << "Surname"
        << " | " << std::setw(15) << "Phone number"
        << " | " << std::setw(30) << "Address"
        << " | " << std::setw(20) << "Start"
        << " | " << std::setw(20) << "End"
        << " | " << std::setw(10) << "RentalTime"
        << " | " << std::setw(10) << "Supplement"
        << " | " << std::setw(20) << "Ski id"
        << " | " << std::setw(20) << "Poles id"
        << " | " << std::setw(20) << "Helmet id"
        << " | " << std::setw(20) << "Boots id"
        << " | " << std::endl
        << std::setfill('-') << std::setw(270) << std::left << " |" << "|" << std::endl;

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(statement, 0);
        std::string FirstName(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));

        std::string LastName(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));

        std::string PhoneNumber(reinterpret_cast<const char*>(sqlite3_column_text(statement, 3)));

        std::string Address(reinterpret_cast<const char*>(sqlite3_column_text(statement, 4)));

        std::string startDate(reinterpret_cast<const char*>(sqlite3_column_text(statement, 5)));

        const char* endDateText = reinterpret_cast<const char*>(sqlite3_column_text(statement, 6));
        std::string endDate = (endDateText != nullptr) ? endDateText : "NULL";

        int rentalTime = sqlite3_column_int(statement, 7);

        const char* supplementText = reinterpret_cast<const char*>(sqlite3_column_text(statement, 8));
        std::string supplement = (supplementText != nullptr) ? ((std::stoi(supplementText) < 0) ? "0" : supplementText) : "NULL";

        //------------------------- Rental Query -------------------------
        std::string skiIDs, polesIDs, helmetIDs, bootsIDs;

        // Wykonanie zapytania SQL
        std::string selectQuery = "SELECT Skis_id, Poles_id, Helmets_id, Boots_id FROM " + rentals + " WHERE customer_id = " + std::to_string(id);
        sqlite3_stmt* statement1;
        int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &statement1, nullptr);

        if (rc != SQLITE_OK) {
            std::cout << "ERROR::CUSTOMERS::SELECT_QUERRY_2:: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // Pobieranie ski_id i dodawanie ich do wektora
        while (sqlite3_step(statement1) == SQLITE_ROW)
        {
            int ski_id = sqlite3_column_int(statement1, 0);
            int poles_id = sqlite3_column_int(statement1, 1);
            int helmet_id = sqlite3_column_int(statement1, 2);
            int boots_id = sqlite3_column_int(statement1, 3);

            skiIDs = skiIDs + " " + std::to_string(ski_id);
            polesIDs = polesIDs + " " + std::to_string(poles_id);
            helmetIDs = helmetIDs + " " + std::to_string(helmet_id);
            bootsIDs = bootsIDs + " " + std::to_string(boots_id);
        }

        //-------------------------cout-------------------------------------
        std::cout << " | " << std::setfill(' ') << std::setw(5) << std::left << id
            << " | " << std::setw(20) << FirstName
            << " | " << std::setw(20) << LastName
            << " | " << std::setw(15) << PhoneNumber
            << " | " << std::setw(30) << Address
            << " | " << std::setw(20) << startDate
            << " | " << std::setw(20) << endDate
            << " | " << std::setw(10) << rentalTime
            << " | " << std::setw(10) << supplement
            << " | " << std::setw(20) << skiIDs
            << " | " << std::setw(20) << polesIDs
            << " | " << std::setw(20) << helmetIDs
            << " | " << std::setw(20) << bootsIDs
            << " | " << std::endl;
    }
    std::cout << std::setfill('_') << std::setw(270) << std::left << " |" << "|" << std::endl;

    sqlite3_finalize(statement);
}
void Customers::displayRentals(sqlite3* db, const std::string& rental)
{
    this->querry = "SELECT * FROM " + rental;
    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return;
    }

    std::cout << std::setfill('_') << std::setw(199) << "_" << std::endl;
    std::cout << " | " << std::setw(10) << "Cusotmers_ID"
        << " | " << std::setw(20) << "Skis id"
        << " | " << std::setw(20) << "Poles id"
        << " | " << std::setw(20) << "Helmet id"
        << " | " << std::setw(20) << "Boots id"
        << " | " << std::endl;
    std::cout << std::setfill('-') << std::setw(199) << "-" << std::endl;

    while (sqlite3_step(this->stmt) == SQLITE_ROW)
    {
        std::string customer_id = reinterpret_cast<const char*>(sqlite3_column_text(this->stmt, 0));
        std::string skiIDs = reinterpret_cast<const char*>(sqlite3_column_text(this->stmt,1));
        std::string polesIDs = reinterpret_cast<const char*>(sqlite3_column_text(this->stmt, 2));
        std::string helmetIDs = reinterpret_cast<const char*>(sqlite3_column_text(this->stmt, 3));
        std::string bootsIDs = reinterpret_cast<const char*>(sqlite3_column_text(this->stmt, 4));


        std::cout << "| " << std::setw(10) << customer_id
            << " | " << std::setw(20) << skiIDs
            << " | " << std::setw(20) << polesIDs
            << " | " << std::setw(20) << helmetIDs
            << " | " << std::setw(20) << bootsIDs

            << " | " << std::endl;
    }

    std::cout << std::setfill('_') << std::setw(199) << "_" << std::endl;

    sqlite3_finalize(this->stmt);
}
void Customers::deleteRental(sqlite3* db, const int id, const std::string& rental)
{
    // Zapytanie z wi¹zaniem parametru dla id
    this->querry = "DELETE FROM " + rental + " WHERE customer_id = ?";

    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS::DELETE_RENTAL::PREPARE:: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Wi¹¿emy id z zapytaniem
    sqlite3_bind_int(this->stmt, 1, id);

    rc = sqlite3_step(this->stmt);
    if (rc != SQLITE_DONE)
    {
        std::cout << "ERROR::CUSTOMERS::DELETE_RENTAL::EXECUTE:: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return;
    }
    sqlite3_finalize(this->stmt);
}
void Customers::equipmentReturn(sqlite3* db, int id)
{
    // Zapytanie z wi¹zaniem parametrów dla id oraz dla data/czas
    this->querry = "UPDATE Customers SET equipmentReturndate = ? WHERE id = ?";
    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "ERROR::CUSTOMERS::PREPARE:: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_text(this->stmt, 1, this->getCurrentdateTime().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(this->stmt, 2, id);

    rc = sqlite3_step(this->stmt);
    if (rc != SQLITE_DONE)
    {
        std::cout << "ERROR::CUSTOMERS::EXECUTE:: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return;
    }

    if (sqlite3_changes(db) > 0)
    {
        std::cout << "Zwrot sprzetu zostal zaktualizowny." << std::endl;
    }
    else
    {
        std::cout << "Nie ma takiego rekordu o ID: " << id << std::endl;
    }

    sqlite3_finalize(this->stmt);
    this->updateSupplement(db, id);
}

void Customers::updateSupplement(sqlite3* db, int id)
{
    std::string selectQuery = "SELECT date_time, equipmentReturndate, rentalTime FROM Customers WHERE id = ?";
    sqlite3_stmt* statement;
    int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &statement, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(statement);
        return;
    }

    sqlite3_bind_int(statement, 1, id);

    rc = sqlite3_step(statement);
    if (rc == SQLITE_ROW)
    {
        const char* datetime1Str = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
        const char* datetime2Str = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
        int rentalTime = sqlite3_column_int(statement, 2);

        std::string datetime1(datetime1Str);
        std::string datetime2(datetime2Str);

        std::tm tm1 = {};
        std::istringstream datetime1Stream(datetime1);
        datetime1Stream >> std::get_time(&tm1, "%Y-%m-%d %H:%M:%S");
        auto tp1 = std::chrono::system_clock::from_time_t(std::mktime(&tm1));

        auto tp1WithAddedHours = tp1 + std::chrono::hours(rentalTime);

        std::tm tm2 = {};
        std::istringstream datetime2Stream(datetime2);
        datetime2Stream >> std::get_time(&tm2, "%Y-%m-%d %H:%M:%S");
        auto tp2 = std::chrono::system_clock::from_time_t(std::mktime(&tm2));

        auto diffMinutes = std::chrono::duration_cast<std::chrono::minutes>(tp2 - tp1WithAddedHours).count();

        std::string update = "UPDATE Customers SET supplement = ? WHERE id = ?";
        sqlite3_stmt* updateStatement;
        rc = sqlite3_prepare_v2(db, update.c_str(), -1, &updateStatement, nullptr);

        if (rc != SQLITE_OK)
        {
            std::cout << "Error while preparing the update query: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(updateStatement);
            sqlite3_finalize(statement);
            return;
        }

        sqlite3_bind_int(updateStatement, 1, diffMinutes / 4);
        sqlite3_bind_int(updateStatement, 2, id);
        rc = sqlite3_step(updateStatement);

        if (rc != SQLITE_DONE)
        {
            std::cout << "Error while updating supplement: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(updateStatement);
    }

    sqlite3_finalize(statement);
}
void Customers::deleteTable(sqlite3* db, const std::string& dbName, const std::string& rental)
{
    std::string confirmation;
    std::cout << "Type 'delete " + dbName + "' to confirm deletion of the table's content: ";
    std::getline(std::cin, confirmation);

    // Remove whitespace from the entered phrase
    confirmation.erase(std::remove_if(confirmation.begin(), confirmation.end(), ::isspace), confirmation.end());

    if (confirmation != "delete" + dbName)
    {
        std::cout << "Invalid confirmation phrase. Content not deleted." << std::endl;
        return;
    }

    std::string deleteCustomerQuery = "DELETE FROM " + dbName + ";";
    std::string deleteRentalQuery = "DELETE FROM " + rental + ";";

    char* errMsg = nullptr;

    // Delete content from Customers (or dbName) Table
    int rc1 = sqlite3_exec(db, deleteCustomerQuery.c_str(), nullptr, nullptr, &errMsg);
    if (rc1 != SQLITE_OK) {
        std::cerr << "Failed to delete content from table " << dbName << ". Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    // Delete content from Rentals (or rental) Table
    int rc2 = sqlite3_exec(db, deleteRentalQuery.c_str(), nullptr, nullptr, &errMsg);
    if (rc2 != SQLITE_OK) {
        std::cerr << "Failed to delete content from table " << rental << ". Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}
