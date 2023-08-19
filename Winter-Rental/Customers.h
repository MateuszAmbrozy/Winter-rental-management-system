#pragma once

#include "Date.h"


class Customers
{
private:
    std::string dbName;
    std::string querry;
    sqlite3_stmt* stmt;
    int rc;
protected:
    void initTable(Date* date, const std::string& dbName);
    void initTableRentals(Date* date, const std::string& rental);
public:
    //COSNTRUCTOR AND DESTRUCTOR
    Customers(Date* date, const std::string& dbName, const std::string& rental);
    Customers() {};
    virtual ~Customers();

    //ACCESSORS
    int getLastCustomerId(sqlite3* db, const std::string& dbName);
    std::string getCurrentdateTime();
    std::string calculateEnddate(const std::string& startdate, const double rentalTime);

    //FUNCTIONS
   void insertDate(sqlite3* db, const std::string& firstName, const std::string& lastName, const std::string& phoneNumber, const std::string& address, double rentalTime, const std::string& dbName);
   void insertRental(sqlite3* db, const std::string rental, const int customer_id, const int ski_id, const int poles_id, const int helmet_id, const int boots_id);
   void deleteDate(sqlite3* db, const int id, const std::string& dbName);
   void deleteRental(sqlite3* db, const int id, const std::string& rental);
   void displayDate(sqlite3* db, const std::string& dbName, const std::string& rentals);
   void displayRentals(sqlite3* db, const std::string& rental);
   void equipmentReturn(sqlite3* db, const int id);
   void updateSupplement(sqlite3* db, const int id);
   void deleteTable(sqlite3* db, const std::string& dbName, const std::string& rental);
};

