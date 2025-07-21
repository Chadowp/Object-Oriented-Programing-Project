#include <iostream>
#include "activity 24L-0575.h"

using namespace std;

const char* Activity::activityTypes[typeCount] =
{
    "feeling",
    "thinking about",
    "making",
    "celebrating"
};

Activity::Activity(ifstream& file)
{
    description = nullptr;
    char buffer[100];

    file >> selectedTypeIndex;
    file.ignore();
    selectedTypeIndex -= 1;

    if (selectedTypeIndex < 0 || selectedTypeIndex >= typeCount)
    {
        selectedTypeIndex = -1;
        return;
    }

    file.getline(buffer, 100, '\n');

    int len = strlen(buffer);
    if (len > 0)
    {
        description = new char[len + 1];
        strcpy_s(description, len + 1, buffer);
    }
    else
    {
        description = nullptr;
    }
}

Activity::~Activity()
{
    delete[] description;
}

void Activity::display() const
{
    if (selectedTypeIndex != -1 && description)
    {
        cout << " is " << activityTypes[selectedTypeIndex] << ' ' << description << endl;
    }
}

