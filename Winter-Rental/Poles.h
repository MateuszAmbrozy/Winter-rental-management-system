#pragma once
#include "Date.h"
#include "Gear.h"
class Poles
    :public Gear
{
public:
    Poles(Date* date);
    virtual ~Poles();
};

