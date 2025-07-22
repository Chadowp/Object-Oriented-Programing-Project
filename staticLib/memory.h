#pragma once

#include <iostream>
#include <string>
#include "Date.h"
#include "Network.h"
#include "post.h"
#include "Comment.h"

class Memory : public Post
{
    Post* origPost;
public:

    Memory(const char* id, const char* text, const Date& currDate, Manager* author, Post* orig);
    void PrintMemory(bool, bool);
    ~Memory();

};
