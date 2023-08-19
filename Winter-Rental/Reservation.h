#pragma once
#include "Customers.h"
#include "Poles.h"
#include "Skis.h"

class Reservation
{
private:
	std::string dbName;
	std::string querry;
	sqlite3_stmt* stmt;
	int rc;

	void initTable(Date* date);
	void initTableRentals(Date* date);
	std::map<std::string, Gear*> gearMap;

public:
	//CONSTRUCTOR AND DESTRUCTOR
	Reservation(Date* date);
	~Reservation();

	//ACCESSORS
	int getReservedGear(sqlite3* db, const std::string& startdate, const std::string& enddate, int gear_id, const std::string& gearColumnName);
	double gethoursDiffrence(const std::string date1, const std::string date2);
	int getLastId(sqlite3* db);
	//MAKING RESERVATION
	void makeReservation(sqlite3* db, Gear* skis, Gear* poles, Gear* helmets, Gear* boots, Customers* customers);
	void insertReservation(sqlite3* db, std::string firstName, std::string lastName, std::string phoneNumber, std::string start_date, std::string end_date);
	void insertRental(sqlite3* db, int customer_id, int ski_id, int poles_id, int helmets_id, int boots_id);

	void cancelReservation(sqlite3* db, int reservationID);
	void confirmReservation(sqlite3* db, Customers* customer);
	void displayReservation(sqlite3* db);
	bool isValiddate(const std::string& date);
};

