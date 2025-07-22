#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include "Date.h"
#include "Network.h"
#include "post.h"
#include "Comment.h"

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
