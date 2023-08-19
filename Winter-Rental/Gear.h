#pragma once
#include "Date.h"
class Gear
{
protected:
   std::string dbName;
   std::string querry;
   sqlite3_stmt* stmt;
   int rc;

   virtual void initTable(Date* date, const std::string& dbName);
public:
    int countReservation; //this variable count how many gear user wants rent for one time

    Gear(Date* date);
    virtual ~Gear();

    virtual void insertDate(sqlite3* db, const std::string& dbName, int stock_count, int minSize);
    virtual void addGear(sqlite3* db, const std::string& dbName);
    virtual void removeGear(sqlite3* db, const std::string& dbName);
    virtual void returnGear(sqlite3* db, const std::string& dbName, const int gearID, Gear* gear);
    virtual void displayDate(sqlite3* db, const std::string& dbName);
    virtual void updateStockCount(sqlite3* db, const int value, const int id, const std::string& dbName);
    virtual int getStockCount(sqlite3* db, const int gearID, const std::string& dbName);
    virtual bool isTableEmpty(sqlite3* db, const std::string& dbName);
    virtual int getGearCountReturnedToReservation(sqlite3* db, const std::string& dbName, const std::string& reservation_date, const int gear_id);
};

