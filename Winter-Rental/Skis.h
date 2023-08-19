#pragma once
#include "Date.h"
#include "Gear.h"
class Skis
    :public Gear
{
private:
public:
    Skis(Date* date);
    virtual ~Skis();
};

