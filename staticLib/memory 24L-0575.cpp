#include <iostream>
#include "memory 24L-0575.h"

using namespace std;

Memory::Memory(const char* id, const char* text, const Date& currentDate, Manager* author, Post* orig)
    : Post(id, text, currentDate, author)
{
    origPost = orig;
}

void Memory::PrintMemory(bool includeDate, bool includeComments)
{
    cout << "----- ";
    getCreator()->showName();  // name of the owner of the memory
    cout << " shared a memory ---- ...";

    if (includeDate)
    {
        getDate().print();
    }

    cout << endl;

    showContent();
    cout << endl << '\t' << '(';
    cout << Date::getCurrentDate().getYear() - origPost->getDate().getYear() << " Years Ago)" << endl;

    origPost->display(true, false);  // original post (with date, no comments)

    if (includeComments)
    {
        showComments();
    }
}

Memory::~Memory()
{
    origPost = nullptr; //already being deleted when destructing post
}