#pragma once

#include <iostream>
#include <string>
#include "Network 24L-0575.h"
#include "Date 24L-0575.h"

class Post;

class Comment
{
    char* commentId;
    char* content;
    Manager* author;

public:
    Comment(const char* id, const char* body, Manager* creator);
    ~Comment();
    Comment(ifstream& file, char* relatedPostId, char* writerId);


    void assignAuthor(Manager* manager);
    const Manager* getAuthor() const;
    void display() const;



    //for gui
    const char* getText() const;
};
