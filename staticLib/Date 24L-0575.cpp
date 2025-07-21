#include "Date 24L-0575.h"
#include <iostream>
#include <fstream>
using namespace std;

Date Date::currentDate = Date(1, 12, 2024);

void Date::setCurrentDate(const Date& d)
{
    currentDate = d;
}

Date Date::getCurrentDate()
{
    return currentDate;
}

Date::Date(int d, int m, int y)
{
    day = d;
    month = m;
    year = y;
}

void Date::readFromFile(ifstream& file)
{
    file >> day >> month >> year;
    file.ignore();
}


bool Date::operator<=(const Date& other) const
{
    if (year < other.year) return true;
    if (year > other.year) return false;

    if (month < other.month) return true;
    if (month > other.month) return false;

    return day <= other.day;
}

bool Date::operator<(const Date& other) const
{
    if (year < other.year) return true;
    if (year > other.year) return false;

    if (month < other.month) return true;
    if (month > other.month) return false;

    return day < other.day;
}

bool Date::operator>=(const Date& other) const
{
    if (year > other.year) return true;
    if (year < other.year) return false;

    if (month > other.month) return true;
    if (month < other.month) return false;

    return day >= other.day;
}

bool Date::operator>(const Date& other) const
{
    if (year > other.year) return true;
    if (year < other.year) return false;

    if (month > other.month) return true;
    if (month < other.month) return false;

    return day > other.day;
}

bool Date::operator==(const Date& other) const
{
    return (year == other.year && month == other.month && day == other.day);
}

void Date::print() const
{
    cout << '(' << day << '/' << month << '/' << year << ')';
}

int Date::getDay() const
{
    return day;
}

int Date::getMonth() const
{
    return month;
}

int Date::getYear() const
{
    return year;
}


bool Date::isValid(int d, int m, int y) const
{
    if (y < 1900 || y > 2099) return false;
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > 31) return false;

    if (m == 2) {
        bool leapYear = ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0));
        if (leapYear && d > 29) return false;
        if (!leapYear && d > 28) return false;
    }

    if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30) return false;

    return true;
}