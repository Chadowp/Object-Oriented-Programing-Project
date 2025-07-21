#pragma once
using namespace std;
#include <fstream>

class Manager;
class Post;

class Date
{
    int day;
    int month;
    int year;
    static Date currentDate;

public:
    static void setCurrentDate(const Date&);
    static Date getCurrentDate();

    Date(int d = 12, int m = 1, int y = 2024);

    void readFromFile(ifstream&);

    bool operator<=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator>=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator==(const Date& other) const;

    void print() const;

    int getDay() const;
    int getMonth() const;
    int getYear() const;

    bool isValid(int d, int m, int y) const;
}; 
