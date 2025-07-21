#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include "Date 24L-0575.h"
#include "Network 24L-0575.h"
#include "post 24L-0575.h"
#include "Comment 24L-0575.h"

using namespace std;

class Activity
{
    static const int typeCount = 4;
    static const char* activityTypes[typeCount];

    int selectedTypeIndex;
    char* description;

public:
    Activity(ifstream& input);
    ~Activity();

    void display() const;
};