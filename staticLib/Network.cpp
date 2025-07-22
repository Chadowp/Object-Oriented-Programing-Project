#include "Network.h"
#include "post.h"
#include "Date.h"
#include <iostream>
#include <fstream>
#include "raylib.h"

using namespace std;


Manager::Manager(ifstream& file)
{
    managerId = nullptr;
    postTimeline = nullptr;
    totalPosts = 0;

    char temp[10];
    file >> temp;
    file.ignore();

    int len = strlen(temp);
    if (len > 0)
    {
        managerId = new char[len + 1];
        strcpy_s(managerId, len + 1, temp);
    }
}

Manager::~Manager()
{
    delete[] managerId;

    if (postTimeline)
    {
        for (int i = 0; i < totalPosts; i++)
            delete postTimeline[i];
        delete[] postTimeline;
    }
}

bool Manager::uploadPost(Post* p)
{
    if (!postTimeline)
    {
        postTimeline = new Post * [maxTimelinePosts] {};
    }

    if (totalPosts >= maxTimelinePosts)
        return false;

    int index = 0;
    while (index < totalPosts&& postTimeline[index] && postTimeline[index]->getDate() >= p->getDate())
        index++;

    for (int i = totalPosts; i > index; i--)
        postTimeline[i] = postTimeline[i - 1];

    postTimeline[index] = p;
    totalPosts++;

    return true;
}

Post* Manager::getLatestPost()
{
    Post* latest = nullptr;

    for (int i = 0; i < totalPosts; i++)
    {
        Post* current = postTimeline[i];

        if (current->getDate() <= Date::getCurrentDate() &&
            (!latest || latest->getDate() < current->getDate()))
        {
            latest = current;
        }
    }

    return latest;
}

const char* Manager::getAccountId() const
{
    return managerId;
}

void Manager::viewTimeline() const
{
    showName();
    cout << " ~ timeline" << endl << endl;

    for (int i = 0; i < totalPosts; i++)
    {
        postTimeline[i]->display(true);
    }
}

void Manager::showDetails() const
{
    cout << managerId << "  ";
    showName();
    cout << endl;
}

void Manager::printMemories() const
{
    bool hasMemory = false;
    cout << "on this day" << endl;

    for (int i = 0; i < totalPosts; i++)
    {
        if (postTimeline[i]->getDate().getDay() == Date::getCurrentDate().getDay() &&
            postTimeline[i]->getDate().getMonth() == Date::getCurrentDate().getMonth() &&
            postTimeline[i]->getDate().getYear() < Date::getCurrentDate().getYear())
        {
            cout << Date::getCurrentDate().getYear() - postTimeline[i]->getDate().getYear()
                << " years ago" << endl;

            postTimeline[i]->display(true);
            hasMemory = true;
        }
    }

    if (!hasMemory)
        cout << "no memories to view" << endl;
}

Page::Page(ifstream& file) : Manager(file)
{
    char temp[100];
    file.getline(temp, 100);
    int len = strlen(temp);

    title = (len > 0) ? strcpy(new char[len + 1], temp) : nullptr;

    followers = nullptr;
    followerCount = 0;
}

Page::~Page()
{
    delete[] title;

    if (followers)
        delete[] followers;
}

bool Page::addFollower(Manager* m)
{
    if (!m)
        return false;

    if (!followers)
    {
        followers = new Manager * [maxFollowers] {};
    }

    if (followerCount < maxFollowers)
    {
        followers[followerCount++] = m;
        return true;
    }

    return false;
}

void Page::showName() const
{
    if (title)
        cout << title;
}

User::User(ifstream& file, char** friends, int& friendCountRef, char** likedPagesArray, int& likedPageCountRef)
    : Manager(file)
{
    this->friendList = nullptr;
    this->likedPages = nullptr;
    friendCount = 0;
    likedPageCount = 0;

    loadData(file, friends, friendCountRef, likedPagesArray, likedPageCountRef);
}

User::~User()
{
    delete[] firstName;
    delete[] lastName;

    if (friendList)
        delete[] friendList;
    if (likedPages)
        delete[] likedPages;
}

void User::loadData(ifstream& file, char** friends, int& friendCountRef, char** likedPagesArray, int& likedPageCountRef)
{
    char temp[100];
    file >> temp;

    firstName = strcpy(new char[strlen(temp) + 1], temp);
    file >> temp;
    lastName = strcpy(new char[strlen(temp) + 1], temp);

    friendCountRef = 0;
    while (friendCountRef < maxFriendsAllowed)
    {
        file >> temp;
        if (strcmp(temp, "-1") == 0) break;
        strcpy_s(friends[friendCountRef++], maxFriendsAllowed, temp);
    }

    likedPageCountRef = 0;
    while (likedPageCountRef < maxLikedPagesAllowed)
    {
        file >> temp;
        if (strcmp(temp, "-1") == 0) break;
        strcpy_s(likedPagesArray[likedPageCountRef++], maxLikedPagesAllowed, temp);
    }
}

bool User::addFriend(User* u)
{
    if (!u) return false;

    if (!friendList)
        friendList = new User * [maxFriendsAllowed] {};

    if (friendCount < maxFriendsAllowed)
    {
        friendList[friendCount++] = u;
        return true;
    }

    return false;
}

bool User::likePage(Page* p)
{
    if (!p) return false;

    if (!likedPages)
        likedPages = new Page * [maxLikedPagesAllowed] {};

    if (likedPageCount < maxLikedPagesAllowed)
    {
        likedPages[likedPageCount++] = p;
        return true;
    }

    return false;
}

void User::showName() const
{
    cout << firstName << " " << lastName;
}

void User::showFriends() const
{
    showName();
    cout << " ~ Friends" << endl;
    for (int i = 0; i < friendCount; i++)
        friendList[i]->showDetails();
}

void User::showLikedPages() const
{
    showName();
    cout << " ~ Liked Pages" << endl;
    for (int i = 0; i < likedPageCount; i++)
        likedPages[i]->showDetails();
}

void User::viewHomeFeed() const
{
    showName();
    cout << " ~ Home Feed" << endl;

    for (int i = 0; i < friendCount; i++)
    {
        Post* latest = friendList[i]->getLatestPost();
        if (latest)
            latest->display();
    }

    for (int i = 0; i < likedPageCount; i++)
    {
        Post* latest = likedPages[i]->getLatestPost();
        if (latest)
            latest->display();
    }
}

int User::getFriendCount() const { return friendCount; }
int User::getLikedPageCount() const { return likedPageCount; }



//for displaying home in the gui
int User::getHomePosts(Post** outputList, int maxCount) const
{
    int count = 0;

    for (int i = 0; i < friendCount && count < maxCount; i++)
    {
        Post* latest = friendList[i]->getLatestPost();
        if (latest)
            outputList[count++] = latest;
    }

    for (int i = 0; i < likedPageCount && count < maxCount; i++)
    {
        Post* latest = likedPages[i]->getLatestPost();
        if (latest)
            outputList[count++] = latest;
    }

    return count;
}

void User::showDetailsRaylib(int x, int y) const
{
    if (getAccountId())
    {
        char display[200];
        sprintf(display, "User: %s (%s %s)", getAccountId(), firstName ? firstName : "", lastName ? lastName : "");
        DrawText(display, x, y, 20, BLACK);
    }
}

void Page::showDetailsRaylib(int x, int y) const
{
    if (getAccountId())
    {
        char display[200];
        sprintf(display, "Page: %s (%s)", getAccountId(), title ? title : "Untitled");
        DrawText(display, x, y, 20, BLACK);
    }
}

User* User::getFriend(int index) const
{
    if (index >= 0 && index < friendCount)
        return friendList[index];
    return nullptr;
}

const char* User::getFirstName() const
{
    return firstName;
}

const char* User::getLastName() const
{
    return lastName;
}

void User::showFriendsRaylib() const
{
    int scrollY = 0;
    int total = getFriendCount();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("------ FRIEND LIST ------", 100, 30, 25, WHITE);

        if (total == 0)
        {
            DrawText("You have no friends added.", 100, 100, 20, RED);
        }
        else
        {
            for (int i = 0; i < total; i++)
            {
                User* f = getFriend(i);
                if (f)
                {
                    int y = 100 + i * 40 - scrollY;
                    if (y > 60 && y < 900)
                    {
                        char line[200];
                        sprintf(line, "%s - %s %s", f->getAccountId(),
                            f->getFirstName() ? f->getFirstName() : "",
                            f->getLastName() ? f->getLastName() : "");
                        DrawText(line, 100, y, 20, WHITE);
                    }
                }
            }

            if (IsKeyDown(KEY_DOWN)) scrollY += 5;
            if (IsKeyDown(KEY_UP)) scrollY -= 5;
            if (scrollY < 0) scrollY = 0;
        }

        DrawText("Press ESC to return.", 100, 960, 20, GRAY);

        if (IsKeyPressed(KEY_ESCAPE)) break;

        EndDrawing();
    }
}

int User::getPostCount() const
{
    return totalPosts;
}

Post* User::getTimelinePost(int index) const
{
    if (index >= 0 && index < totalPosts)
        return postTimeline[index];
    return nullptr;
}

const char* Page::getTitle() const
{
    if (title != nullptr)
        return title;
    else
        return nullptr;
}

Page* User::getLikedPage(int index) const
{
    if (index >= 0 && index < likedPageCount)
        return likedPages[index];
    return nullptr;
}

int Page::getPostCount() const
{
    return totalPosts;
}

Post* Page::getTimelinePost(int index) const
{
    if (index >= 0 && index < totalPosts)
        return postTimeline[index];
    return nullptr;
}
