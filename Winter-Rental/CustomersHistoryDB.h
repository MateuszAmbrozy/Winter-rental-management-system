#pragma once
#include <iostream>
#include <string>
#include<vector>
#include <windows.h>
#include<iomanip>
#include <limits>
#include <chrono>
#include <sstream>
#include "sqllite/sqlite3.h"
#include <string>
#include "Data.h"
#include "Customers.h"


class CustomersHistoryDB
    :public Customers
{
private:
   // void initTable(Data* data);

public:
    CustomersHistoryDB(Data* data);
    virtual ~CustomersHistoryDB();

    //void insertData(sqlite3* db, const int ski_id, const int poles_id, std::string firstName, std::string lastName, std::string phoneNumber, std::string address, double rentalTime);
    //void deleteData(sqlite3* db, int id);
    //void displayData(sqlite3* db);
    void deleteTable(sqlite3* db);
};

