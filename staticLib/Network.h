#pragma once

#include <string>
#include <fstream>
#include "Date.h"
#include "post.h"
#include "Comment.h"

using namespace std;

class User;
class Page;
class Post;
class Comment;

class Manager
{
protected:
    char* managerId;
    const int maxTimelinePosts = 10;
    Post** postTimeline;
    int totalPosts;

public:
    Manager(ifstream&);
    virtual ~Manager();

    bool uploadPost(Post* newPost);
    Post* getLatestPost();
    const char* getAccountId() const;

    void viewTimeline() const;
    void printMemories() const;

    virtual void showName() const = 0;
    virtual void showDetails() const;

    //for gui
    virtual void showDetailsRaylib(int x, int y) const = 0;
};

class Page : public Manager
{
    static const int maxFollowers = 10;
    char* title;
    int followerCount;
    Manager** followers;

public:
    Page(ifstream&);
    ~Page();

    bool addFollower(Manager*);
    void showName() const override;

    //for gui
    void showDetailsRaylib(int x, int y) const override;
    const char* getTitle() const;
    int getPostCount() const;
    Post* getTimelinePost(int index) const;
};

class User : public Manager
{
    char* firstName;
    char* lastName;
    const int maxFriendsAllowed = 10;
    const int maxLikedPagesAllowed = 10;

    Page** likedPages;
    int likedPageCount;

    User** friendList;
    int friendCount;
    

public:
    User(ifstream&, char**, int&, char**, int&);
    ~User();

    void loadData(ifstream&, char**, int&, char**, int&);
    bool addFriend(User*);
    bool likePage(Page*);

    void showName() const override;
    void showFriends() const;
    void showLikedPages() const;
    void viewHomeFeed() const;

    int getFriendCount() const;
    int getLikedPageCount() const;

    //for displaying home feed in gui
    int getHomePosts(Post** outputList, int maxCount) const;
    void showDetailsRaylib(int x, int y) const override;
    User* getFriend(int index) const;
    const char* getFirstName() const;
    const char* getLastName() const;
    void showFriendsRaylib() const;
    int getPostCount() const;
    Post* getTimelinePost(int index) const;
    Page* getLikedPage(int index) const;
};
