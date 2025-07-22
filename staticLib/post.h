#pragma once

#include <string>
#include <fstream>
#include "Date.h"
#include "activity.h"
#include "Comment.h"

class Manager;
class Comment;
class Activity;

class Post
{
    static const int maxComments = 10;
    static const int maxLikers = 10;

    char* postId;
    char* content;

    int likerCount;
    Manager** likedBy;

    Date postedOn;

    Comment** commentList;
    int commentCount;

    Activity* mood;
    Manager* creator;


protected:
    void showComments();
    void showContent();

public:
    Post(const char* id, const char* text, const Date& d, Manager* owner);
    Post(std::ifstream& input, char* ownerId, char** likerIds, int& totalLikes);
    virtual ~Post();

    Date getDate() const;
    bool addLike(Manager*);
    bool addComment(Comment*);
    virtual void display(bool withDate = false, bool withComments = true);
    void setCreator(Manager*);
    Manager* getCreator() const;
    char* getId() const;
    void showLikes() const;
    int getCommentCount() const;
    void incrementCommentCount();

    //for gui
    int getLikerCount() const;
    Manager* getLiker(int index) const;
    const char* getContent() const;
    Comment* getComment(int index) const;
};


