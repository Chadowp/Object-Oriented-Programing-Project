#include "Comment.h"
#include "Date.h"
#include "memory.h"
#include "Network.h"
#include "post.h"
#include "SocialMediaApp.h"
#include <iostream>
#include <fstream>
#include "raylib.h"

using namespace std;

SocialMediaApp::SocialMediaApp()
{
    totalNumOfUsers = 0;
    totalNumOfPages = 0;
    totalNumOfPosts = 0;
    totalNumOfComments = 0;

    currentUser = nullptr;
    user = nullptr;
    page = nullptr;
    post = nullptr;
}

SocialMediaApp::~SocialMediaApp()
{
    for (int i = 0; i < totalNumOfUsers; i++)
        delete user[i]; //deletes currentUser as well
    delete[] user;

    for (int i = 0; i < totalNumOfPages; i++)
        delete page[i];
    delete[] page;

    if (post)
    {
        for (int i = 0; i < totalNumOfPosts; i++)
        {
            if (post[i]) //there was an error when closing because of 0xFFFFFFF 
            {
                delete post[i];
                post[i] = nullptr;
            }
        }
        delete[] post;
        post = nullptr;
    }

}

void SocialMediaApp::ReadDataFromFile()
{
    ifstream file("user.txt");
    if (!file.is_open()) return;

    totalNumOfUsers = 15;
    char*** userFriends = new char** [totalNumOfUsers];
    int* noOfFriends = new int[totalNumOfUsers];
    char*** userLikedPages = new char** [totalNumOfUsers];
    int* noOfLikedPages = new int[totalNumOfUsers];

    for (int i = 0; i < totalNumOfUsers; i++)
    {
        userFriends[i] = new char* [maxFriends];
        for (int j = 0; j < maxFriends; j++)
            userFriends[i][j] = new char[maxIDLength];

        userLikedPages[i] = new char* [maxLikedPages];
        for (int j = 0; j < maxLikedPages; j++)
            userLikedPages[i][j] = new char[maxIDLength];
    }

    user = new User * [totalNumOfUsers];
    for (int i = 0; i < totalNumOfUsers; i++)
        user[i] = new User(file, userFriends[i], noOfFriends[i], userLikedPages[i], noOfLikedPages[i]);

    file.close();

    file.open("page.txt");
    if (file.is_open())
    {
        totalNumOfPages = 10;
        page = new Page * [totalNumOfPages];
        for (int i = 0; i < totalNumOfPages; i++)
            page[i] = new Page(file);
        file.close();
    }

    SetUsersFriends(userFriends, noOfFriends);
    SetUsersLikedPages(userLikedPages, noOfLikedPages);
    ClearMemory(userFriends, noOfFriends, userLikedPages, noOfLikedPages);

    file.open("post.txt");
    if (file.is_open())
    {
        totalNumOfPosts = 12;
        post = new Post * [totalNumOfPosts];
        ReadPostFile(file);
        file.close();
    }

    file.open("comments.txt");
    if (file.is_open())
    {
        totalNumOfComments = 10;
        ReadCommentsFile(file, totalNumOfComments);
    }
}

void SocialMediaApp::ClearMemory(char***& userFriends, int*& noOfFriends, char***& userLikedPages, int*& noOfLikedPages)
{
    for (int i = 0; i < totalNumOfUsers; i++)
    {
        for (int j = 0; j < maxLikedPages; j++)
            delete[] userLikedPages[i][j];
        delete[] userLikedPages[i];
    }
    delete[] userLikedPages;
    delete[] noOfLikedPages;

    for (int i = 0; i < totalNumOfUsers; i++)
    {
        for (int j = 0; j < maxFriends; j++)
            delete[] userFriends[i][j];
        delete[] userFriends[i];
    }
    delete[] userFriends;
    delete[] noOfFriends;
}

void SocialMediaApp::ReadPostFile(ifstream& file)
{
    file.ignore(100, '\n');

    char** likerIds = new char* [totalNumOfUsers];
    for (int i = 0; i < totalNumOfUsers; i++)
        likerIds[i] = new char[maxIDLength];

    for (int i = 0; i < totalNumOfPosts; i++)
    {
        int likeCount = 0;
        char ownerId[10];

        post[i] = new Post(file, ownerId, likerIds, likeCount);
        Manager* creator = (ownerId[0] == 'u') ? (Manager*)findUserByID(ownerId) : (Manager*)findPageByID(ownerId);

        if (creator)
        {
            creator->uploadPost(post[i]);
            post[i]->setCreator(creator);

            for (int j = 0; j < likeCount; j++)
            {
                Manager* liker = nullptr;
                const char* likerId = likerIds[j];

                if (likerId && likerId[0] == 'u')
                    liker = (Manager*)findUserByID(likerId);
                else if (likerId)
                    liker = (Manager*)findPageByID(likerId);

                if (liker)
                    post[i]->addLike(liker);
            }
        }

        file.ignore();
        file.ignore(100, '\n');
    }

    for (int i = 0; i < totalNumOfUsers; i++)
        delete[] likerIds[i];
    delete[] likerIds;
}

void SocialMediaApp::ReadCommentsFile(ifstream& file, int totalNumOfComments)
{
    for (int i = 0; i < totalNumOfComments; i++)
    {
        char ownerId[10] = "";
        char postId[10] = "";
        Manager* author = nullptr;
        Post* targetPost = nullptr;

        Comment* newComment = new Comment(file, postId, ownerId);

        if (strlen(ownerId) == 0 || strlen(postId) == 0)
        {
            cout << "Invalid post ID or owner ID found in comment entry." << endl;
            delete newComment;
            continue;
        }

        if (ownerId[0] == 'p')
            author = findPageByID(ownerId);
        else if (ownerId[0] == 'u')
            author = findUserByID(ownerId);

        targetPost = findPostByID(postId);

        if (!targetPost)
        {
            cout << "Post not found: " << postId << endl;
            delete newComment;
            continue;
        }

        if (!author)
        {
            cout << "Author not found: " << ownerId << endl;
        }

        
        if (targetPost->addComment(newComment))
        {
            if (author)
                newComment->assignAuthor(author);

            targetPost->incrementCommentCount();
        }
        else
        {
            cout << "Failed to add comment to post: " << postId << endl;
            delete newComment;
        }
    }
}

void SocialMediaApp::SetUsersFriends(char*** userFriends, int* friendsCount)
{
    for (int i = 0; i < totalNumOfUsers; i++)
    {
        for (int j = 0; j < friendsCount[i]; j++)
        {
            User* newFriend = findUserByID(userFriends[i][j]);
            if (newFriend)
                user[i]->addFriend(newFriend);
        }
    }
}

void SocialMediaApp::SetUsersLikedPages(char*** userLikedPages, int* likedPagesCount)
{
    for (int i = 0; i < totalNumOfUsers; i++)
    {
        for (int j = 0; j < likedPagesCount[i]; j++)
        {
            Page* likedPage = findPageByID(userLikedPages[i][j]);
            if (likedPage)
            {
                user[i]->likePage(likedPage);
                likedPage->addFollower(user[i]);
            }
        }
    }
}

User* SocialMediaApp::findUserByID(const char* userId)
{
    if (!userId || userId[0] != 'u') return nullptr;

    for (int i = 0; i < totalNumOfUsers; i++)
    {
        if (user[i] && user[i]->getAccountId() && strcmp(user[i]->getAccountId(), userId) == 0)
            return user[i];
    }

    return nullptr;
}

Page* SocialMediaApp::findPageByID(const char* pageId)
{
    if (!pageId || pageId[0] != 'p')
    {
        cout << "Invalid page ID format: " << pageId << endl;
        return nullptr;
    }

    for (int i = 0; i < totalNumOfPages; i++)
    {
        if (page[i] && strcmp(page[i]->getAccountId(), pageId) == 0)
            return page[i];
    }

    return nullptr;
}

Post* SocialMediaApp::findPostByID(const char* postID)
{
    if (!postID || strncmp(postID, "post", 4) != 0)
        return nullptr;

    for (int i = 0; i < totalNumOfPosts; i++)
    {
        if (post[i] && strcmp(post[i]->getId(), postID) == 0)
            return post[i];
    }

    return nullptr;
}

void SocialMediaApp::SetCurrentUser(const char* userId)
{
    if (!userId || userId[0] != 'u')
    {
        cout << "Error: Invalid user ID format." << endl;
        return;
    }

    currentUser = findUserByID(userId);

    if (currentUser)
    {
        currentUser->showName();
        cout << " successfully set as current user." << endl;
    }
    else
    {
        cout << "Error: User not found." << endl;
    }
}

void SocialMediaApp::ViewHomePage()
{
    if (!currentUser)
    {
        cout << "Please set the current user first." << endl;
        return;
    }

    currentUser->viewHomeFeed();
}
void SocialMediaApp::ViewTimeline()
{
    if (!currentUser)
    {
        cout << "Please set the current user first." << endl;
        return;
    }

    currentUser->viewTimeline();
}

void SocialMediaApp::ViewPostLikedList(const char* postId)
{
    Post* targetPost = findPostByID(postId);

    if (!targetPost)
    {
        cout << "Post not found!" << endl;
        return;
    }

    targetPost->showLikes();
}

bool SocialMediaApp::LikePost(const char* postId)
{
    if (!currentUser)
    {
        cout << "Please set the current user first." << endl;
        return false;
    }

    Post* targetPost = findPostByID(postId);

    if (targetPost)
    {
        cout << postId << " has been liked." << endl;
        return targetPost->addLike(currentUser);
    }

    cout << "Failed to like post: " << postId << endl;
    return false;
}
bool SocialMediaApp::commentOnPost(const char* postID, const char* text)
{
    if (!currentUser)
    {
        cout << "Please set the current user first." << endl;
        return false;
    }

    Post* targetPost = findPostByID(postID);

    if (targetPost)
    {
        string commentID = "c" + to_string(totalNumOfComments++);
        Comment* newComment = new Comment(commentID.c_str(), text, currentUser);

        if (targetPost->addComment(newComment))
        {
            cout << "Comment added." << endl;
            return true;
        }

        delete newComment;
        cout << "Failed to add comment." << endl;
        return false;
    }

    cout << "Post not found." << endl;
    return false;
}

void SocialMediaApp::ViewPost(const char* postId)
{
    Post* targetPost = findPostByID(postId);

    if (targetPost)
    {
        targetPost->display(true);  // true = include date
    }
    else
    {
        cout << "Post not found." << endl;
    }
}

void SocialMediaApp::ViewUserFriendList()
{
    if (!currentUser)
    {
        cout << "Please set the current user first." << endl;
        return;
    }

    currentUser->showFriends();
}

void SocialMediaApp::ViewLikedPagesList()
{
    if (!currentUser)
    {
        cout << "Please set the current user first." << endl;
        return;
    }

    currentUser->showLikedPages();
}



void SocialMediaApp::ViewPage(const char* pageID)
{
    Page* pageObj = findPageByID(pageID);

    if (pageObj)
    {
        pageObj->viewTimeline();
    }
    else
    {
        cout << "Page not found." << endl;
    }
}

void SocialMediaApp::PrintMemories()
{
    if (!currentUser)
    {
        cout << "Please set the current user first." << endl;
        return;
    }

    cout << "\nWe hope you enjoy revisiting your memories!\n" << endl;
    currentUser->printMemories();
}

bool SocialMediaApp::ShareMemory(const char* postId, const char* body)
{
    if (!currentUser)
    {
        cout << "Please set the current user first." << endl;
        return false;
    }

    Post* originalPost = findPostByID(postId);
    if (!originalPost)
    {
        cout << "Post not found." << endl;
        return false;
    }

    if (originalPost->getCreator() != currentUser)
    {
        cout << "You can only share your own memories." << endl;
        return false;
    }

    string newPostId = "post" + to_string(totalNumOfPosts + 1);
    Memory* memoryPost = new Memory(newPostId.c_str(), body, Date::getCurrentDate(), currentUser, originalPost);

    if (!currentUser->uploadPost(memoryPost))
    {
        delete memoryPost;
        cout << "Failed to share memory." << endl;
        return false;
    }

    // Add the memory post to the global post array
    Post** newPostList = new Post * [totalNumOfPosts + 1];
    for (int i = 0; i < totalNumOfPosts; i++)
        newPostList[i] = post[i];

    newPostList[totalNumOfPosts] = memoryPost;
    totalNumOfPosts++;

    delete[] post;
    post = newPostList;

    cout << "Memory shared successfully." << endl;
    return true;
}

void GetRaylibTextInput(const char* prompt, char* outputBuffer, int bufferSize)
{
    outputBuffer[0] = '\0';
    int index = 0;
    bool typing = true;
    char error[100] = "";

    while (typing && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText(prompt, 100, 100, 20, WHITE);
        DrawRectangle(100, 140, 400, 40, LIGHTGRAY);
        DrawText(outputBuffer, 110, 150, 20, BLACK);

        if (error[0] != '\0')
        {
            DrawText(error, 100, 200, 20, RED);
        }

        int key = GetCharPressed();
        while (key > 0 && index < bufferSize - 1)
        {
            if (key >= 32 && key <= 125)
            {
                outputBuffer[index++] = (char)key;
                outputBuffer[index] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && index > 0)
        {
            index--;
            outputBuffer[index] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            if (index == 0)
            {
                strcpy(error, "Input cannot be empty!");
            }
            else
            {
                typing = false;
            }
        }

        EndDrawing();
    }
}

bool GetDateFromUser(int* day, int* month, int* year)
{
    char fields[3][10] = { "", "", "" };  // day, month, year
    const char* labels[3] = { "Day", "Month", "Year" };
    Rectangle boxes[3] = {
        { 100, 150, 100, 40 },
        { 220, 150, 100, 40 },
        { 340, 150, 100, 40 }
    };

    int currentField = 0;
    int indexes[3] = { 0, 0, 0 };
    bool typing = true;
    char error[100] = "";

    while (typing && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Enter date (dd/mm/yyyy):", 100, 100, 20, WHITE);

        for (int i = 0; i < 3; i++)
        {
            DrawRectangleRec(boxes[i], (i == currentField) ? YELLOW : LIGHTGRAY);
            DrawText(labels[i], boxes[i].x, boxes[i].y - 20, 20, WHITE);
            DrawText(fields[i], boxes[i].x + 10, boxes[i].y + 10, 20, BLACK);
        }

        if (error[0] != '\0')
        {
            DrawText(error, 100, 220, 20, RED);
        }

        int key = GetCharPressed();
        while (key > 0 && indexes[currentField] < 9)
        {
            if (key >= '0' && key <= '9')
            {
                fields[currentField][indexes[currentField]++] = (char)key;
                fields[currentField][indexes[currentField]] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && indexes[currentField] > 0)
        {
            indexes[currentField]--;
            fields[currentField][indexes[currentField]] = '\0';
        }

        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_RIGHT))
        {
            currentField = (currentField + 1) % 3;
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            currentField = (currentField + 2) % 3;
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            if (strlen(fields[0]) == 0 || strlen(fields[1]) == 0 || strlen(fields[2]) == 0)
            {
                strcpy(error, "All fields must be filled.");
            }
            else
            {
                *day = atoi(fields[0]);
                *month = atoi(fields[1]);
                *year = atoi(fields[2]);
                return true;
            }
        }

        EndDrawing();
    }

    return false;
}

int SocialMediaApp::ViewPostLikedListGUI(const char* postId)
{
    Post* targetPost = findPostByID(postId);
    if (!targetPost)
    {
        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(DARKGRAY);
            DrawText("Post not found!", 100, 100, 25, RED);
            DrawText("Press ESC or BACKSPACE to return.", 100, 150, 20, WHITE);
            EndDrawing();

            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
                return 1; // return to home
        }
        return 1;
    }

    int scrollY = 0;
    int count = targetPost->getLikerCount();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Post Liked By:", 100, 30, 25, WHITE);

        if (count == 0)
        {
            DrawText("This post has no likes.", 100, 100, 20, RED);
        }
        else
        {
            for (int i = 0; i < count; i++)
            {
                int y = 100 + i * 40 - scrollY;
                if (y > 60 && y < 900)
                {
                    DrawRectangle(100, y, 600, 30, LIGHTGRAY);
                    targetPost->getLiker(i)->showDetailsRaylib(110, y + 5);
                }
            }
        }

        DrawText("Press ESC or BACKSPACE to return to home feed.", 100, 960, 20, GRAY);

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
            return 1;  // tell caller to return to home feed

        if (IsKeyDown(KEY_DOWN)) scrollY += 5;
        if (IsKeyDown(KEY_UP)) scrollY -= 5;
        if (scrollY < 0) scrollY = 0;

        EndDrawing();
    }

    return 1;
}





int SocialMediaApp::DisplayFullPostRaylib(Post* post, bool showLikesOption)
{
    if (!post) return 0;

    const char* postId = post->getId();
    const char* ownerId = "Unknown";

    if (post->getCreator() != nullptr)
    {
        ownerId = post->getCreator()->getAccountId();
    }

    Date date = post->getDate();

    char dateText[32];
    sprintf(dateText, "%02d/%02d/%04d", date.getDay(), date.getMonth(), date.getYear());

    int escCount = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Viewing Post", 100, 30, 30, WHITE);
        DrawText(TextFormat("Post ID: %s", postId), 100, 100, 20, WHITE);
        DrawText(TextFormat("Posted by: %s", ownerId), 100, 130, 20, WHITE);
        DrawText(TextFormat("Date: %s", dateText), 100, 160, 20, WHITE);

        DrawText("Post Content:", 100, 200, 20, WHITE);
        DrawRectangle(100, 230, 600, 100, LIGHTGRAY);
        DrawText(post->getContent(), 110, 250, 20, BLACK);

        DrawText("Comments:", 100, 350, 20, YELLOW);
        int commentY = 380;

        for (int i = 0; i < post->getCommentCount(); i++)
        {
            Comment* c = post->getComment(i);
            if (c != nullptr)
            {
                const Manager* author = c->getAuthor();
                const char* text = c->getText();
                char line[512];

                if (author != nullptr)
                {
                    const char* id = author->getAccountId();

                    if (id != nullptr)
                    {
                        if (id[0] == 'u')
                        {
                            User* user = findUserByID(id);
                            if (user != nullptr && user->getFirstName() != nullptr && user->getLastName() != nullptr)
                            {
                                char fullName[100];
                                strcpy(fullName, user->getFirstName());
                                strcat(fullName, " ");
                                strcat(fullName, user->getLastName());
                                sprintf(line, "- %s: %s", fullName, text);
                            }
                            else
                            {
                                sprintf(line, "- %s: %s", id, text);
                            }
                        }
                        else if (id[0] == 'p')
                        {
                            Page* page = findPageByID(id);
                            if (page != nullptr && page->getTitle() != nullptr)
                            {
                                sprintf(line, "- %s: %s", page->getTitle(), text);
                            }
                            else
                            {
                                sprintf(line, "- Untitled Page: %s", text);
                            }
                        }
                        else
                        {
                            sprintf(line, "- %s: %s", id, text);
                        }
                    }
                    else
                    {
                        sprintf(line, "- Unknown: %s", text);
                    }
                }
                else
                {
                    sprintf(line, "- Unknown: %s", text);
                }

                DrawText(line, 100, commentY, 18, WHITE);
                commentY += 30;
            }
        }

        if (showLikesOption)
        {
            DrawText("Press L to view likes.", 100, 600, 20, SKYBLUE);
        }

        DrawText("Press ESC twice to return, BACKSPACE to go back to feed.", 100, 700, 18, GRAY);

        EndDrawing();

        if (showLikesOption && IsKeyPressed(KEY_L))
        {
            return 1;
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            return 0;
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (escCount == 1)
                return 0;
            else
                escCount = 1;
        }
    }

    return 0;
}



void SocialMediaApp::ViewTimelineRaylib()
{
    if (!currentUser) return;

    int scrollY = 0;
    int totalPosts = currentUser->getPostCount();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("------ TIMELINE ------", 100, 30, 25, WHITE);
        DrawText(TextFormat("User: %s", currentUser->getAccountId()), 100, 70, 20, LIGHTGRAY);

        if (totalPosts == 0)
        {
            DrawText("No posts in timeline.", 100, 120, 20, RED);
        }
        else
        {
            Vector2 mouse = GetMousePosition();

            for (int i = 0; i < totalPosts; i++)
            {
                Post* p = currentUser->getTimelinePost(i);
                int y = 120 + i * 80 - scrollY;

                if (y > 60 && y < 950)
                {
                    Rectangle box = { 100, (float)y, 600, 60 };
                    Rectangle viewBtn = { 620, (float)y + 15, 70, 30 };

                    DrawRectangleRec(box, LIGHTGRAY);
                    DrawText(p->getId(), 110, y + 10, 20, BLACK);
                    DrawRectangleRec(viewBtn, GRAY);
                    DrawText("VIEW", viewBtn.x + 10, viewBtn.y + 5, 20, WHITE);

                    if (CheckCollisionPointRec(mouse, viewBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        int result = DisplayFullPostRaylib(p, false);
                        if (result == 1)
                        {
                            int back = ViewPostLikedListGUI(p->getId());
                        }
                    }
                }
            }

            if (IsKeyDown(KEY_DOWN)) scrollY += 5;
            if (IsKeyDown(KEY_UP)) scrollY -= 5;
            if (scrollY < 0) scrollY = 0;
        }

        DrawText("Press ESC or BACKSPACE to return.", 100, 960, 20, GRAY);
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) break;

        EndDrawing();
    }
}


void SocialMediaApp::ViewPageRaylib(Page* page)
{
    if (!page) return;

    int scrollY = 0;
    int totalPosts = page->getPostCount();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("------ PAGE TIMELINE ------", 100, 30, 25, WHITE);
        DrawText(TextFormat("Page: %s", page->getTitle() ? page->getTitle() : "Untitled"), 100, 70, 20, LIGHTGRAY);

        if (totalPosts == 0)
        {
            DrawText("No posts on this page.", 100, 120, 20, RED);
        }
        else
        {
            Vector2 mouse = GetMousePosition();

            for (int i = 0; i < totalPosts; i++)
            {
                Post* p = page->getTimelinePost(i);
                int y = 120 + i * 80 - scrollY;

                if (y > 60 && y < 950)
                {
                    Rectangle box = { 100, (float)y, 600, 60 };
                    Rectangle viewBtn = { 620, (float)y + 15, 70, 30 };

                    DrawRectangleRec(box, LIGHTGRAY);
                    DrawText(p->getId(), 110, y + 10, 20, BLACK);
                    DrawRectangleRec(viewBtn, GRAY);
                    DrawText("VIEW", viewBtn.x + 10, viewBtn.y + 5, 20, WHITE);

                    if (CheckCollisionPointRec(mouse, viewBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        int result = DisplayFullPostRaylib(p, true);
                        if (result == 1)
                        {
                            int back = ViewPostLikedListGUI(p->getId());
                            if (back == 1)
                                continue;
                        }
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



void SocialMediaApp::Run(SocialMediaApp* App)
{
    InitWindow(1000, 1000, "Social Media Application");
    SetTargetFPS(60);

    const char* menuItems[] = {
        "1. Set current system date",
        "2. Set current user",
        "3. View Home",
        "4. Like Post",
        "5. View Post Liked List",
        "6. Post Comment",
        "7. View Post",
        "8. View Friend List",
        "9. View Timeline",
        "10. View Liked Pages",
        "11. View Memories",
        "12. Share Memory",
        "13. View Page",
        "14. Exit"
    };

    Font font = LoadFont("arial.ttf");

    if (font.texture.id == 0)
    {
        cout << "Failed to load font." << endl;
    }



    Color backgroundColor = DARKGRAY;
    Color textColor = WHITE;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(backgroundColor);

        DrawTextEx(font, "**** Welcome to the App ****", { 250, 30 }, 30, 1, textColor);

        for (int i = 0; i < 14; i++)
        {
            float y = 100 + i * 40;
            DrawTextEx(font, menuItems[i], { 100, y }, 20, 1, textColor);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mouse = GetMousePosition();

                // If the mouse is over this menu item
                if (mouse.x >= 100 && mouse.x <= 900 && mouse.y >= y && mouse.y <= y + 30)
                {
                    switch (i)
                    {
                    case 0: // Set current date
                    {
                        int d, m, y;
                        if (GetDateFromUser(&d, &m, &y))
                        {
                            Date temp;
                            if (temp.isValid(d, m, y))
                            {
                                Date::setCurrentDate(Date(d, m, y));
                                cout << "Date updated: ";
                                Date::getCurrentDate().print();
                                cout << endl;
                            }
                            else
                            {
                                cout << "Invalid date." << endl;
                            }
                        }
                        break;
                    }
                    case 1: // Set current user
                    {
                        char userId[100];
                        GetRaylibTextInput("Enter user ID (u<id>):", userId, sizeof(userId));
                        if (strlen(userId) > 0)
                        {
                            App->SetCurrentUser(userId);
                        }
                        break;
                    }

                    case 2: // View Home
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }

                        Post* posts[20];
                        int count = currentUser->getHomePosts(posts, 20);
                        int scrollY = 0;

                        while (!WindowShouldClose())
                        {
                            BeginDrawing();
                            ClearBackground(DARKGRAY);

                            DrawText("------ HOME FEED ------", 100, 30, 25, WHITE);

                            Vector2 mouse = GetMousePosition();
                            Date current = Date::getCurrentDate();
                            int visiblePostCount = 0;
                            int yIndex = 0;

                            for (int i = 0; i < count; i++)
                            {
                                if (!(posts[i]->getDate() == current))
                                    continue;

                                visiblePostCount++;

                                int y = 100 + yIndex * 80 - scrollY;
                                yIndex++;

                                if (y > 60 && y < 1000)
                                {
                                    Rectangle postBox = { 100, (float)y, 600, 60 };
                                    Rectangle viewButton = { 620, (float)y + 15, 70, 30 };

                                    DrawRectangleRec(postBox, LIGHTGRAY);
                                    DrawText(posts[i]->getId(), 110, y + 10, 20, BLACK);
                                    DrawRectangleRec(viewButton, GRAY);
                                    DrawText("VIEW", viewButton.x + 10, viewButton.y + 5, 20, WHITE);

                                    if (CheckCollisionPointRec(mouse, viewButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                                    {
                                        int result = DisplayFullPostRaylib(posts[i], true);
                                        if (result == 1)
                                        {
                                            int back = ViewPostLikedListGUI(posts[i]->getId());
                                            if (back == 1)
                                                continue;
                                        }
                                    }
                                }
                            }

                            if (visiblePostCount == 0)
                            {
                                DrawText("No current posts.", 100, 100, 20, RED);
                            }

                            DrawText("Press ESC to return.", 100, 600, 20, WHITE);
                            if (IsKeyDown(KEY_DOWN)) scrollY += 5;
                            if (IsKeyDown(KEY_UP)) scrollY -= 5;
                            if (scrollY < 0) scrollY = 0;

                            if (IsKeyPressed(KEY_ESCAPE)) break;

                            EndDrawing();
                        }

                        cout << endl;
                        cout << "---------------------------------------------------------------------------------------------------------------" << endl;
                        cout << "---------------------------------------------------------------------------------------------------------------" << endl;
                        App->ViewHomePage();
                        cin.clear();

                        break;
                    }

                    case 3: // Like Post
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }
                        char postId[100] = "";
                        int inputIndex = 0;
                        bool typing = true;
                        char errorMessage[100] = "";

                        while (typing && !WindowShouldClose())
                        {
                            BeginDrawing();
                            ClearBackground(DARKGRAY);

                            DrawText("Enter post ID (post<id>):", 100, 100, 20, WHITE);
                            DrawRectangle(100, 140, 400, 40, LIGHTGRAY);
                            DrawText(postId, 110, 150, 20, BLACK);

                            if (errorMessage[0] != '\0')
                            {
                                DrawText(errorMessage, 100, 200, 20, RED);
                            }

                            int key = GetCharPressed();
                            while (key > 0 && inputIndex < 99)
                            {
                                if (key >= 32 && key <= 125)
                                {
                                    postId[inputIndex++] = (char)key;
                                    postId[inputIndex] = '\0';
                                }
                                key = GetCharPressed();
                            }

                            if (IsKeyPressed(KEY_BACKSPACE) && inputIndex > 0)
                            {
                                postId[--inputIndex] = '\0';
                            }

                            if (IsKeyPressed(KEY_ENTER))
                            {
                                if (inputIndex == 0)
                                {
                                    strcpy(errorMessage, "Post ID cannot be empty!");
                                }
                                else
                                {
                                    typing = false;
                                }
                            }

                            EndDrawing();
                        }

                        if (inputIndex > 0)
                        {
                            DrawText("------------- LIKING POST -------------", 100, 240, 20, WHITE);
                            App->LikePost(postId);
                        }

                        break;
                    }

                    case 4: // View Post Liked List
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }
                        char postId[100] = "";
                        int inputIndex = 0;
                        bool typing = true;
                        char error[100] = "";

                        // Get post ID input
                        while (typing && !WindowShouldClose())
                        {
                            BeginDrawing();
                            ClearBackground(DARKGRAY);

                            DrawText("Enter post ID (post<id>):", 100, 100, 20, WHITE);
                            DrawRectangle(100, 140, 400, 40, LIGHTGRAY);
                            DrawText(postId, 110, 150, 20, BLACK);

                            if (error[0] != '\0')
                            {
                                DrawText(error, 100, 200, 20, RED);
                            }

                            DrawText("Press ESC to return.", 100, 240, 20, WHITE);

                            if (IsKeyPressed(KEY_ESCAPE))
                            {
                                inputIndex = 0;
                                break;
                            }

                            int key = GetCharPressed();
                            while (key > 0 && inputIndex < 99)
                            {
                                if (key >= 32 && key <= 125)
                                {
                                    postId[inputIndex++] = (char)key;
                                    postId[inputIndex] = '\0';
                                }
                                key = GetCharPressed();
                            }

                            if (IsKeyPressed(KEY_BACKSPACE) && inputIndex > 0)
                            {
                                postId[--inputIndex] = '\0';
                            }

                            if (IsKeyPressed(KEY_ENTER))
                            {
                                if (inputIndex == 0)
                                {
                                    strcpy(error, "Post ID cannot be empty!");
                                }
                                else
                                {
                                    typing = false;
                                }
                            }

                            EndDrawing();
                        }

                        // View likes GUI
                        if (inputIndex > 0)
                        {
                            App->ViewPostLikedListGUI(postId);  
                        }

                        break;
                    }

                    case 5: // Post Comment
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }
                        char postId[100] = "";
                        char commentText[256] = "";
                        int postIndex = 0, commentIndex = 0;
                        int currentBox = 0; // 0 = postId, 1 = comment
                        char error[100] = "";
                        bool typing = true;

                        while (typing && !WindowShouldClose())
                        {
                            BeginDrawing();
                            ClearBackground(DARKGRAY);

                            DrawText("Enter post ID (post<id>):", 100, 80, 20, WHITE);
                            DrawRectangle(100, 110, 600, 40, (currentBox == 0) ? YELLOW : LIGHTGRAY);
                            DrawText(postId, 110, 120, 20, BLACK);

                            DrawText("Enter comment:", 100, 170, 20, WHITE);
                            DrawRectangle(100, 200, 600, 40, (currentBox == 1) ? YELLOW : LIGHTGRAY);
                            DrawText(commentText, 110, 210, 20, BLACK);

                            if (error[0] != '\0')
                                DrawText(error, 100, 270, 20, RED);

                            int key = GetCharPressed();
                            while (key > 0)
                            {
                                if (key >= 32 && key <= 125)
                                {
                                    if (currentBox == 0 && postIndex < 99)
                                    {
                                        postId[postIndex++] = (char)key;
                                        postId[postIndex] = '\0';
                                    }
                                    else if (currentBox == 1 && commentIndex < 255)
                                    {
                                        commentText[commentIndex++] = (char)key;
                                        commentText[commentIndex] = '\0';
                                    }
                                }
                                key = GetCharPressed();
                            }

                            if (IsKeyPressed(KEY_BACKSPACE))
                            {
                                if (currentBox == 0 && postIndex > 0)
                                    postId[--postIndex] = '\0';
                                else if (currentBox == 1 && commentIndex > 0)
                                    commentText[--commentIndex] = '\0';
                            }

                            if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT))
                            {
                                currentBox = (currentBox + 1) % 2;
                            }

                            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT))
                            {
                                currentBox = (currentBox + 1) % 2;
                            }

                            if (IsKeyPressed(KEY_ENTER))
                            {
                                if (postId[0] == '\0' || commentText[0] == '\0')
                                {
                                    strcpy(error, "Both fields must be filled!");
                                }
                                else
                                {
                                    typing = false;
                                }
                            }

                            EndDrawing();
                        }

                        if (postId[0] != '\0' && commentText[0] != '\0')
                        {
                            App->commentOnPost(postId, commentText);
                        }

                        break;
                    }
                    case 6: // View Post 
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }
                        char postId[100] = "";
                        int inputIndex = 0;
                        bool typing = true;
                        char error[100] = "";

                        // Text input screen for post ID
                        while (typing && !WindowShouldClose())
                        {
                            BeginDrawing();
                            ClearBackground(DARKGRAY);

                            DrawText("Enter post ID (post<id>):", 100, 100, 20, WHITE);
                            DrawRectangle(100, 140, 400, 40, LIGHTGRAY);
                            DrawText(postId, 110, 150, 20, BLACK);

                            if (error[0] != '\0')
                            {
                                DrawText(error, 100, 200, 20, RED);
                            }

                            int key = GetCharPressed();
                            while (key > 0 && inputIndex < 99)
                            {
                                if (key >= 32 && key <= 125)
                                {
                                    postId[inputIndex++] = (char)key;
                                    postId[inputIndex] = '\0';
                                }
                                key = GetCharPressed();
                            }

                            if (IsKeyPressed(KEY_BACKSPACE) && inputIndex > 0)
                            {
                                postId[--inputIndex] = '\0';
                            }

                            if (IsKeyPressed(KEY_ENTER))
                            {
                                if (inputIndex == 0)
                                {
                                    strcpy(error, "Post ID cannot be empty!");
                                }
                                else
                                {
                                    typing = false;
                                }
                            }

                            EndDrawing();
                        }

                        // Show the post if input was entered
                        if (inputIndex > 0)
                        {
                            Post* target = App->findPostByID(postId);
                            if (target)
                            {
                                DisplayFullPostRaylib(target, false);
                            }
                            else
                            {
                                // Show not found message
                                while (!WindowShouldClose())
                                {
                                    BeginDrawing();
                                    ClearBackground(DARKGRAY);
                                    DrawText("Post not found!", 100, 100, 25, RED);
                                    DrawText("Press ESC to return.", 100, 150, 20, WHITE);
                                    if (IsKeyPressed(KEY_ESCAPE)) break;
                                    EndDrawing();
                                }
                            }
                        }

                        break;
                    }

                    case 7: // View Friend List
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }

                        currentUser->showFriendsRaylib();
                        break;
                    }

                    case 8: // View Timeline (user's profile)
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }

                        App->ViewTimeline();
                        App->ViewTimelineRaylib();
                        break;
                    }

                    case 9: // View Liked Pages
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }

                        int scrollY = 0;
                        int total = currentUser->getLikedPageCount();

                        while (!WindowShouldClose())
                        {
                            BeginDrawing();
                            ClearBackground(DARKGRAY);

                            DrawText("------ LIKED PAGES ------", 100, 30, 25, WHITE);

                            if (total == 0)
                            {
                                DrawText("You have no liked pages.", 100, 100, 20, RED);
                            }
                            else
                            {
                                for (int i = 0; i < total; i++)
                                {
                                    Page* liked = currentUser->getLikedPage(i);

                                    if (liked)
                                    {
                                        int y = 100 + i * 40 - scrollY;

                                        if (y > 60 && y < 900)
                                        {
                                            const char* title = liked->getTitle();
                                            const char* id = liked->getAccountId();

                                            char buffer[200];
                                            if (title != nullptr)
                                            {
                                                sprintf(buffer, "%s - %s", id, title);
                                            }
                                            else
                                            {
                                                sprintf(buffer, "%s - Untitled Page", id);
                                            }

                                            DrawText(buffer, 100, y, 20, WHITE);
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

                        break;
                    }
                    case 10: // View Memories
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }
                        cout << "\n------------- MEMORIES -------------\n";
                        App->PrintMemories();
                        break;
                    }
                    case 11: // Share Memory
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }
                        char postId[100], postBody[256];
                        cout << "Enter post ID to share (post<id>): ";
                        cin.getline(postId, 100);
                        cout << "Enter memory text: ";
                        cin.getline(postBody, 256);
                        cout << "\n------------- SHARING MEMORY -------------\n";
                        App->ShareMemory(postId, postBody);
                        break;
                    }

                    case 12: // View Page
                    {
                        if (!currentUser)
                        {
                            while (!WindowShouldClose())
                            {
                                BeginDrawing();
                                ClearBackground(DARKGRAY);
                                DrawText("No current user set!", 100, 100, 25, RED);
                                DrawText("Go to 'Set Current User' first.", 100, 140, 20, WHITE);
                                DrawText("Press ESC to return.", 100, 180, 20, WHITE);
                                EndDrawing();

                                if (IsKeyPressed(KEY_ESCAPE)) break;
                            }
                            break;
                        }

                        char pageId[100] = "";
                        int inputIndex = 0;
                        bool typing = true;
                        char error[100] = "";

                        while (typing && !WindowShouldClose())
                        {
                            BeginDrawing();
                            ClearBackground(DARKGRAY);

                            DrawText("Enter page ID (p<id>):", 100, 100, 20, WHITE);
                            DrawRectangle(100, 140, 400, 40, LIGHTGRAY);
                            DrawText(pageId, 110, 150, 20, BLACK);

                            if (error[0] != '\0')
                            {
                                DrawText(error, 100, 200, 20, RED);
                            }

                            DrawText("Press ESC to return.", 100, 240, 20, WHITE);

                            if (IsKeyPressed(KEY_ESCAPE))
                            {
                                inputIndex = 0;
                                break;
                            }

                            int key = GetCharPressed();
                            while (key > 0 && inputIndex < 99)
                            {
                                if (key >= 32 && key <= 125)
                                {
                                    pageId[inputIndex++] = (char)key;
                                    pageId[inputIndex] = '\0';
                                }
                                key = GetCharPressed();
                            }

                            if (IsKeyPressed(KEY_BACKSPACE) && inputIndex > 0)
                            {
                                pageId[--inputIndex] = '\0';
                            }

                            if (IsKeyPressed(KEY_ENTER))
                            {
                                if (inputIndex == 0)
                                {
                                    strcpy(error, "Page ID cannot be empty!");
                                }
                                else
                                {
                                    typing = false;
                                }
                            }

                            EndDrawing();
                        }

                        if (inputIndex > 0)
                        {
                            Page* targetPage = App->findPageByID(pageId);
                            if (targetPage)
                            {
                                App->ViewPage(pageId);
                                App->ViewPageRaylib(targetPage);
                            }
                            else
                            {
                                while (!WindowShouldClose())
                                {
                                    BeginDrawing();
                                    ClearBackground(DARKGRAY);
                                    DrawText("Page not found!", 100, 100, 25, RED);
                                    DrawText("Press ESC to return.", 100, 150, 20, WHITE);
                                    if (IsKeyPressed(KEY_ESCAPE)) break;
                                    EndDrawing();
                                }
                            }
                        }

                        break;
                    }

                    case 13: // Exit
                    {
                        cout << "\nExiting application..." << endl;
                        CloseWindow();
                        return;
                    }
                    } 
                } 
            } 
        } 

        EndDrawing();
    } 

    CloseWindow();
} 



int main()
{
    SocialMediaApp app;
    app.ReadDataFromFile();  
    app.Run(&app);           
    return 0;
}
