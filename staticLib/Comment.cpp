#include "Comment.h"
#include "Network.h"
#include <fstream>
using namespace std;
Comment::Comment(const char* id, const char* body, Manager* creator)
{
    if (id)
    {
        commentId = new char[strlen(id) + 1];
        strcpy_s(commentId, strlen(id) + 1, id);
    }
    else
        commentId = nullptr;

    if (body)
    {
        content = new char[strlen(body) + 1];
        strcpy_s(content, strlen(body) + 1, body);
    }
    else
        content = nullptr;

    author = creator;
}

// reading the comment from file
Comment::Comment(ifstream& file, char* relatedPostId, char* writerId)
{
    author = nullptr;
    char temp[100];

    file >> temp;
    if (strlen(temp) > 0)
    {
        commentId = new char[strlen(temp) + 1];
        strcpy(commentId, temp);
    }
    else
    {
        commentId = nullptr;
    }


    file >> temp;
    if (strlen(temp) > 0)
    {
        strcpy(relatedPostId, temp);
    }


    file >> temp;
    if (strlen(temp) > 0)
    {
        strcpy(writerId, temp);
    }

    file.ignore(); // skip the space after writerId

    file.getline(temp, 100);
    if (strlen(temp) > 0)
    {
        content = new char[strlen(temp) + 1];
        strcpy(content, temp);
    }
    else
    {
        content = nullptr;
    }
}
Comment::~Comment()
{
    delete[] commentId;
    delete[] content;
}

void Comment::assignAuthor(Manager* manager)
{
    author = manager;
}

const Manager* Comment::getAuthor() const
{
    return author;
}


void Comment::display() const
{
    if (author)
    {
        author->showName();
        cout << " wrote: ";
    }

    if (content)
    {
        cout << "'" << content << "'" << endl;
    }
}

//for gui
const char* Comment::getText() const
{
    if (content)
        return content;
    else
        return "";
}
