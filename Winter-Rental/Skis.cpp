#include "stdafx.h"
#include "Skis.h"
Skis::Skis(Date* date)
    :Gear(date)
{
    this->initTable(date, "Skis");
    this->insertDate(date->db, "Skis", 15, 130);
}
Skis::~Skis()
{
}

