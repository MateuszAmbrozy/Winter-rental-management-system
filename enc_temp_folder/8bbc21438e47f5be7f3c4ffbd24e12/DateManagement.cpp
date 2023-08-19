#include "stdafx.h"
#include "DateManagement.h"

//usuwanie w history nie dzia³a
// ataki typu SQL Injection - oczytaæ i zastosowaæ siê do tego

//PRIVATE FUCTIONS
void DateManagement::mainMenu()
{

	int menuItem = -1;
	do 
	{
		std::cout << "===============================================" << std::endl;
		std::cout << "||                                           ||" << std::endl;
		std::cout << "||       WINTER RENTAL SYSTEM 2023           ||" << std::endl;
		std::cout << "||                                           ||" << std::endl;
		std::cout << "||===========================================||" << std::endl;
		std::cout << "||              MAIN MENU                    ||" << std::endl;
		std::cout << "||-------------------------------------------||" << std::endl;
		std::cout << "||   1 - Manage Customers                    ||" << std::endl;
		std::cout << "||   2 - Manage History of customers         ||" << std::endl;
		std::cout << "||   3 - Manage Gear                         ||" << std::endl;
		std::cout << "||   4 - Manage Reservations                 ||" << std::endl;
		std::cout << "||   0 - Exit                                ||" << std::endl;
		std::cout << "===============================================" << std::endl;

		menuItem = io_read<int>("[Input] Main Menu Item: ", "Invalid input. Please try again");
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << std::endl << std::endl;

		switch (menuItem) {
		case 1:
			std::system("cls");
			this->customerMenu();

			break;
		case 2:

			std::system("cls");
			this->historyCustomerMenu();

			break;
		case 3:
			std::system("cls");
			this->gearMenu();

			break;
		case 4:
			std::system("cls");
			this->reservationMenu();

			break;
		default:
			std::system("cls");
			break;
		}
		//std::system("cls");
	} while (menuItem != 0);

}
void DateManagement::customerMenu()
{
	int menuItem = -1;

	do 
	{
		std::cout << "||=================================||" << std::endl;
		std::cout << "||                                 ||" << std::endl;
		std::cout << "||     WINTER RENTAL SYSTEM 2022   ||" << std::endl;
		std::cout << "||                                 ||" << std::endl;
		std::cout << "||=================================||" << std::endl;
		std::cout << "||      CUSTOMERS SUB MENU         ||" << std::endl;
		std::cout << "||---------------------------------||" << std::endl;
		std::cout << "||   1 - Add Customer              ||" << std::endl;
		std::cout << "||   2 - Return Equipment          ||" << std::endl;
		std::cout << "||   3 - Show All Customers        ||" << std::endl;
		std::cout << "||   4 - Show All Rentals          ||" << std::endl;
		std::cout << "||   0 - Back to MAIN MENU         ||" << std::endl;
		std::cout << "||---------------------------------||" << std::endl;
		menuItem = io_read<int>("[Input] Customers Menu Item: ", "Invalid input. Please try again");
		std::cout << "-----------------------------------" << std::endl;
		std::cout << std::endl << std::endl;

		switch (menuItem) 
		{
		case 1://INSERT CUSTOMER
		{
			//SKI
			std::string firstName, lastName, phoneNumber, address;

			std::cout << "Enter First name: ";
			std::getline(std::cin, firstName);

			std::cout << "Enter Last name: ";
			std::getline(std::cin, lastName);

			std::cout << "Enter phone number: ";
			std::getline(std::cin, phoneNumber);

			std::cout << "Enter address: ";
			std::getline(std::cin, address);

			double rentalTime = io_read<double>("Enter rentalTime: ", "Invalid input. Please try again");

			this->customer->insertDate(date.db, firstName, lastName, phoneNumber, address, rentalTime, "Customers");
			int lastCustomerId = customer->getLastCustomerId(date.db, "Customers");

			while(true)
			{
				int ski_id = io_read<int>("Enter a ski id: ", "Invalid input. Please try again");
				int poles_id = io_read<int>("Enter a poles id: ", "Invalid input. Please try again");
				int helmet_id = io_read<int>("Enter a helmet id: ", "Invalid input. Please try again");
				int boots_id = io_read<int>("Enter a boots id: ", "Invalid input. Please try again");

				if (this->skis->getStockCount(this->date.db, ski_id, "Skis") <= 0 && this->poles->getStockCount(date.db, poles_id, "Poles") <= 0
					&& this->helmets->getStockCount(this->date.db, helmet_id, "Helmets") <= 0 && this->boots->getStockCount(date.db, boots_id, "Boots") <= 0)
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

					if (gearItem.second->getStockCount(date.db, gearId, gearItem.first) > 0)
					{
						gearItem.second->countReservation++;

						if (gearItem.second->getStockCount(date.db, gearId, gearItem.first) -
							reservation->getReservedGear(date.db, this->customer->getCurrentdateTime(),
								this->customer->calculateEnddate(this->customer->getCurrentdateTime(), rentalTime),
								gearId, gearColumnName) < gearItem.second->countReservation)
						{
							std::cout << "Not enough " << gearItem.first << ", look at reservations\n";
							break;
						}
					}
					gearItem.second->updateStockCount(this->date.db, -1, gearId, gearItem.first);
				}
				this->customer->insertRental(this->date.db, "Rentals", lastCustomerId, ski_id, poles_id, helmet_id, boots_id);
			}

			std::system("cls");

			break;
		}
		case 2: //RETURN EQ
		{
			int id = io_read<int>("Enter user id: ", "Invalid input. Please try again");
			this->customer->equipmentReturn(date.db, id);

			// Rozpoczêcie transakcji
			std::string beginTransaction = "BEGIN TRANSACTION;";
			if (sqlite3_exec(date.db, beginTransaction.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
			{
				std::cerr << "Error starting transaction: " << sqlite3_errmsg(date.db) << std::endl;
				return;
			}

			// Wstawienie danych do CustomerHistoryDB
			std::string query1 = "INSERT INTO CustomersHistoryDB SELECT * FROM Customers WHERE id = ?;";
			sqlite3_stmt* stmt1;
			if (sqlite3_prepare_v2(date.db, query1.c_str(), -1, &stmt1, nullptr) == SQLITE_OK)
			{
				sqlite3_bind_int(stmt1, 1, id);
				if (sqlite3_step(stmt1) != SQLITE_DONE)
				{
					std::cerr << "Query execution error: " << sqlite3_errmsg(date.db) << std::endl;
					sqlite3_finalize(stmt1);
					sqlite3_exec(date.db, "ROLLBACK;", nullptr, nullptr, nullptr);
					return;
				}
				sqlite3_finalize(stmt1);
			}

			// Wstawienie danych do HistoryRentals
			std::string query2 = "INSERT INTO HistoryRentals SELECT * FROM Rentals WHERE customer_id = ?;";
			sqlite3_stmt* stmt2;
			if (sqlite3_prepare_v2(date.db, query2.c_str(), -1, &stmt2, nullptr) == SQLITE_OK)
			{
				sqlite3_bind_int(stmt2, 1, id);
				if (sqlite3_step(stmt2) != SQLITE_DONE)
				{
					std::cerr << "Query execution error: " << sqlite3_errmsg(date.db) << std::endl;
					sqlite3_finalize(stmt2);
					sqlite3_exec(date.db, "ROLLBACK;", nullptr, nullptr, nullptr);
					return;
				}
				sqlite3_finalize(stmt2);
			}

			for (const auto& gearItem : gearMap)
			{
				gearItem.second->returnGear(date.db, gearItem.first, id, gearItem.second);
			}

			this->customer->deleteDate(date.db, id, "Customers");
			this->customer->deleteRental(date.db, id, "Rentals");

			// Zatwierdzenie transakcji
			if (sqlite3_exec(date.db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK)
			{
				std::cerr << "Error committing transaction: " << sqlite3_errmsg(date.db) << std::endl;
				return;
			}

			std::system("cls");
			this->customer->displayDate(date.db, "Customers", "Rentals");
			break;
		}

		case 3: //SHOW ALL CUSTOMERS
			std::system("cls");
			this->customer->displayDate(date.db, "Customers", "Rentals");
			break;
		case 4:
			this->customer->displayRentals(date.db, "Rentals");
			break;
		default:
			std::system("cls");
			break;
		}
	} while (menuItem != 0);
}
void DateManagement::historyCustomerMenu()
{
	int menuItem = -1;

	do {
		std::cout << "||===================================||" << std::endl;
		std::cout << "||                                   ||" << std::endl;
		std::cout << "||     WINTER RENTAL SYSTEM 2022     ||" << std::endl;
		std::cout << "||                                   ||" << std::endl;
		std::cout << "||===================================||" << std::endl;
		std::cout << "||      CUSTOMERS HISTORY SUB MENU   ||" << std::endl;
		std::cout << "||-----------------------------------||" << std::endl;
		std::cout << "||   1 - Show History of Customers   ||" << std::endl;
		std::cout << "||   2 - Delete Customer             ||" << std::endl;
		std::cout << "||   3 - Clear Table                 ||" << std::endl;
		std::cout << "||   0 - Back to MAIN MENU           ||" << std::endl;
		std::cout << "||-----------------------------------||" << std::endl;
		menuItem = io_read<int>("[Input] History Menu Item: ", "Invalid input. Please try again");
		std::cout << "-----------------------------------" << std::endl;
		std::cout << std::endl << std::endl;

		switch (menuItem)
		{
		case 1:
		{
			std::system("cls");
			this->history->displayDate(date.db, "CustomersHistoryDB", "HistoryRentals");
			break;
		}
		case 2:
		{
			int id = io_read<int>("Enter a id to delete: ", "Invalid input. Please try again");
			this->history->deleteDate(date.db, id, "CustomersHistoryDB");
			this->history->deleteRental(date.db, id, "HistoryRentals");
			std::system("cls");
		}
			break;
		case 3:
			this->history->deleteTable(date.db, "CustomersHistoryDB", "HistoryRentals");
			std::system("cls");
			break;
		default:
			std::system("cls");
			break;
		}

	} while (menuItem != 0);
}
void DateManagement::gearMenu()
{
	int menuItem = -1;

	do 
	{
		std::cout << "||==================================||" << std::endl;
		std::cout << "||                                  ||" << std::endl;
		std::cout << "||      WINTER RENTAL SYSTEM 2022   ||" << std::endl;
		std::cout << "||                                  ||" << std::endl;
		std::cout << "||==================================||" << std::endl;
		std::cout << "||       GEAR MANAGE SUB MENU       ||" << std::endl;
		std::cout << "||----------------------------------||" << std::endl;
		std::cout << "||   1 - Add skis                   ||" << std::endl;
		std::cout << "||   2 - Add Poles                  ||" << std::endl;
		std::cout << "||   3 - Add Boots                  ||" << std::endl;
		std::cout << "||   4 - Add Helmets                ||" << std::endl;
		std::cout << "||   5 - Remove skis                ||" << std::endl;
		std::cout << "||   6 - Remove Poles               ||" << std::endl;
		std::cout << "||   7 - Remove Boots               ||" << std::endl;
		std::cout << "||   8 - Remove Helmets             ||" << std::endl;
		std::cout << "||   9 - Show skis date base        ||" << std::endl;
		std::cout << "||   10 - Show poles date base      ||" << std::endl;
		std::cout << "||   11 - Show boots date base      ||" << std::endl;
		std::cout << "||   12 - Show helmets date base    ||" << std::endl;
		std::cout << "||   0 - Back to MAIN MENU          ||" << std::endl;
		std::cout << "||----------------------------------||" << std::endl;
		menuItem = io_read<int>("[Input] Gear Menu Item: ", "Invalid input. Please try again");
		std::cout << "-----------------------------------" << std::endl;
		std::cout << std::endl << std::endl;

		switch (menuItem)
		{
		case 1:
			this->skis->addGear(date.db, "Skis");
			std::system("cls");
		break;

		case 2:
			this->poles->addGear(date.db, "Poles");
			std::system("cls");
		break;

		case 3:
			this->boots->addGear(date.db, "Boots");
			std::system("cls");
			break;

		case 4:
			this->helmets->addGear(date.db, "Helmets");
			//std::system("cls");
			break;

		case 5:
			this->skis->removeGear(date.db, "Skis");
			std::system("cls");
			break;

		case 6:
			this->poles->removeGear(date.db, "Poles");
			std::system("cls");
			break;

		case 7:
			this->boots->removeGear(date.db, "Boots");
			std::system("cls");
			break;

		case 8:
			this->helmets->removeGear(date.db, "Helmets");
			std::system("cls");
			break;

		case 9:
			std::system("cls");
			skis->displayDate(date.db, "Skis");
			break;

		case 10:
			std::system("cls");
			poles->displayDate(date.db, "Poles");
			break;

		case 11:
			std::system("cls");
			boots->displayDate(date.db, "Boots");
			break;

		case 12:
			std::system("cls");
			helmets->displayDate(date.db, "Helmets");
			break;
		default:
			std::system("cls");
			break;
	}

	} while (menuItem != 0);
}
void DateManagement::reservationMenu()
{
	int menuItem = -1;

	do {
		std::cout <<
			"||===================================||\n"
			"||                                   ||\n"
			"||     WINTER RENTAL SYSTEM 2022     ||\n"
			"||                                   ||\n"
			"||===================================||\n"
			"||      RESERVATION SUB MENU         ||\n"
			"||-----------------------------------||\n"
			"||   1 - Make reservation            ||\n"
			"||   2 - Cancel reservation          ||\n"
			"||   3 - Confirm reservations        ||\n"
			"||   4 - Display reservations        ||\n"
			"||   0 - Back to MAIN MENU           ||\n"
			"||-----------------------------------||\n";
			menuItem = io_read<int>("[Input] Main Menu Item: ", "Invalid input. Please try again");
		std::cout << "-----------------------------------" << std::endl;
		std::cout << std::endl << std::endl;

		switch (menuItem)
		{
		case 1:
			std::system("cls");
			this->reservation->makeReservation(date.db, skis, poles, helmets, boots, this->customer);
			break;
		case 2:
		{
			int reservationID = io_read<int>("Enter a reservation id to cancel: ", "Invalid input. Please try again");
			this->reservation->cancelReservation(date.db, reservationID);
			std::system("cls");
		}
		break;
		case 3:
			this->reservation->confirmReservation(date.db, this->customer);
			break;
		case 4:
			this->reservation->displayReservation(date.db);
			break;
		default:
			std::system("cls");
			break;
		}

	} while (menuItem != 0);
}

//CONSTRUCTOR AND DESTRUCTOR
DateManagement::DateManagement()
{
	this->date.rc = sqlite3_open("DB", &this->date.db);

	this->customer = new Customers(&this->date, "Customers", "Rentals");
	this->history = new Customers(&this->date, "CustomersHistoryDB", "HistoryRentals");

	this->skis = new Skis(&this->date);
	this->poles = new Poles(&this->date);
	this->helmets = new Helmet(&this->date);
	this->boots = new Boots(&this->date);
	
	this->gearMap["Skis"] = skis;
	this->gearMap["Poles"] = poles;
	this->gearMap["Helmets"] = helmets;
	this->gearMap["Boots"] = boots;
	this->reservation = new Reservation(&this->date);
}
DateManagement::~DateManagement()
{
	delete this->history;
    delete this->customer;
    delete this->skis;
	delete this->boots;
	delete this->helmets;
	delete this->reservation;
    sqlite3_close(date.db);
}
//UPdate
void DateManagement::update()
{
	this->mainMenu();
}