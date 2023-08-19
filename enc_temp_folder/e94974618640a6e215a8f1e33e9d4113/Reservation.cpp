#include "stdafx.h"
#include "Reservation.h"

void Reservation::initTable(Date* date)
{
    this->querry =
        "CREATE TABLE IF NOT EXISTS Reservations(\
        id INTEGER PRIMARY KEY,\
        FirstName VARCHAR(50) NOT NULL,\
        LastName VARCHAR(50) NOT NULL,\
        phoneNumber VARCHAR(16) NOT NULL,\
        start_date dateTIME,\
        end_date dateTIME)";
    this->rc = sqlite3_exec(date->db, this->querry.c_str(), nullptr, nullptr, nullptr);
    if (this->rc != SQLITE_OK)
    {
        std::cout << "Error while creating the Reservations table : " << sqlite3_errmsg(date->db) << std::endl;
        return;
    }
}
void Reservation::initTableRentals(Date* date)
{

    this->querry =
        "CREATE TABLE IF NOT EXISTS RentalReservation (\
        customer_id INTEGER,\
        Skis_id INTEGER,\
        Poles_id INTEGER,\
        Helmets_id INTEGER, \
        Boots_id INTEGER, \
        FOREIGN KEY(customer_id) REFERENCES Reservations(id),\
        FOREIGN KEY(Skis_id) REFERENCES Skis(id),\
        FOREIGN KEY(Poles_id) REFERENCES Poles(id)\
        FOREIGN KEY(Helmets_id) REFERENCES Helmets(id) \
        FOREIGN KEY(Boots_id) REFERENCES Boots(id) \
        )";
    this->rc = sqlite3_exec(date->db, this->querry.c_str(), nullptr, nullptr, nullptr);
    if (this->rc != SQLITE_OK)
    {
        std::cout << "Blad przy tworzeniu tabeli: " << sqlite3_errmsg(date->db) << std::endl;
        return;
    }
}

//CONSTRUCTOR AND DESTRUCTOR
Reservation::Reservation(Date* date)
    : dbName(""),
    querry(""),
    stmt(nullptr),
    rc(0)
{
    this->initTable(date);
    this->initTableRentals(date);
}
Reservation::~Reservation()
{

}

//ACCESSORS
int Reservation::getReservedGear(sqlite3* db, const std::string& startdate, const std::string& enddate, int gear_id, const std::string& gearColumnName)
{
    this->querry = "SELECT COUNT(*) FROM RentalReservation\
                        WHERE " + gearColumnName + " = ? \
                        AND customer_id IN (SELECT customer_id FROM Reservations WHERE start_date >= ? AND end_date <= ?)";

    this->rc = sqlite3_prepare_v2(db, this->querry.c_str(), -1, &stmt, nullptr);

    if (this->rc != SQLITE_OK)
    {
        std::cout << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Bind values to placeholders
    sqlite3_bind_int(stmt, 1, gear_id);
    sqlite3_bind_text(stmt, 2, startdate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, enddate.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        int totalGearCount = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return totalGearCount;
    }

    sqlite3_finalize(stmt);
    return -1;
}


//MAKING RESERVATION
void Reservation::makeReservation(sqlite3* db, Gear* skis, Gear* poles, Gear* helmets, Gear* boots, Customers* customers)
{
    std::string firstName, lastName, phoneNumber, startdate, enddate;
    int ski_count = 0;
    int poles_count = 0;
    // Pobieranie danych od u¿ytkownika
    std::cout << "Enter first name: ";
    std::getline(std::cin, firstName);

    std::cout << "Enter last name: ";
    std::getline(std::cin, lastName);

    std::cout << "Enter phone number: ";
    std::getline(std::cin, phoneNumber);

    std::cout << "Enter start date (YYYY-MM-DD HH:MM:SS): ";
    std::getline(std::cin, startdate);
    while (!isValiddate(startdate))
    {
        std::cout << "Enter start date (YYYY-MM-DD HH:MM:SS): ";
        std::getline(std::cin, startdate);
    }

    std::cout << "Enter end date (YYYY-MM-DD HH:MM:SS): ";
    std::getline(std::cin, enddate);
    while (!isValiddate(enddate))
    {
        std::cout << "Enter end date (YYYY-MM-DD HH:MM:SS): ";
        std::getline(std::cin, enddate);
    }
    this->insertReservation(db, firstName, lastName, phoneNumber, startdate, enddate);
    int lastID = this->getLastId(db);
    while(true)
    {
        int ski_id = io_read<int>("Enter a ski id: ", "Invalid input. Please try again");
        int poles_id = io_read<int>("Enter a poles id: ", "Invalid input. Please try again");
        int helmet_id = io_read<int>("Enter a helmet id: ", "Invalid input. Please try again");
        int boots_id = io_read<int>("Enter a boots id: ", "Invalid input. Please try again");

        if (skis->getStockCount(db, ski_id, "Skis") <= 0 && poles->getStockCount(db, poles_id, "Poles") <= 0
            && helmets->getStockCount(db, helmet_id, "Helmets") <= 0 && boots->getStockCount(db, boots_id, "Boots") <= 0)
        {
            break;
        }

        std::map<std::string, int> idMap;
        idMap["Skis"] = ski_id;
        idMap["Poles"] = poles_id;
        idMap["Helmets"] = helmet_id;
        idMap["Boots"] = boots_id;

        for (const auto& gearItem : gearMap)
        {
            int gearId = idMap[gearItem.first];
            std::string gearColumnName = gearItem.first + "_id";

            if (gearItem.second->getStockCount(db, gearId, gearItem.first) > 0)
            {
                gearItem.second->countReservation++;
            }

            if (gearItem.second->getStockCount(db, gearId, gearItem.first) -
                getReservedGear(
                    db,
                    startdate,
                    enddate,
                    gearId, gearColumnName) + 
                    gearItem.second->getGearCountReturnedToReservation(db, gearItem.first, startdate, gearId) < gearItem.second->countReservation)
            {
                std::cout << "Not enough " << gearItem.first << ", look at reservations\n";
                break;
            }
        }
        this->insertRental(db, lastID, ski_id, poles_id, helmet_id, boots_id);
    };

}

double Reservation::gethoursDiffrence(const std::string date1, const std::string date2)
{
    std::tm tm1 = {}, tm2 = {};
    std::istringstream ss1(date1);
    std::istringstream ss2(date2);

    ss1 >> std::get_time(&tm1, "%Y-%m-%d %H:%M:%S");
    ss2 >> std::get_time(&tm2, "%Y-%m-%d %H:%M:%S");

    auto time1 = std::mktime(&tm1);
    auto time2 = std::mktime(&tm2);

    auto diff = std::abs(difftime(time1, time2)); // oblicza ró¿nicê czasu w sekundach
    std::cout << date1 << std::endl << date2 << std::endl << "Diff:: " << diff / 3600 << std::endl;
    return diff / 3600.0; // konwersja na godziny
}

int Reservation::getLastId(sqlite3* db)
{
    this->querry = "SELECT MAX(id) FROM Reservations";
    int lastId = -1;

    if (sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(this->stmt) == SQLITE_ROW)
        {
            lastId = sqlite3_column_int(this->stmt, 0);
        }
    }

    sqlite3_finalize(this->stmt);

    return lastId;
}

void Reservation::insertReservation(sqlite3* db, std::string firstName, std::string lastName, std::string phoneNumber, std::string start_date, std::string end_date)
{
    this->querry = "INSERT INTO Reservations (FirstName, LastName, phoneNumber, start_date, end_date) VALUES (?, ?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(this->stmt, 1, firstName.c_str(), -1, 0);
        sqlite3_bind_text(this->stmt, 2, lastName.c_str(), -1, 0);
        sqlite3_bind_text(this->stmt, 3, phoneNumber.c_str(), -1, 0);
        sqlite3_bind_text(this->stmt, 4, start_date.c_str(), -1, 0);
        sqlite3_bind_text(this->stmt, 5, end_date.c_str(), -1, 0);

        if (sqlite3_step(this->stmt) != SQLITE_DONE)
        {
            std::cout << "Error inserting date: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(this->stmt);
    }
    else
    {
        std::cout << "Preparation error: " << sqlite3_errmsg(db) << std::endl;
    }
}

void Reservation::insertRental(sqlite3* db, int customer_id, int ski_id, int poles_id, int helmets_id, int boots_id)
{
    this->querry = "INSERT INTO RentalReservation (customer_id, Skis_id, Poles_id, Helmets_id, Boots_id) VALUES (?, ?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(this->stmt, 1, customer_id);
        sqlite3_bind_int(this->stmt, 2, ski_id);
        sqlite3_bind_int(this->stmt, 3, poles_id);
        sqlite3_bind_int(this->stmt, 4, helmets_id);
        sqlite3_bind_int(this->stmt, 5, boots_id);

        if (sqlite3_step(this->stmt) != SQLITE_DONE)
        {
            std::cout << "Error inserting date: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(this->stmt);
    }
    else
    {
        std::cout << "Preparation error: " << sqlite3_errmsg(db) << std::endl;
    }
}

void Reservation::confirmReservation(sqlite3* db, Customers* customer)
{
    std::string firstName, lastName, phoneNumber;
    std::cout << "Enter first name: ";
    std::getline(std::cin, firstName);

    std::cout << "Enter last name: ";
    std::getline(std::cin, lastName);

    std::cout << "Enter phone number: ";
    std::getline(std::cin, phoneNumber);

    this->querry = "SELECT * FROM Reservations WHERE FirstName = ? AND LastName = ? AND phoneNumber = ?";

    if (sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, nullptr) != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(this->stmt);
        return;
    }

    sqlite3_bind_text(this->stmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(this->stmt, 2, lastName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(this->stmt, 3, phoneNumber.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(this->stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(this->stmt, 0);

        sqlite3_stmt* rentalStatement;
        std::string rentalQuery = "SELECT Skis_id, Poles_id, Helmets_id, Boots_id FROM RentalReservation WHERE customer_id = ?";

        if (sqlite3_prepare_v2(db, rentalQuery.c_str(), -1, &rentalStatement, nullptr) != SQLITE_OK)
        {
            std::cout << "Error preparing rental query: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(this->stmt);
            return;
        }

        sqlite3_bind_int(rentalStatement, 1, id);

        while (sqlite3_step(rentalStatement) == SQLITE_ROW)
        {
            int ski_id = sqlite3_column_int(rentalStatement, 0);
            int poles_id = sqlite3_column_int(rentalStatement, 1);
            int helmets_id = sqlite3_column_int(rentalStatement, 2);
            int boots_id = sqlite3_column_int(rentalStatement, 3);

            customer->insertRental(db, "Rentals", id, ski_id, poles_id, helmets_id, boots_id);
        }

        sqlite3_finalize(rentalStatement);

        std::string deleteRentalResQuery = "DELETE FROM RentalReservation WHERE customer_id = ?";
        int rc = sqlite3_exec(db, deleteRentalResQuery.c_str(), nullptr, nullptr, nullptr);

        if (rc != SQLITE_OK)
        {
            std::cout << "Error deleting from RentalReservation: " << sqlite3_errmsg(db) << std::endl;
            return;
        }
    }

    sqlite3_finalize(this->stmt);

    std::string deleteResQuery = "DELETE FROM Reservations WHERE FirstName = ? AND LastName = ? AND phoneNumber = ?";
    sqlite3_stmt* deleteStmt;

    if (sqlite3_prepare_v2(db, deleteResQuery.c_str(), -1, &deleteStmt, nullptr) != SQLITE_OK)
    {
        std::cout << "Error preparing delete statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(deleteStmt);
        return;
    }

    sqlite3_bind_text(deleteStmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(deleteStmt, 2, lastName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(deleteStmt, 3, phoneNumber.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(deleteStmt) != SQLITE_DONE)
    {
        std::cout << "Error deleting from Reservations: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(deleteStmt);
}

void Reservation::cancelReservation(sqlite3* db, int reservationID)
{
    std::string firstName, lastName, phoneNumber;

    std::cout << "Enter first name: ";
    std::getline(std::cin, firstName);

    std::cout << "Enter last name: ";
    std::getline(std::cin, lastName);

    std::cout << "Enter phone number: ";
    std::getline(std::cin, phoneNumber);

    std::string deleteQueryReservations = "DELETE FROM Reservations WHERE FirstName = ? AND LastName = ? AND phoneNumber = ?;";
    std::string deleteQueryRentalReservation = "DELETE FROM RentalReservation WHERE customer_id = ?";

    if (sqlite3_prepare_v2(db, this->querry.c_str(), -1, &this->stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lastName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, phoneNumber.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(this->stmt) != SQLITE_DONE) {
            std::cout << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        }
        else
        {
            std::cout << "Records deleted successfully from RentalReservation" << std::endl;
        }
        sqlite3_finalize(this->stmt);
    }


    this->rc = sqlite3_prepare_v2(db, deleteQueryReservations.c_str(), -1, &this->stmt, nullptr);
    sqlite3_bind_int(this->stmt, 1, reservationID);

    if (sqlite3_step(this->stmt) != SQLITE_DONE)
    {
        std::cout << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    }
    else 
    {
        std::cout << "Records deleted successfully from Reservations" << std::endl;
    }
    sqlite3_finalize(this->stmt);
}
void Reservation::displayReservation(sqlite3* db)
{
    std::string selectQuery = "SELECT * FROM Reservations";
    sqlite3_stmt* statement;
    int rc = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &statement, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "Error while preparing the query: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "|_" << std::right << std::setfill('*') << std::setw(45) << "_ | " << std::endl;
    std::cout << "|_ " << std::right << std::setfill(' ') << std::setw(28) << "RESERVATION LIST"
        << std::right << std::setfill(' ') << std::setw(16) << "_ | " << std::endl;
    std::cout << "|_" << std::right << std::setfill('*') << std::setw(45) << "_ | " << std::endl;

    std::cout << std::setfill('_') << std::setw(199) << std::left << " _" << "_" << std::endl
        << " | " << std::setfill(' ') << std::setw(5) << std::left << "ID"
        << " | " << std::setw(20) << "Name"
        << " | " << std::setw(20) << "Surname"
        << " | " << std::setw(15) << "Phone number"
        << " | " << std::setw(20) << "Start"
        << " | " << std::setw(20) << "End"
        << " | " << std::setw(20) << "Ski id"
        << " | " << std::setw(20) << "Poles id"
        << " | " << std::setw(20) << "Helmets id"
        << " | " << std::setw(20) << "Boots id"
        << " | " << std::endl
        << std::setfill('-') << std::setw(199) << std::left << " |" << "|" << std::endl;

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(statement, 0);
        std::string FirstName(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
        std::string LastName(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));
        std::string PhoneNumber(reinterpret_cast<const char*>(sqlite3_column_text(statement, 3)));
        std::string startDate(reinterpret_cast<const char*>(sqlite3_column_text(statement, 4)));
        std::string endDate(reinterpret_cast<const char*>(sqlite3_column_text(statement, 5)));

        std::string skiIDs, polesIDs, helmetsIDs, bootsIDs;

        // Wykonanie zapytania SQL
        std::string selectQuery = "SELECT Skis_id, Poles_id, Helmets_id, Boots_id FROM RentalReservation WHERE customer_id = ?";
        sqlite3_stmt* statement1;
        int rc_inner = sqlite3_prepare_v2(db, selectQuery.c_str(), -1, &statement1, nullptr);

        if (rc_inner != SQLITE_OK)
        {
            std::cout << "B³¹d przy przygotowywaniu zapytania: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(statement);
            return;
        }

        sqlite3_bind_int(statement1, 1, id);

        // Pobieranie ski_id i dodawanie ich do wektora
        while (sqlite3_step(statement1) == SQLITE_ROW)
        {
            int ski_id = sqlite3_column_int(statement1, 0);
            int poles_id = sqlite3_column_int(statement1, 1);
            int helmets_id = sqlite3_column_int(statement1, 2);
            int boots_id = sqlite3_column_int(statement1, 3);

            skiIDs += " " + std::to_string(ski_id);
            polesIDs += " " + std::to_string(poles_id);
            helmetsIDs += " " + std::to_string(helmets_id);
            bootsIDs += " " + std::to_string(boots_id);
        }

        sqlite3_finalize(statement1);

        std::cout << " | " << std::setfill(' ') << std::setw(5) << std::left << id
            << " | " << std::setw(20) << FirstName
            << " | " << std::setw(20) << LastName
            << " | " << std::setw(15) << PhoneNumber
            << " | " << std::setw(20) << startDate
            << " | " << std::setw(20) << endDate
            << " | " << std::setw(20) << skiIDs
            << " | " << std::setw(20) << polesIDs
            << " | " << std::setw(20) << helmetsIDs
            << " | " << std::setw(20) << bootsIDs
            << " | " << std::endl;
    }

    std::cout << std::setfill('_') << std::setw(199) << std::left << " |" << "|" << std::endl;

    sqlite3_finalize(statement);
}


bool Reservation::isValiddate(const std::string& date)
{
    std::istringstream iss(date);
    std::tm tm = {};
    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (iss.fail())
    {
        std::cout << "Invalid date format. Please enter date in YYYY-MM-DD HH:MM::SS format." << std::endl;
        return false;
    }

    // SprawdŸ, czy date jest poprawna
    std::time_t t = std::mktime(&tm);
    if (t == -1)
    {
        std::cout << "Invalid date. Please enter a valid date." << std::endl;
        return false;
    }

    return true;
}

