#pragma once
#include "Date.h"
#include "Customers.h"
#include "Skis.h"
#include "Poles.h"
#include "Helmet.h"
#include "Boots.h"
#include "Reservation.h"
class DateManagement
{
private:
	Date date;
	Customers* customer;
	Customers* history;
	std::map<std::string, Gear*> gearMap;

	Gear* skis;
	Gear* poles;
	Gear* helmets;
	Gear* boots;

	Reservation* reservation;

	void mainMenu();
	void customerMenu();
	void historyCustomerMenu();
	void gearMenu();
	void reservationMenu();
public:
	DateManagement();
	~DateManagement();
	void update();
};


