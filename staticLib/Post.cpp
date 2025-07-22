#include <iostream>
#include "post.h"
#include "Date.h"
#include "Network.h"
#include <fstream>

using namespace std;

Post::Post(const char* id, const char* text, const Date& d, Manager* creatorRef)
{
    commentList = nullptr;
    likedBy = nullptr;
    likerCount = 0;
    commentCount = 0;
    mood = nullptr;

    postId = id ? strcpy(new char[strlen(id) + 1], id) : nullptr;
    content = text ? strcpy(new char[strlen(text) + 1], text) : nullptr;

    creator = creatorRef;
    postedOn = d;
}

Post::Post(ifstream& file, char* ownerId, char** likerIds, int& likeCount)
{
    likedBy = nullptr;
    likerCount = 0;
    commentList = nullptr;
    commentCount = 0;
    creator = nullptr;

    int type = 0;
    char temp[100];

    file >> type >> temp;
    postId = strlen(temp) > 0 ? strcpy(new char[strlen(temp) + 1], temp) : nullptr;

    postedOn.readFromFile(file);
    file.getline(temp, 100);
    content = strlen(temp) > 0 ? strcpy(new char[strlen(temp) + 1], temp) : nullptr;

    mood = (type == 2) ? new Activity(file) : nullptr;

    file >> ownerId;

    likeCount = 0;
    while (likeCount < maxLikers)
    {
        file >> temp;
        if (strcmp(temp, "-1") == 0) break;
        strcpy_s(likerIds[likeCount++], 10, temp);
    }
}

int Post::getCommentCount() const { return commentCount; }
void Post::incrementCommentCount() { commentCount++; }

Post::~Post()
{
    delete[] postId;
    delete[] content;

    if (commentList)
    {
        for (int i = 0; i < maxComments; i++)
            delete commentList[i];  
        delete[] commentList;
    }

    if (likedBy)
        delete[] likedBy;

    delete mood;
}

Date Post::getDate() const { return postedOn; }

bool Post::addLike(Manager* user)
{
    if (!user) return false;

    if (!likedBy)
    {
        likedBy = new Manager * [maxLikers] {};
    }

    for (int i = 0; i < likerCount; i++)
    {
        if (likedBy[i] == user) return true;
    }

    if (likerCount < maxLikers)
    {
        likedBy[likerCount++] = user;
        return true;
    }

    return false;
}

bool Post::addComment(Comment* c)
{
    if (!c) return false;

    if (!commentList)
    {
        commentList = new Comment * [maxComments];
        for (int i = 0; i < maxComments; i++)
            commentList[i] = nullptr;
    }

    if (commentCount < maxComments)
    {
        commentList[commentCount++] = c;
        return true;
    }
    return false;

}

void Post::showContent()
{
    cout << "'" << content << "'";
}

void Post::display(bool includeDate, bool includeComments)
{
    if (creator)
        creator->showName();
    else
        cout << " unknown owner";

    if (mood)
    {
        mood->display();
        cout << "    ";
    }
    else
    {
        cout << " shared ";
    }

    showContent();

    if (includeDate)
    {
        cout << "...";
        postedOn.print();
    }

    cout << endl;

    if (includeComments)
    {
        showComments();
    }
}

void Post::showComments()
{
    for (int i = 0; i < commentCount; i++)
    {
        if (commentList[i])
        {
            cout << endl << endl;
            commentList[i]->display();
        }
    }

    cout << endl;
}

void Post::setCreator(Manager* m) { creator = m; }
Manager* Post::getCreator() const { return creator; }
char* Post::getId() const { return postId; }

void Post::showLikes() const
{
    cout << endl << "Post Liked By:" << endl;
    for (int i = 0; i < likerCount; i++)
    {
        likedBy[i]->showDetails();
    }
}


//for gui
int Post::getLikerCount() const
{
    return likerCount;
}

Manager* Post::getLiker(int index) const
{
    if (index >= 0 && index < likerCount)
        return likedBy[index];
    return nullptr;
}

const char* Post::getContent() const
{
    if (content)
        return content;
    else
        return "No content.";
}

Comment* Post::getComment(int index) const
{
    if (index >= 0 && index < commentCount)
        return commentList[index];
    return nullptr;
}
