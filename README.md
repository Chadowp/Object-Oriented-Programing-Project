# Social Network App (C++ with Raylib) – OOP Project

A graphical social network simulation built in C++ using Raylib and object-oriented design principles. This solo project was developed as part of the **CS1004 Object-Oriented Programming** course at **FAST NUCES Lahore**.

## Project Overview

The application simulates a simplified social network where users can:
- View posts from friends and liked pages
- Like and comment on posts
- Share memories
- View timelines, friends, and pages

Text is rendered using the `Arial.ttf` font via Raylib. All functionality is GUI-based and dynamically updated based on user commands and file data.

---

## OOP Design Highlights

- Full object-oriented structure with `.h` and `.cpp` files
- No global variables or functions
- Dynamically allocated arrays (using double pointers where needed)
- Proper memory deallocation
- All commands handled through class-based design (`SocialMediaApp::Run()`)
- No direct logic inside `main()`

---

## Raylib Setup

To run this project, you need Raylib installed and configured for Visual Studio.

The easiest way is to use the official quickstart template:

    https://github.com/raylib-extras/raylib-quickstart

Steps:

    1.Download or clone the raylib-quickstart repo.

    2.Follow its instructions to set up Raylib in Visual Studio.

    3.Once it's working, you can copy this project into the template structure or open game-premake-main.sln directly (if Raylib is globally set up).

Raylib is licensed under the zlib/libpng license and works out of the box with minimal configuration.


## 📁 Project Structure
```plaintext
social-network-raylib/
├── game/
│   └── src/
│       └── main.cpp                      # Entry point
├── raylib-master/
│   └── src/
│       ├── activity 24L-0575.cpp/.h
│       ├── Comment 24L-0575.cpp/.h
│       ├── Date 24L-0575.cpp/.h
│       ├── memory 24L-0575.cpp/.h
│       ├── Network 24L-0575.cpp/.h
│       ├── Post 24L-0575.cpp/.h
│       └── SocialMediaApp 24L-0575.cpp/.h
├── game-premake-main.sln                # Visual Studio solution file
├── arial.ttf                            # Font for Raylib rendering
├── page.txt                             # Page metadata
├── comments.txt                         # Post comment data
├── .gitignore
└── README.md

## Features Implemented

    Set system date & current user

    View posts by friends and liked pages (last 24h)

    Like and comment on posts (max 10 likes)

    View comments and liked lists

    Share memories with optional text

    View timeline, friend list, and liked pages

    Render all post elements and UI using Raylib

    Note: This project was completed under a tight deadline, so some functionality still uses console-based I/O alongside the Raylib interface. All core features and logic are fully functional and implemented.

## Technologies Used

    -C++ (OOP)

    -Raylib (GUI rendering)

    -Dynamic memory management

    -Text file input for users, pages, posts, comments

    -TrueType font rendering (Arial.ttf)

##This was a solo project developed by:
	Laiba Fida