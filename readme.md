**Bibliotheque** – Library Management System

This is a mini project for managing a library, developed as part of a data structures course.
It demonstrates the use of structures, pointers, and file operations in C, with a simple ncurses-based terminal interface.

# Features:
- Add, list, and search for books
- Register and list students
- Borrow and return books
- Persistent storage using text files
- Simple, scrollable ncurses UI

# run on windows 
```
    gcc main.c storagemanagement.c -I ".\include\" -L ".\lib\" -l:pdcurses.a -o bibliotheque
```
# run on mac/linux
```
    gcc main.c storagemanagement.c -lncurses -o bibliotheque
```
# Usage
- Run the compiled executable.
- Navigate the menu using the keyboard to manage books and students.