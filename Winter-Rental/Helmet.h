#pragma once
#include "Date.h"
#include "Gear.h"
class Helmet
    :public Gear
{
private:
    void initTable(Date* date, const std::string& dbName);
    void insertDate(sqlite3* db, const std::string& dbName, const int stock_count, const int minLength);
public:
    Helmet(Date* date);
    virtual ~Helmet();

    void displayDate(sqlite3* db, const std::string& dbName);

    void addGear(sqlite3* db, const std::string& dbName);
    void removeGear(sqlite3* db, const std::string& dbName);
};
