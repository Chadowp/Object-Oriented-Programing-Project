#pragma once

#include "raylib.h"
#include <iostream>
#include <string>
#include "Network.h"
#include "post.h"
#include "Comment.h"
#include "Date.h"

using namespace std;

class User;
class Page;
class Post;
class Comment;

class SocialMediaApp
{
    const int maxFriends = 10;
    const int maxLikedPages = 10;
    const int maxIDLength = 10;

    User* currentUser;
    int totalNumOfUsers;
    int totalNumOfPages;
    int totalNumOfPosts;
    int totalNumOfComments;

    User** user;
    Page** page;
    Post** post;

public:
    SocialMediaApp();

    void ReadPostFile(ifstream&);
    void ReadCommentsFile(ifstream&, int);

    void SetUsersFriends(char***, int*);
    void SetUsersLikedPages(char***, int*);
    User* findUserByID(const char*);
    Page* findPageByID(const char*);
    Post* findPostByID(const char*);

    void ReadDataFromFile();
    void SetCurrentUser(const char*);
    void ViewPostLikedList(const char*);
    void ViewUserFriendList();
    void ViewLikedPagesList();
    void ViewHomePage();
    void ViewTimeline();
    void ViewPage(const char*);
    void ViewPost(const char*);
    void PrintMemories();
    bool LikePost(const char*);
    bool commentOnPost(const char*, const char*);
    bool ShareMemory(const char*, const char*);
    ~SocialMediaApp();
    void Run(SocialMediaApp* app);

    void ClearMemory(char***& userFriends, int*& noOfFriends, char***& userLikedPages, int*& noOfLikedPages);

    //for gui
    int ViewPostLikedListGUI(const char* postId);
    int DisplayFullPostRaylib(Post* post, bool showLikesOption = true);
    void ViewTimelineRaylib();
    void ViewPageRaylib(Page* page);
};
