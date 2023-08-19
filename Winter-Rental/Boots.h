#pragma once
#include "Date.h"
#include "Gear.h"
class Boots
    :public Gear
{
protected:
    void insertDate(sqlite3* db, const std::string& dbName, const int stock_count, const int minSize);
public:
    Boots(Date* date);
    virtual ~Boots();
};

