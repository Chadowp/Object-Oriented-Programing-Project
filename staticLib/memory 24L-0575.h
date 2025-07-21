#pragma once

#include <iostream>
#include <string>
#include "Date 24L-0575.h"
#include "Network 24L-0575.h"
#include "post 24L-0575.h"
#include "Comment 24L-0575.h"

class Memory : public Post
{
    Post* origPost;
public:

    Memory(const char* id, const char* text, const Date& currDate, Manager* author, Post* orig);
    void PrintMemory(bool, bool);
    ~Memory();

};
