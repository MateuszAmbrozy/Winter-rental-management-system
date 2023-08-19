#include "stdafx.h"
#include "Poles.h"

Poles::Poles(Date* date)
    :Gear(date)
{
    this->initTable(date, "Poles");
    this->insertDate(date->db, "Poles", 15, 130);
}

Poles::~Poles()
{
}